//thread 的参数传递机制.这里传入的是栈中的空间？
/*
将不同类型的参数（包括自定义对象）传入线程函数，并理解拷贝与生命周期机制

1. thread 是如何传参的？
底层机制： 模板函数（template function） 实现，所以支持任意参数（基本类型、对象、指针）
传参方式： 值传递（by value）。每个参数都会 复制一份 传给线程函数
注意：如果你传的是指针或引用，那就要注意线程安全，因为你传过去的是同一个地址。



2. 为什么会有 1 次 Create + 3 次 Drop，1 次 Copy？
		创建 p → Create
		将 p 传入线程 → 复制一次 → Copy
		后续生命周期中涉及三次析构：
		原始 p 的析构
		拷贝到线程时生成副本 → 线程结束后析构
		std::thread 内部的额外结构可能会再创建一次副本用于调度 → 析构
传对象 → 至少两次拷贝，一次在线程外，一次在线程内。
退出作用域 → 有几个对象，就会析构几次。

3. 想进一步减少拷贝次数怎么办？
	std::ref(p)：传引用，减少拷贝（但要小心生命周期）
	shared_ptr<Para>：安全传对象指针，控制共享资源
	move(p)：如果你只想转移对象所有权（但对象不能再用）

*/

#include <iostream>
#include <thread>
#include <string>
using namespace std;

class Para//用构造函数 / 拷贝构造函数 / 析构函数 打印信息，便于追踪对象生命周期。
{
public:
	string name;

	Para(){
		cout << "creat para" << endl;
	}

	Para(const Para& p) {
		cout << "copy phar" << endl;
	}

	~Para() {
		cout << "Drop  para" << endl;
	}

};

void ThreadMain(int p1, float p2, string str, Para p4) {
	// ?? 执行前 p4 又被拷贝了一次 → 打印 copy para
	this_thread::sleep_for(chrono::milliseconds(100));  
	cout << "ThreadMain: " << p1 << " " << p2 << " " << str << " " << p4.name << endl;
	// ?? ThreadMain 结束 → p4 被析构 → 打印 Drop para
}

int main() {
	// ?? 步骤 1：声明一个 thread 类型的变量 th（但还没启动线程）
	thread th;// 提前定义线程变量
	//创建一个局部作用域（block scope）
	//创建一些变量，但不想让它们影响后面的代码时很有用
	{
		float f1 = 12.1f;
		Para p; // ?? 步骤 2：调用默认构造函数 → 打印 "creat para"
		p.name = "test para class";

		//?? 步骤 3：创建线程，传入函数和参数
		th = thread(ThreadMain, 101, f1, "test string para", p);// 创建线程并传入参数
		//      ↑ 创建子线程时会复制一份 Para（打印 copy para）
		//      ↑ 在子线程开始执行 ThreadMain 的时候会再复制一次 Para（再次打印 copy para）
	
	}
	// ?? 步骤 4：出作用域，p 被析构 → 打印 Drop para


	th.join();// ?? 步骤 5：主线程等子线程结束
	return 0;

}
