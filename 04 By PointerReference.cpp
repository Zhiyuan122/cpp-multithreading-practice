/*
 线程函数参数传递的三种方式
 | 方式                 | 示例                     | 是否拷贝    | 风险点                             |
| 值传递（By Value）      | `thread(func, p)`      | 自动拷贝 | 安全（会复制一份）                       |
| 指针传递（By Pointer）   | `thread(func, &p)`     |  不拷贝   | 若原变量被释放，子线程访问将出错               |
| 引用传递（By Reference） | `thread(func, ref(p))` |  不拷贝   | 必须用`ref()`标明，否则模板匹配错误，也有生命周期风险 |

1. 值传递方式（thread(ThreadMain, p)）------多线程函数默认是“值传递”
创建 Para 对象并赋值。
主线程传值给子线程，thread 内部通过 模板推导 + 参数复制。
生命周期安全：子线程拿到的是 p 的拷贝，哪怕主线程的 p 被销毁也不影响。
会触发 Copy Constructor 多次（Create → Copy → Drop ×3）

2. 指针传递方式（thread(ThreadMainPtr, &p)）
p 是在主线程栈上创建的对象，传入地址（指针）给子线程。
如果使用 detach()，子线程可能在 p 被销毁后才访问它 → 会访问到非法内存。
关键：
不会触发 copy，但必须确保 p 活着时子线程才能访问它。
最危险的是 detach() + 栈变量地址

3. 引用传递方式（thread(ThreadMainRef, ref(p))）
也不会触发拷贝，性能更好。
但和指针一样，如果主线程先结束引用对象，子线程访问就会出错。
要用 std::ref() 明确告诉模板函数：我传的是引用！

关键：
模板无法自动识别引用，要加 ref()。
生命周期必须小于线程执行时间，否则出错。

summary:
1. 多线程函数默认是“值传递”，编译器自动复制参数
2. 指针和引用不会复制原数据，所以要 确保原数据不提前销毁
3. 如果传引用，必须用 std::ref() 包裹参数
4. 若线程函数访问的数据生命周期较长，可以考虑用堆分配或类成员变量


*/

#include<iostream>
#include<thread>
#include<string>
using namespace std;

//第一步：创建 Para 类，用来追踪对象生命周期
class Para {
public:
	string	name;
	Para() {
		cout << "Create Para" << endl;
	}
	Para(const Para& p) {
		cout << "copy Para" << endl;
	}
	~Para() {
		cout << "drop Para" << endl;
	}

};


//第二步：写线程函数 ThreadMain，采用值传递（by value）
void ThreadMain(int p1, float p2, string str, Para p4) {
	this_thread::sleep_for(chrono::milliseconds(100));// why seep?
	cout << "ThreadMain:" << p1 << " " << p2 << " " << str << " " << p4.name << endl; //  p4.name    p4 OK?

}

//第四步：写线程函数 ThreadMainPtr，采用指针传递
void ThreadMainPtr(Para* p) {
	this_thread::sleep_for(chrono::milliseconds(100));
	cout << "ThreadMainPtr name = " << p->name << endl;
}

//第六步：写线程函数 ThreadMainRef，采用引用传递
void ThreadMainRef(Para& p) {
	this_thread::sleep_for(chrono::milliseconds(100));
	cout << "ThreadMainRef name = " << p.name << endl; // 是不是和普通函数一个逻辑，如果入参是Para* p，则输出 p->name ，如果入参是Para& p的引用格式，出参数则是p.name

}





//第三步：测试值传递

int main() {
	thread th;

	{
		float f1 = 12.2f;//这里没有定义P2影响吗
		Para p;
		p.name = "test para class";
		// 传值方式（会触发拷贝)
		th = thread(ThreadMain, 101, f1, "test string", p);
		//thread 是 类型（来自 <thread>），表示这是一个线程对象
		//th 是 变量名
		//ThreadMain 是一个 函数指针，指向你定义的线程函数

	}
	th.join();

	// 第五步：测试指针传递join和detach
	{
		Para p;
		p.name = "";
		thread th(ThreadMainPtr, &p); // 创建一个线程对象th，并将 &p（p 的地址）传给线程函数 ThreadMainPtr
		th.join(); //干嘛的，每个结构体后面都有吗？应该放哪里？
	}

	{
		Para p;
		p.name = "";
		thread th(ThreadMainPtr, &p);//这句语法到底是什么
		th.detach();
	
	}

	getchar(); // 确保主线程不提前退出,为啥？怎么做到的，这句啥意思？
	//阻塞函数，从标准输入读取一个字符。通常我们用它来“按任意键继续”，也就 等你按下一个键，才继续执行
	//在多线程程序中，我们有时会用它来 “卡住主线程”防止提前退出。

	//第七步：测试引用传递（ref）
	{
		Para p;
		p.name = "";

		thread th(ThreadMainRef, ref(p));//线程函数是模板函数，会默认做“值传递”普通函数这里直接p就行，但是线程的就必须用 std::ref 包裹
		th.join();
	}

	// 第八步（可选）：复现危险情况：detach + 指针
	{
		Para p;
		p.name = "dangerous pointer case";

		thread th(ThreadMainPtr, &p);
		th.detach(); // 线程有可能访问被销毁的 p
	}

	this_thread::sleep_for(chrono::milliseconds(500)); // 加延迟模拟冲突? 为什么要加延迟？


	return 0;
}