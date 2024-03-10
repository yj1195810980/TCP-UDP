#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include"../common/CSockinit.h"
#include <iostream>
#include <vector>
#include <map>

#include <mswsock.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>//注意顺序，先包含WinSock2，否则会和windows.h中的一些旧的版本api冲突

//或者只包含其中一个

//如果不想管顺序，那么可以使用,仅保留windows.h中必要的头文件

#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Mswsock.lib")//iocp需要该lib
using namespace std;

enum Overlapped_Type
{
	OT_ACCEPT,
	OT_RECV,
	OT_SEND
};

struct MyOverlapped :public OVERLAPPED
{
public:
	MyOverlapped()
	{
		//初始化一下
		memset(this, 0, sizeof(*this));
		m_bufRecv.len = sizeof(m_aBufRecv);
		m_bufRecv.buf = m_aBufRecv;

		m_bufSend.len = sizeof(m_aBufSend);
		m_bufSend.buf = m_aBufSend;
	}
	SOCKET m_sock;
	Overlapped_Type m_ot;

	BYTE m_AccepBuff[sizeof(sockaddr_in) * 2] = {};//一个是自己的ip地址和端口号，一个是对方的ip地址和端口号
	DWORD dwAccepRecv = 0;
	CHAR m_aBufRecv[MAXBYTE] = {};
	WSABUF m_bufRecv;

	CHAR m_aBufSend[MAXBYTE] = {};
	WSABUF m_bufSend;
};


//投递请求
bool PostAcceptEx(HANDLE hIocp, SOCKET socketlists)
{
	//向iocp投递接受连接的请求
	SOCKET sockClient = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//用于给acceptEX绑定的socket


	//关联客户端的socket到iocp
	HANDLE hRet = CreateIoCompletionPort((HANDLE)sockClient, hIocp, NULL, 0);
	if (hIocp == NULL)
	{
		cout << "bind iocp form socket";
		return 0;
	}

	/*
	为什么要申请内存？和继承OVERLAPPED？
	1：申请内存
		因为AccepEx在函数内，函数内创建的是局部的变量会释放，这导致参数3，7，8在使用GetQueuedCompletionStatus
		拿数据的时候，会导致空指针问题
	2：为什么要继承OVERLAPPED 添加自己的成员变量？
		因为继承一下，代表这个数据成为了一个整体，这样方便管理和GetQueuedCompletionStatus拿数据的时候区分
	*/
	MyOverlapped* pOV = new MyOverlapped;
	pOV->m_sock = sockClient;//将客户端的socket存储到结构中
	pOV->m_ot = Overlapped_Type::OT_ACCEPT;//标记着是一个接受连接的
	bool bRet = AcceptEx(
		socketlists,
		sockClient,
		pOV->m_AccepBuff,
		0,
		sizeof(sockaddr_in) + 16,
		sizeof(sockaddr_in) + 16,
		&pOV->dwAccepRecv,
		pOV);
	if (!bRet)
	{
		int nRet = WSAGetLastError();

		if (nRet != ERROR_IO_PENDING)//如果等于ERROR_IO_PENDING代表成功了
		{
			cout << "AcceptEx err" << endl;
			return false;
		}
	}
	return true;
}


//接收数据
void PostRecv(SOCKET sock)
{
	MyOverlapped* pOV = new MyOverlapped;
	pOV->m_ot = Overlapped_Type::OT_RECV;
	pOV->m_sock = sock;
	DWORD dwFlag = 0;//标志要给个地址，可以不要
	int nRet = WSARecv(sock, &pOV->m_bufRecv, 1, NULL, &dwFlag, pOV, NULL);
	if (nRet == SOCKET_ERROR && GetLastError() == ERROR_IO_PENDING)
	{
		cout << "投递接收数据的请求 succes" << endl;
	}
	else
	{
		cout << "投递接收数据的请求 failed" << endl;
	}

}
void PostSend(SOCKET sock)
{
	MyOverlapped* pOV = new MyOverlapped;
	pOV->m_ot = Overlapped_Type::OT_SEND;
	pOV->m_sock = sock;
	strcpy(pOV->m_aBufSend, "recv");

	int nRet = WSASend(sock, &pOV->m_bufSend, 1, NULL, 0, pOV, NULL);
	if ((nRet == SOCKET_ERROR && GetLastError() == ERROR_IO_PENDING) || nRet == 0)
	{
		cout << "投递发送数据的请求 succes" << endl;
	}
	else
	{
		cout << "投递发送数据的请求 failed" << endl;
	}


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

		//创建iocp对象
		HANDLE hIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
		if (hIocp == NULL)
		{
			cout << "CreateIocp obj err";
			return 0;
		}

		//关联socket和iocp
		HANDLE hRet = CreateIoCompletionPort((HANDLE)sockServer, hIocp, NULL, 0);

		if (hIocp == NULL)
		{
			cout << "bind iocp form socket";
			return 0;
		}

		////投递一个接收连接的请求
		bool ppresut = PostAcceptEx(hIocp, sockServer);
		if (ppresut == false)
		{
			cout << "PostAcceptEx err";
		}


		while (true)
		{
			//等待队列中有完成的请求
			DWORD dwBytes = 0;
			ULONG_PTR dwKey = 0;
			MyOverlapped* pOv = NULL;//传出的数据存储在这
			GetQueuedCompletionStatus(hIocp, &dwBytes, &dwKey, (LPOVERLAPPED*)&pOv, INFINITE);//我们干的事就是处理队列

			switch (pOv->m_ot)
			{
			case OT_ACCEPT:
			{
				PostAcceptEx(hIocp, sockServer);//再次投递
				PostRecv(pOv->m_sock);//连接上的客户端socket会通过pOv传出
				break;
			}
			case OT_RECV:
			{
				sockaddr_in siClient;
				int nLen = sizeof(siClient);
				getpeername(pOv->m_sock, (sockaddr*)&siClient, &nLen);
				printf("from %s %d recv%s\r\n",
					inet_ntoa(siClient.sin_addr),
					ntohs(siClient.sin_port),
					pOv->m_aBufRecv);

				PostSend(pOv->m_sock);//服务端发送个数据回复
				break;
			}
			case OT_SEND:
			{
				PostRecv(pOv->m_sock);//连接上的客户端socket会通过pOv传出
				break;
			}
			default:
				break;
			}
		}
	}
	return 0;
}
