/*
 �̺߳����������ݵ����ַ�ʽ
 | ��ʽ                 | ʾ��                     | �Ƿ񿽱�    | ���յ�                             |
| ֵ���ݣ�By Value��      | `thread(func, p)`      | �Զ����� | ��ȫ���Ḵ��һ�ݣ�                       |
| ָ�봫�ݣ�By Pointer��   | `thread(func, &p)`     |  ������   | ��ԭ�������ͷţ����̷߳��ʽ�����               |
| ���ô��ݣ�By Reference�� | `thread(func, ref(p))` |  ������   | ������`ref()`����������ģ��ƥ�����Ҳ���������ڷ��� |

1. ֵ���ݷ�ʽ��thread(ThreadMain, p)��------���̺߳���Ĭ���ǡ�ֵ���ݡ�
���� Para ���󲢸�ֵ��
���̴߳�ֵ�����̣߳�thread �ڲ�ͨ�� ģ���Ƶ� + �������ơ�
�������ڰ�ȫ�����߳��õ����� p �Ŀ������������̵߳� p ������Ҳ��Ӱ�졣
�ᴥ�� Copy Constructor ��Σ�Create �� Copy �� Drop ��3��

2. ָ�봫�ݷ�ʽ��thread(ThreadMainPtr, &p)��
p �������߳�ջ�ϴ����Ķ��󣬴����ַ��ָ�룩�����̡߳�
���ʹ�� detach()�����߳̿����� p �����ٺ�ŷ����� �� ����ʵ��Ƿ��ڴ档
�ؼ���
���ᴥ�� copy��������ȷ�� p ����ʱ���̲߳��ܷ�������
��Σ�յ��� detach() + ջ������ַ

3. ���ô��ݷ�ʽ��thread(ThreadMainRef, ref(p))��
Ҳ���ᴥ�����������ܸ��á�
����ָ��һ����������߳��Ƚ������ö������̷߳��ʾͻ����
Ҫ�� std::ref() ��ȷ����ģ�庯�����Ҵ��������ã�

�ؼ���
ģ���޷��Զ�ʶ�����ã�Ҫ�� ref()��
�������ڱ���С���߳�ִ��ʱ�䣬�������

summary:
1. ���̺߳���Ĭ���ǡ�ֵ���ݡ����������Զ����Ʋ���
2. ָ������ò��Ḵ��ԭ���ݣ�����Ҫ ȷ��ԭ���ݲ���ǰ����
3. ��������ã������� std::ref() ��������
4. ���̺߳������ʵ������������ڽϳ������Կ����öѷ�������Ա����


*/

#include<iostream>
#include<thread>
#include<string>
using namespace std;

//��һ�������� Para �࣬����׷�ٶ�����������
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


//�ڶ�����д�̺߳��� ThreadMain������ֵ���ݣ�by value��
void ThreadMain(int p1, float p2, string str, Para p4) {
	this_thread::sleep_for(chrono::milliseconds(100));// why seep?
	cout << "ThreadMain:" << p1 << " " << p2 << " " << str << " " << p4.name << endl; //  p4.name    p4 OK?

}

//���Ĳ���д�̺߳��� ThreadMainPtr������ָ�봫��
void ThreadMainPtr(Para* p) {
	this_thread::sleep_for(chrono::milliseconds(100));
	cout << "ThreadMainPtr name = " << p->name << endl;
}

//��������д�̺߳��� ThreadMainRef���������ô���
void ThreadMainRef(Para& p) {
	this_thread::sleep_for(chrono::milliseconds(100));
	cout << "ThreadMainRef name = " << p.name << endl; // �ǲ��Ǻ���ͨ����һ���߼�����������Para* p������� p->name ����������Para& p�����ø�ʽ������������p.name

}





//������������ֵ����

int main() {
	thread th;

	{
		float f1 = 12.2f;//����û�ж���P2Ӱ����
		Para p;
		p.name = "test para class";
		// ��ֵ��ʽ���ᴥ������)
		th = thread(ThreadMain, 101, f1, "test string", p);
		//thread �� ���ͣ����� <thread>������ʾ����һ���̶߳���
		//th �� ������
		//ThreadMain ��һ�� ����ָ�룬ָ���㶨����̺߳���

	}
	th.join();

	// ���岽������ָ�봫��join��detach
	{
		Para p;
		p.name = "";
		thread th(ThreadMainPtr, &p); // ����һ���̶߳���th������ &p��p �ĵ�ַ�������̺߳��� ThreadMainPtr
		th.join(); //����ģ�ÿ���ṹ����涼����Ӧ�÷����
	}

	{
		Para p;
		p.name = "";
		thread th(ThreadMainPtr, &p);//����﷨������ʲô
		th.detach();
	
	}

	getchar(); // ȷ�����̲߳���ǰ�˳�,Ϊɶ����ô�����ģ����ɶ��˼��
	//�����������ӱ�׼�����ȡһ���ַ���ͨ�����������������������������Ҳ�� ���㰴��һ�������ż���ִ��
	//�ڶ��̳߳����У�������ʱ�������� ����ס���̡߳���ֹ��ǰ�˳���

	//���߲����������ô��ݣ�ref��
	{
		Para p;
		p.name = "";

		thread th(ThreadMainRef, ref(p));//�̺߳�����ģ�庯������Ĭ������ֵ���ݡ���ͨ��������ֱ��p���У������̵߳ľͱ����� std::ref ����
		th.join();
	}

	// �ڰ˲�����ѡ��������Σ�������detach + ָ��
	{
		Para p;
		p.name = "dangerous pointer case";

		thread th(ThreadMainPtr, &p);
		th.detach(); // �߳��п��ܷ��ʱ����ٵ� p
	}

	this_thread::sleep_for(chrono::milliseconds(500)); // ���ӳ�ģ���ͻ? ΪʲôҪ���ӳ٣�


	return 0;
}