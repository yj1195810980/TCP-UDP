#include <iostream>
#include "CThreadPool.h"

int main() {

	CThreadPool tp;
	tp.CreateThreadPool(6);//创建6个线程

	for (int i = 0; i <= 10000; ++i)
	{
		if (i % 2 == 0)
		{
			CThreadPool::TASKINIEFACE task = [i]() {printf("偶数%08X ,pid%d\r\n", i, GetCurrentThreadId()); };
			tp.AddTask(task);
		}
		else
		{
			CThreadPool::TASKINIEFACE task = [i]() {printf("奇数%08X ,pid%d\r\n", i, GetCurrentThreadId()); };
			tp.AddTask(task);
		}
	}


	printf("结束1\r\n");
	system("pause");//暂停主线程
	tp.DestroyThreadPool();//销毁线程池
	printf("结束2\r\n");
	system("pause");
	printf("结束3\r\n");
	return 0;
}
