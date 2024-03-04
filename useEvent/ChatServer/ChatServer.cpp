#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include"../common/CSockinit.h"
#include<iostream>
#include<vector>
#include<map>
#include<WinSock2.h>
#include<ws2tcpip.h>
#include<Windows.h>//注意顺序，先包含WinSock2，否则会和windows.h中的一些旧的版本api冲突
//或者只包含其中一个

//如果不想管顺序，那么可以使用,仅保留windows.h中必要的头文件
#define WIN32_LEAN_AND_MEAN
#pragma comment(lib,"Ws2_32.lib")
using namespace std;


vector<SOCKET>g_vctSocks;

void HandleData(SOCKET sock)
{
	char szBuff[MAXBYTE] = {};
	auto nRet = recv(sock, szBuff, sizeof(szBuff), 0);
	if (nRet == 0 || nRet == SOCKET_ERROR)
	{
		cout << "recv 失败" << endl;
	}
	else
	{
		sockaddr_in siClient;
		int nLen = sizeof(siClient);
		getpeername(sock, (sockaddr*)&siClient, &nLen);
		printf("from %s %d recv:%s \r\n", inet_ntoa(siClient.sin_addr), ntohs(siClient.sin_port), szBuff);
	}
	char szSend[] = { "recv ok!" };
	nRet = send(sock, szSend, sizeof(szSend), 0);
	if (nRet == SOCKET_ERROR)
	{
		cout << "send 失败" << endl;
	}
}

DWORD WINAPI WorkThreadFunc(LPVOID IpParam)
{
	SOCKET sock = (SOCKET)IpParam;
	while (true)
	{
		fd_set fsRead;//fd_set只能放64个socket，也就是说，如果多于46个，就再开一个线程
		FD_ZERO(&fsRead);//初始化一下
		for (auto sock : g_vctSocks)
		{
			FD_SET(sock, &fsRead);//FD_SET宏提供了写入fd_Set的功能，并且检查
		}

		timeval tv = { 0,0 };
		auto nRet = select(0, &fsRead, NULL, NULL, &tv);
		/*
		参数1 省略
		参数2 可以读的时候的结构体
		参数3 可以写的时候的结构体 在这里不关心可以写
		参数4 等待时间
		*/
		if (nRet == SOCKET_ERROR || nRet == 0)
		{
			//错误或超时
			//没有数据的时候，可以在这里做其他事情


			continue;
		}


		//根据输出参数，遍历输出，拿到已经准备好读数据的socket
		for (auto sock : g_vctSocks)
		{
			if (FD_ISSET(sock, &fsRead))
			{
				HandleData(sock);
			}
		}


#if 0
		for (size_t i = 0; i < fsRead.fd_count; i++)
		{
			/*auto sock = fsRead.fd_array[i];
			HandleData(sock);*/
		}
#endif // 0


	}
	return 0;
}


int main()
{
	/*创建套接字*/
	SOCKET sockServer = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockServer == INVALID_SOCKET)
	{
		cout << "创建套接字失败";
		return 0;
	}

	sockaddr_in si;
	si.sin_family = AF_INET;
	si.sin_port = htons(0x7788);//大尾转小尾，本机字节序转网络字节序
	in_addr in = { 0 };
	if (inet_pton(AF_INET, "127.0.0.1", &in) == 1)//将127.0.0.1转换为小端
	{
		si.sin_addr.S_un.S_addr = in.S_un.S_addr;//小尾地址使用inet_pton将其转换为字符串转换为数值，然后再转换为小尾
		int nRet = ::bind(sockServer, (sockaddr*)&si, sizeof(si));
		if (nRet == SOCKET_ERROR)
		{
			cout << "bind失败";
			return 0;
		}

		/*监听*/
		nRet = listen(sockServer, SOMAXCONN);
		if (nRet == SOCKET_ERROR)
		{
			cout << "监听错误" << endl;
			return 0;
		}

		/*HANDLE hThread = CreateThread(
			NULL,
			0,
			WorkThreadFunc,
			(LPVOID)NULL,
			0,
			NULL
		);
		CloseHandle(hThread);*/

		WSAEVENT hEvent = WSACreateEvent();//创建事件模型
		WSAEventSelect(sockServer, hEvent, FD_ACCEPT);//此socket只用来建立连接


		map<WSAEVENT, SOCKET>mpES;
		mpES[hEvent] = sockServer;

		while (true)
		{
			//检测socket
			WSAEVENT aryEvents[WSA_MAXIMUM_WAIT_EVENTS] = {};
			INT nCnt = 0;
			for (auto i : mpES)
			{
				aryEvents[nCnt++] = i.first;
			}

			DWORD dwRet = WSAWaitForMultipleEvents(nCnt, aryEvents, false, INFINITE, false);
			//数组大小，数组地址，是否等待所有，返回触发触发事件的事件数组索引
			//该函数会等待数组中的任何一个socket上发生的事情


			WSAEVENT hEvent = aryEvents[dwRet];
			SOCKET sockWait = mpES[hEvent];
			WSANETWORKEVENTS event;
			WSAEnumNetworkEvents(sockWait, hEvent, &event);
			//获取指定socket上发生的事件

			if (event.lNetworkEvents & FD_ACCEPT)
			{
				//检测到socket有新的连接
				//建立连接
				sockaddr_in siClient;
				int nLen = sizeof(siClient);
				SOCKET sockClient = accept(sockServer, (sockaddr*)&siClient, &nLen);
				if (sockClient == INVALID_SOCKET)
				{
					cout << "等待连接错误" << endl;
				}

				//保存
				WSAEVENT hEvent = WSACreateEvent();//创建事件模型
				WSAEventSelect(sockClient, hEvent, FD_READ|FD_CLOSE);//此socket是连接成功后的socket，在这里用来监测可读和关闭
				mpES[hEvent] = sockClient;//存储添加了事件的sock
			}
			else if (event.lNetworkEvents & FD_READ) {
				HandleData(sockWait);
			}
			else if (event.lNetworkEvents & FD_CLOSE)
			{
				
				cout << "Client close" <<"\r\n";
			}
		}

	}
	return 0;
}
