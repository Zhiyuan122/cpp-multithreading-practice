#include <iostream>
#include <thread>
using namespace std;

atomic<bool> is_exit(false);

void ThreadMain() {
	cout << "[子线程] 启动，线程 ID:" << this_thread::get_id() << endl;

	// 等待主线程允许开始执行
	for (int i = 0; i < 10; i++)
	{
		if (!is_exit) { 
			cout << "[子线程] 尚未收到执行信号，提前退出。" << endl;
			break; // 如果主线程还没发信号，就退出
		}

		cout << "[子线程] 正在执行任务 " << i << endl;
		this_thread::sleep_for(chrono::seconds(1));
	}
	cout << "[子线程] 退出，线程 ID: " << this_thread::get_id() << endl;
}

int main() {
	cout <<"[主线程] 启动，线程 ID: " << this_thread::get_id() << endl;

	thread th(ThreadMain); // 创建子线程，但子线程先等待

	// 模拟一些主线程准备时间
	this_thread::sleep_for(chrono::seconds(2));
	cout << "[主线程] 准备工作完成，通知子线程开始执行任务" << endl;

	is_exit = true;// 发信号：可以开始工作了

	th.join();// 等子线程执行完
	cout << "[主线程] 子线程已退出，主线程结束。" << endl;

	/*
	三种子线程管理方式：
	①（标准做法）	join()      等待子线程退出	   th.join();	     主线程阻塞，直到子线程跑完，安全
	②（后台分离）	detach()    分离子线程	      th.detach();	     主线程不等子线程，可能危险
	③（错误演示）	不调用      join/detach	   thread th(...); // 没处理	 非法操作，程序崩溃
	

	thread th(ThreadMain);
	th.join();// 主线程会等子线程跑完

	thread th(ThreadMain);
	th.detach(); //子线程和主线程脱钩（detach）
				 //主线程一结束，子线程可能还没跑完就被强制终止；整个进程的资源就可能被清理，detach 的线程也可能瞬间被杀掉。
				 //常用于日志线程、后台监听线程等“非关键任务”

	thread th(ThreadMain);// th 离开作用域，但没有被 join 或 detach



	*/



	return 0;
}
