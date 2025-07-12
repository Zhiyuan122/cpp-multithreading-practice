/*
Lambda表达式——C++11 引入的“匿名函数”或“临时函数”
核心：不定义函数名，直接写逻辑块。
等价于：临时函数 + 可访问外部变量”的函数体。

		[捕获列表](参数列表) -> 返回值类型 {函数体};
		[]() { cout << "Hello Lambda" << endl; }

	[capture_list]：决定Lambda 函数可以使用哪些外部变量（外部 = 定义 Lambda 的那个作用域里已有的变量）
	(parameter_list)：就像普通函数的参数
	-> return_type： 指定 Lambda 的返回值类型。可以省略，除非返回值类型写不清楚，比如多分支返回不同类型。
	
	很多例子中没有 ->？
		返回值很简单（比如返回 int，或根本没有返回）
		所以编译器可以 自动推导返回类型
		[]() { cout << "Hello Lambda" << endl; }  // 没有 return，所以推导为 void


常见用途：在不定义函数的情况下，传递一段逻辑作为参数，比如用于线程、排序、算法等
		  实现更简洁的线程启动方式，适合短逻辑、临时任务。

重点：
1. 可以传给 std::thread 作为入口函数，代替普通函数指针。
2. 如果你要在 Lambda 中访问类的成员变量，必须加 [this] 捕获当前对象。


作为线程入口（两种方式）
（1）用作普通线程入口函数

（2）作为类成员中的线程函数




*/

#include <iostream>
#include <thread>
#include <string> 
using namespace std;

//写一个类，并在类中使用 lambda 访问成员变量
class TestLambda
{
public:
	TestLambda();
	~TestLambda(); //这两个自动生成的还需要吗

	string name = "test lambda";

	void Start() {
		// 启动线程，并通过 lambda 访问成员变量
		thread th([this]() {
			cout << "name = " << name << endl;
			}
		);
		th.join();
	}
};

/*唯一重点：
[this]：捕获当前类对象的指针，才能访问 name
如果不加 [this]，lambda 里访问 name 会编译错误
*/

//类外的定义（definition），提供了构造函数和析构函数的具体实现。
TestLambda::TestLambda()
{
}

TestLambda::~TestLambda()
{
}


//在 main 函数中调用上面这个类

//写一个使用 lambda 的简单线程

int main() {
	// 第一部分：lambda + 参数
	// 创建线程，并使用 lambda 表达式作为入口函数
	thread th(
		[](int i) {
			cout << "test lamada" << i << endl;
		}, 
		123
	);

	th.join();

	// 第二部分：类中 lambda 捕获成员变量
	TestLambda test;
	test.Start();

	return 0;

}

/*
 为什么是 thread th(...); 而不是 thread th() { ... };？

 thread th(...) 是 构造函数调用，我们在创建一个线程对象，并传入参数
 thread th() { ... }; 是 C++ 的 函数定义语法，不是创建线程对象。

	 // 这是函数定义
	void foo() { ... }
	// 这是类对象构造
	MyClass obj(...);

为什么可以直接用逗号传 123，而不是放到大括号里？

std::thread 的构造函数是模板函数，支持传递任意可调用对象和

参数
thread th(函数对象, 参数1, 参数2, ...);


*/
