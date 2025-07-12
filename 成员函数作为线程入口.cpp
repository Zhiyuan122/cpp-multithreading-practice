/*
| 方面        |            全局函数为线程入口                                |                成员函数作为线程入口                                     |
| 线程函数类型|  使用 普通全局函数/ 静态成员函数                                 | 使用非静态成员函数                                          |
| 函数绑定方式| `std::thread(ThreadMain, args...)`<br>（直接传函数名和参数） | std::thread(&Class::Func, &obj)`<br>（函数指针 + 对象指针） |
| 参数传递    |    通过参数显式传入（值传、指针、引用）                      | 通过类成员变量隐式传入，无需额外参数                                 |
| 生命周期控制|  参数生命周期需手动管理（特别是指针和引用传递）              | 对象生命周期自动绑定线程生命周期                                   |
| 重点知识点  |   参数传递方式（值、指针、引用）<br>生命周期安全问题         | 成员函数调用规则<br>this 指针绑定<br>线程封装的设计思想                 |

二者关系：
1. 都基于 C++11 的 std::thread 线程库进行线程创建与管理
2. 生命周期管理：全局函数强调参数生命周期，成员函数强调用类封装统一管理生命周期（更安全、更清晰）
3. 全局函数强调参数传递方式及风险；
   成员函数强调用类成员替代参数传递，简化逻辑并支持封装
4. 函数级线程调用——基础练习
   类级线程封装——工程化提升


核心：
	如何用 成员函数（非静态）作为线程入口，
	并进一步封装一个线程基类（XThread），
	实现工程化管理线程生命周期（启动、退出、等待）

静态成员函数	thread(Class::StaticFunc)	和普通函数一样，没有 this 指针问题
非静态成员函数	thread(&Class::Func, &obj)	必须提供对象地址（this 指针）+ 函数指针组合传入
			第一个参数是 成员函数指针，第二个参数是 对象地址

非静态成员函数：需要依赖对象实例才能调用的函数
class Person{
public:
	string name;

	void sayHi(){
		cout << "hi" << name << endl; //sayHi() 就是一个非静态成员函数。
	}
};
int main(){
	Person P;
	p.name = "tom";
	p.sayHi();  //必须通过对象来调用
	Person::sayHi();  // 错误，因为它不是 static 函数
}
静态成员函数static member function 指不依赖对象实例就能调用的函数
class Person {
public:
	static void greet() {
		cout << "Hello!" << endl;
	}
};

Person::greet();  //  正确 不需要任何对象就调用：

|比较点                |        非静态成员函数                  |          静态成员函数                  |
|是否依赖对象实例      |  需要对象（有 `this` 指针）            |  不需要对象                            |
|是否能访问成员变量    | 可以访问所有成员变量和函数             |  只能访问 `static` 成员                |
|如何调用              |              obj.func()                | `ClassName::func()` 或 `obj.func()`    |
|是否能作为线程入口函数| 不能直接用在 `std::thread`（要带对象） | 可以直接用在 `std::thread`             |

非静态成员是依赖对象的，它们保存在对象的内存中。而静态函数不依赖对象、没有 this 指针，所以无法知道该访问哪一个对象的成员变量
非静态成员函数，它依赖 this 指针（即：它是“属于对象”的函数）

想用非静态成员函数作为线程入口，必须显式传入对象指针：
thread th(&MyClass::func, &myObj);  // 必记
静态成员函数因为不需要对象，可以直接传
thread th(&MyClass::staticFunc);   // 可以这么写但是较少

线程入口函数---线程一开始运行的函数



+------------------------------+
|       主线程（main）        |
|-----------------------------|
| 执行到 thread th(func);     |
| 调用 pthread_create         |
| -> 分配 TCB、栈              |
| -> 设置入口 PC = func       |
| -> 加入就绪队列             |
+------------------------------+

+------------------------------+
|       子线程（func）         |
|-----------------------------|
| 单独的栈（1MB）              |
| 自己的 TCB                   |
| 独立调度、与主线程并行执行   |
+------------------------------+
对于thread th(func);
语言层 → 库层 → 操作系统层的理解：

一、语言层面
	thread th(func);
	创建了一个 新线程对象 th，它会在后台开一个新线程，让它从 func() 开始执行
	主线程和这个新线程是并行执行的

二、C++ 标准库层面（std::thread）
	std::thread → 调用操作系统 API → 创建真实的线程

三、操作系统层面理解（重点）
	
	1.分配线程控制块（TCB）
		操作系统为这个新线程分配一个 线程控制块（Thread Control Block）
		里面包含线程 ID、寄存器状态、栈指针、程序计数器等
	2：分配线程栈空间
	    每个线程都有自己的「用户栈（stack）」
		操作系统会为这个新线程分配一个独立的栈区，比如 1MB
		用来存放局部变量、函数调用链、返回地址等
	3：设置线程入口地址
		把线程的**入口函数指针（这里是 func）**写入到 TCB 的 PC（程序计数器）字段
		当操作系统调度这个线程时，就从这个函数开始执行
	4：插入到调度器就绪队列中
		新线程处于 ready 状态，等待操作系统的调度器（scheduler）分配 CPU
		一旦轮到它执行，CPU 会跳转到 func() 开始执行逻辑
		
 Summary: 让操作系统为你开一个全新的执行轨道（线程），
		从 func() 开始运行，拥有独立的栈空间、执行状态，由调度器决定何时执行。

*/

//简单版：用一个普通类的成员函数当线程入口
//封装版：写一个线程基类 XThread，以后只要继承它就能跑线程


#include <iostream>
#include <string>
#include <thread>
using namespace std;

//第一步：写一个类 MyThread
class MyThread {
public:
	string name;
	int age = 100;

	void main() {
		cout << "MyThread running, name = " << name << ", age = " << age << endl;
	}

};

//第3步：创建一个线程基类 XThread
//封装好线程的创建、等待和退出逻辑，你只要继承它并实现一个 Main() 函数就能跑线程。

class XThread {
public:
	// 启动线程
	virtual void start(){// why virtual? 之前怎么不见start启动线程
		is_exit_ = false; // 什么命名规范
		th_ = thread(&XThread::Main, this); // 没看懂 把当前对象传进去，用成员函数作为线程入口
	}
	// 通知线程退出
	virtual void Stop() {
		is_exit_ = true;
		Wait(); // why wait here?
	
	}
	//等待线程退出
	virtual void Wait() { //啥意思，怎么知道这么写的
		{
			th_.join;
		}
	}
	
	// 提供一个接口，给子类用来检查是否该退出了？？？？？
	bool is_exit() {
		return is_exit_;
	}
protected:
	virtual void Main() = 0; // 留给子类实现的线程函数，纯虚函数
private:
	thread th_;             // 线程对象
	bool is_exit_ = false;  // 退出标志
};
/*
Main() 是纯虚函数：子类必须实现这个函数。

线程启动使用的是成员函数 &XThread::Main 和 this。

is_exit_ 是一个标志变量，用于控制线程是否应该退出。
*/

//第4步:写一个继承 XThread 的类，比如 TestXThread
class TestXThread : public XThread
{
public:
	TestXThread : public XThread();
	~TestXThread : public XThread(); //这俩默认生成的干啥的还有用吗还保留吗

	void Main() override { //override???
		cout << "TestXThread Main begin" << endl;
		while (!is_exit()) {
			this_thread::sleep_for(100ms);  // 让线程睡 100 毫秒
			cout << "." << flush;           // 打一个点，刷新输出缓冲
		}
		cout << "\nTestXThread Main end" << endl;
	}

};
/*
Main() 是子类中实现的线程入口函数。

通过 !is_exit() 控制循环是否继续。

sleep_for 可以模拟“线程正在工作”。
*/
TestXThread : public XThread::TestXThread : public XThread()
{
}

TestXThread : public XThread::~TestXThread : public XThread()
{
}












// 第4步：在主函数中使用这个线程类
//第二步：在主函数中创建对象，并启动线程
int main() {
	MyThread t;
	t.name = "Thread A";
	t.age = 20;

	//注意：成员函数当线程入口，需要传入 &类名::成员函数, 对象指针
	thread th(&MyThread::main, &t);

	th.join();

	TestXThread test_thread;
	test_thread.Start();   // 启动线程

	this_thread::sleep_for(3s); // 主线程睡 3 秒
	test_thread.Stop();    // 通知子线程退出并等待它结束

	getchar();  // 卡住主线程，防止直接退出






	return 0;


}