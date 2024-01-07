#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>
#include<WinSock2.h>
#include<ws2tcpip.h>
#include<Windows.h>//注意顺序，先包含WinSock2，否则会和windows.h中的一些旧的版本api冲突
//或者只包含其中一个

//如果不想管顺序，那么可以使用,仅保留windows.h中必要的头文件
#define WIN32_LEAN_AND_MEAN
#pragma comment(lib,"Ws2_32.lib")
using namespace std;

DWORD WINAPI WorkThread(LPVOID obj)
{
	SOCKET sock = (SOCKET)obj;
	while (true)
	{
		char szBuff[MAXBYTE] = {};
		int nRet = recv(sock, szBuff, sizeof(szBuff), 0);
		if (nRet == SOCKET_ERROR || nRet == 0)
		{

		}
		else
		{
			sockaddr_in siClient;
			int nLen = sizeof(siClient);
			getpeername(sock, (sockaddr*)&siClient, &nLen);/*拿对方的ip和端口*/
			//getsockname()/*拿自己的ip和端口
			printf("from port:%d  from addr:%s  recv:%s\r\n", ntohs(siClient.sin_port), inet_ntoa(siClient.sin_addr), szBuff);
		}

		char szBend[] = { "recv ok!" };
		nRet = send(sock, szBend, sizeof(szBend), 0);
		if (nRet == SOCKET_ERROR)
		{
			cout << "send数据失败" << endl;
		}
	}
	return 0;
}

int main()
{
	{
		//按照版本初始化Ws2_32.lib
		WORD wVersionRequested;
		WSADATA wsaData;
		int err;

		/* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
		wVersionRequested = MAKEWORD(2, 2);

		err = WSAStartup(wVersionRequested, &wsaData);
		if (err != 0) {
			/* Tell the user that we could not find a usable */
			/* Winsock DLL.                                  */
			printf("WSAStartup failed with error: %d\n", err);
			return 1;
		}

		/* Confirm that the WinSock DLL supports 2.2.*/
		/* Note that if the DLL supports versions greater    */
		/* than 2.2 in addition to 2.2, it will still return */
		/* 2.2 in wVersion since that is the version we      */
		/* requested.                                        */

		if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
			/* Tell the user that we could not find a usable */
			/* WinSock DLL.                                  */
			printf("Could not find a usable version of Winsock.dll\n");
			WSACleanup();
			return 1;
		}
		else
			printf("The Winsock 2.2 dll was found okay\n");


		/* The Winsock DLL is acceptable. Proceed to use it. */

		/* Add network programming using Winsock here */

		/* then call WSACleanup when done using the Winsock dll */
	}

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

		sockaddr_in siClient;
		int nLen = sizeof(siClient);
		SOCKET sock = accept(sockServer, (sockaddr*)&siClient, &nLen);
		if (sock == INVALID_SOCKET)
		{
			cout << "等待连接错误" << endl;
		}

		while (true)
		{
			int nSize = 0;
			nRet = recv(sock, (char*)&nSize, sizeof(nSize), 0);
			char* szBuff = new char[nSize];
			nRet = recv(sock, szBuff, nSize, 0);
		}

	}
	return 0;
}
