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
	SOCKET ClientSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ClientSocket == INVALID_SOCKET)
	{
		cout << "创建套接字失败";
		return 0;
	}
	char szSend[MAXBYTE] = {};

	in_addr in = { 0 };
	if (inet_pton(AF_INET, "127.0.0.1", &in) == 1)//将127.0.0.1转换为小端
	{
		sockaddr_in si = {};
		si.sin_family = AF_INET;
		si.sin_port = si.sin_port = htons(7788);//接收者的端口号
		si.sin_addr.S_un.S_addr = in.S_un.S_addr;//接收者的ip地址
		/*连接服务器*/
		int nRet = connect(ClientSocket, (sockaddr*)&si, sizeof(si));
		if (nRet == SOCKET_ERROR)
		{
			cout << "connect连接服务器失败" << endl;
			return 0;
		}

		closesocket(ClientSocket);


		int nSize = 0x1000;//4096字节
		char* szSend = new char[nSize];
		memset(szSend, 0x66, nSize);
		nRet = send(ClientSocket, (char*)&nSize, sizeof(nSize), 0);
		//套接字，发送的内容(int 强转为char)，发送的大小
		//这样可以把下一个包的大小告诉服务器

		nRet = send(ClientSocket, szSend, nSize, 0);
		//套接字，发送的内容，发送的大小，这里发送的大小是上一个包的内容决定的


		nSize = 100;
		szSend = new char[nSize];
		memset(szSend, 0x55, nSize);
		nRet = send(ClientSocket, (char*)&nSize, sizeof(nSize), 0);
		nRet = send(ClientSocket, szSend, nSize, 0);

		cout << "oksend" << endl;
	}
}