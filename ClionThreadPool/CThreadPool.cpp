//
// Created by yang on 2024/2/26.
//

#include "CThreadPool.h"

bool CThreadPool::CreateThreadPool(DWORD dwCntOfThread) {


    //创建信号量
    m_hSemForQue = CreateSemaphore(NULL, 0, MAXLONG, NULL);//初始化为0个可用信号，可以有MAXLONG个
    if (m_hSemForQue == NULL) {
        return false;
    }


    //创建线程池里的线程
    m_bWorking=true;
    for (DWORD i = 0; i < dwCntOfThread; ++i) {
       HANDLE hThread= CreateThread(NULL, 0, WorkThreadFunc, this, 0, NULL);//参数3：线程要执行的函数指针
       m_vctThreadHandles.push_back(hThread);//存储线程句柄

    }
    return true;
}

void CThreadPool::DestroyThreadPool() {


    //线程退出标志设置为false
    m_bWorking = false;


    //丢出2倍的信号，确保有些线程不会卡在WaitForSingleObject那里，进而线程不退出
    ReleaseSemaphore(m_hSemForQue, m_vctThreadHandles.size()*2, NULL);


    //等待所有线程完成并退出
    WaitForMultipleObjects(m_vctThreadHandles.size(), m_vctThreadHandles.data(), true, INFINITE);

    for (auto  i : m_vctThreadHandles)
    {
        CloseHandle(i);//关闭所有的线程句柄
    }

    //退出
    CloseHandle(m_hSemForQue);//关闭信号量
}

void CThreadPool::AddTask(CThreadPool::TASKINIEFACE &task) {
    m_lckForQue.Lock();//加锁
    m_queTasks.push(task);//加入任务队列
    ReleaseSemaphore(m_hSemForQue, 1, NULL);//增加信号
    m_lckForQue.Unlock();//解锁
}

DWORD CALLBACK CThreadPool::WorkThreadFunc(LPVOID IpParam) {

    CThreadPool* pThis = (CThreadPool*)IpParam;

    while (pThis->m_bWorking)
    {
        //等待任务
        WaitForSingleObject(pThis->m_hSemForQue, INFINITE);

        if (!pThis->m_bWorking)//有些在WaitForSingleObject的时候我们已经要求销毁线程池了，所以这里要再判断一下
        {
            return 0;
        }

        //有任务，从队列中取出任务，处理任务
        pThis->m_lckForQue.Lock();//加锁
        auto task = pThis->m_queTasks.front();//拿出头部任务
        pThis->m_queTasks.pop();//删除头部任务
        pThis->m_lckForQue.Unlock();//解锁


        //处理任务(执行函数)
        task();
    }
    return 0;
}
