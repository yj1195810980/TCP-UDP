//
// Created by yang on 2024/2/26.
//

#ifndef CLIONTHREADPOOL_CTHREADPOOL_H
#define CLIONTHREADPOOL_CTHREADPOOL_H

//排除多余头文件
#define WIN32_LEAN_AND_MEAN
#define NOCOMM


#include <Windows.h>
#include <queue>
#include <functional>
#include <vector>
#include "CLock.h"


using namespace std;


class CThreadPool {


public:
    using TASKINIEFACE = function<void(void)>;

public:
    /***
     * 创建线程池
     * @param dwCntOfThread 线程数量
     * @return
     */
    bool CreateThreadPool(DWORD dwCntOfThread);

    /***
     * 销毁退出线程池
     */
    void DestroyThreadPool();


    void AddTask(TASKINIEFACE &task);

private:
    /*
    关键的是这个函数，所有的线程都在执行该函数，该函数的任务是在任务队列里去拿任务，然后拿到了就给执行这个函数的
    线程执行，并且里面有死循环，这会让线程池里的函数永远不会退出
    */
    static DWORD CALLBACK WorkThreadFunc(LPVOID IpParam);

private:

    queue<TASKINIEFACE> m_queTasks;//任务队列
    CLock m_lckForQue;//同步队列


    HANDLE  m_hSemForQue;//信号量，指示队列中有多少个任务
    bool m_bWorking;
    vector<HANDLE>m_vctThreadHandles;//保存创建的线程句柄
  

};


#endif //CLIONTHREADPOOL_CTHREADPOOL_H
