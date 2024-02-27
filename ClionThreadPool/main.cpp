#include <iostream>
#include "CThreadPool.h"

int main() {

	CThreadPool tp;
	tp.CreateThreadPool(6);//����6���߳�

	for (int i = 0; i <= 10000; ++i)
	{
		if (i % 2 == 0)
		{
			CThreadPool::TASKINIEFACE task = [i]() {printf("ż��%08X ,pid%d\r\n", i, GetCurrentThreadId()); };
			tp.AddTask(task);
		}
		else
		{
			CThreadPool::TASKINIEFACE task = [i]() {printf("����%08X ,pid%d\r\n", i, GetCurrentThreadId()); };
			tp.AddTask(task);
		}
	}


	printf("����1\r\n");
	system("pause");//��ͣ���߳�
	tp.DestroyThreadPool();//�����̳߳�
	printf("����2\r\n");
	system("pause");
	printf("����3\r\n");
	return 0;
}
