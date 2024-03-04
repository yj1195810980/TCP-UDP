#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include"../common/CSockinit.h"
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
		si.sin_port = si.sin_port = htons(0x7788);//接收者的端口号
		si.sin_addr.S_un.S_addr = in.S_un.S_addr;//接收者的ip地址
		/*连接服务器*/
		int nRet = connect(ClientSocket, (sockaddr*)&si, sizeof(si));
		if (nRet == SOCKET_ERROR)
		{
			cout << "connect连接服务器失败" << endl;
			return 0;
		}

		//closesocket(ClientSocket);


		//int nSize = 0x1000;//4096字节
		//char* szSend = new char[nSize];
		//memset(szSend, 0x66, nSize);
		//nRet = send(ClientSocket, (char*)&nSize, sizeof(nSize), 0);

		while (true)
		{
			int nSize = 0x1000;//4096字节
			char* szSend = new char[nSize];

			char data[100] = {0};
			cin >> data;
			nRet = send(ClientSocket, data, sizeof(data), 0);

		}


	

		cout << "oksend" << endl;
	}
}