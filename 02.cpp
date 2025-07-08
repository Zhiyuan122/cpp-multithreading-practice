#include <iostream>
#include <thread>
using namespace std;

atomic<bool> is_exit(false);

void ThreadMain() {
	cout << "[���߳�] �������߳� ID:" << this_thread::get_id() << endl;

	// �ȴ����߳�����ʼִ��
	for (int i = 0; i < 10; i++)
	{
		if (!is_exit) { 
			cout << "[���߳�] ��δ�յ�ִ���źţ���ǰ�˳���" << endl;
			break; // ������̻߳�û���źţ����˳�
		}

		cout << "[���߳�] ����ִ������ " << i << endl;
		this_thread::sleep_for(chrono::seconds(1));
	}
	cout << "[���߳�] �˳����߳� ID: " << this_thread::get_id() << endl;
}

int main() {
	cout <<"[���߳�] �������߳� ID: " << this_thread::get_id() << endl;

	thread th(ThreadMain); // �������̣߳������߳��ȵȴ�

	// ģ��һЩ���߳�׼��ʱ��
	this_thread::sleep_for(chrono::seconds(2));
	cout << "[���߳�] ׼��������ɣ�֪ͨ���߳̿�ʼִ������" << endl;

	is_exit = true;// ���źţ����Կ�ʼ������

	th.join();// �����߳�ִ����
	cout << "[���߳�] ���߳����˳������߳̽�����" << endl;

	/*
	�������̹߳���ʽ��
	�٣���׼������	join()      �ȴ����߳��˳�	   th.join();	     ���߳�������ֱ�����߳����꣬��ȫ
	�ڣ���̨���룩	detach()    �������߳�	      th.detach();	     ���̲߳������̣߳�����Σ��
	�ۣ�������ʾ��	������      join/detach	   thread th(...); // û����	 �Ƿ��������������
	

	thread th(ThreadMain);
	th.join();// ���̻߳�����߳�����

	thread th(ThreadMain);
	th.detach(); //���̺߳����߳��ѹ���detach��
				 //���߳�һ���������߳̿��ܻ�û����ͱ�ǿ����ֹ���������̵���Դ�Ϳ��ܱ�����detach ���߳�Ҳ����˲�䱻ɱ����
				 //��������־�̡߳���̨�����̵߳ȡ��ǹؼ�����

	thread th(ThreadMain);// th �뿪�����򣬵�û�б� join �� detach



	*/



	return 0;
}
