//thread �Ĳ������ݻ���.���ﴫ�����ջ�еĿռ䣿
/*
����ͬ���͵Ĳ����������Զ�����󣩴����̺߳���������⿽�����������ڻ���

1. thread ����δ��εģ�
�ײ���ƣ� ģ�庯����template function�� ʵ�֣�����֧������������������͡�����ָ�룩
���η�ʽ�� ֵ���ݣ�by value����ÿ���������� ����һ�� �����̺߳���
ע�⣺����㴫����ָ������ã��Ǿ�Ҫע���̰߳�ȫ����Ϊ�㴫��ȥ����ͬһ����ַ��



2. Ϊʲô���� 1 �� Create + 3 �� Drop��1 �� Copy��
		���� p �� Create
		�� p �����߳� �� ����һ�� �� Copy
		���������������漰����������
		ԭʼ p ������
		�������߳�ʱ���ɸ��� �� �߳̽���������
		std::thread �ڲ��Ķ���ṹ���ܻ��ٴ���һ�θ������ڵ��� �� ����
������ �� �������ο�����һ�����߳��⣬һ�����߳��ڡ�
�˳������� �� �м������󣬾ͻ��������Ρ�

3. ���һ�����ٿ���������ô�죿
	std::ref(p)�������ã����ٿ�������ҪС���������ڣ�
	shared_ptr<Para>����ȫ������ָ�룬���ƹ�����Դ
	move(p)�������ֻ��ת�ƶ�������Ȩ�������������ã�

*/

#include <iostream>
#include <thread>
#include <string>
using namespace std;

class Para//�ù��캯�� / �������캯�� / �������� ��ӡ��Ϣ������׷�ٶ����������ڡ�
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
	// ?? ִ��ǰ p4 �ֱ�������һ�� �� ��ӡ copy para
	this_thread::sleep_for(chrono::milliseconds(100));  
	cout << "ThreadMain: " << p1 << " " << p2 << " " << str << " " << p4.name << endl;
	// ?? ThreadMain ���� �� p4 ������ �� ��ӡ Drop para
}

int main() {
	// ?? ���� 1������һ�� thread ���͵ı��� th������û�����̣߳�
	thread th;// ��ǰ�����̱߳���
	//����һ���ֲ�������block scope��
	//����һЩ������������������Ӱ�����Ĵ���ʱ������
	{
		float f1 = 12.1f;
		Para p; // ?? ���� 2������Ĭ�Ϲ��캯�� �� ��ӡ "creat para"
		p.name = "test para class";

		//?? ���� 3�������̣߳����뺯���Ͳ���
		th = thread(ThreadMain, 101, f1, "test string para", p);// �����̲߳��������
		//      �� �������߳�ʱ�Ḵ��һ�� Para����ӡ copy para��
		//      �� �����߳̿�ʼִ�� ThreadMain ��ʱ����ٸ���һ�� Para���ٴδ�ӡ copy para��
	
	}
	// ?? ���� 4����������p ������ �� ��ӡ Drop para


	th.join();// ?? ���� 5�����̵߳����߳̽���
	return 0;

}
