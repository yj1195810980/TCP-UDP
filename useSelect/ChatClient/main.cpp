#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include"../common/CSockinit.h"
#include<iostream>
#include<WinSock2.h>
#include<ws2tcpip.h>
#include<Windows.h>//ע��˳���Ȱ���WinSock2��������windows.h�е�һЩ�ɵİ汾api��ͻ
//����ֻ��������һ��

//��������˳����ô����ʹ��,������windows.h�б�Ҫ��ͷ�ļ�
#define WIN32_LEAN_AND_MEAN
#pragma comment(lib,"Ws2_32.lib")
using namespace std;
int main()
{


	/*�����׽���*/
	SOCKET ClientSocket = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ClientSocket == INVALID_SOCKET)
	{
		cout << "�����׽���ʧ��";
		return 0;
	}
	char szSend[MAXBYTE] = {};

	in_addr in = { 0 };
	if (inet_pton(AF_INET, "127.0.0.1", &in) == 1)//��127.0.0.1ת��ΪС��
	{
		sockaddr_in si = {};
		si.sin_family = AF_INET;
		si.sin_port = si.sin_port = htons(0x7788);//�����ߵĶ˿ں�
		si.sin_addr.S_un.S_addr = in.S_un.S_addr;//�����ߵ�ip��ַ
		/*���ӷ�����*/
		int nRet = connect(ClientSocket, (sockaddr*)&si, sizeof(si));
		if (nRet == SOCKET_ERROR)
		{
			cout << "connect���ӷ�����ʧ��" << endl;
			return 0;
		}

		//closesocket(ClientSocket);


		//int nSize = 0x1000;//4096�ֽ�
		//char* szSend = new char[nSize];
		//memset(szSend, 0x66, nSize);
		//nRet = send(ClientSocket, (char*)&nSize, sizeof(nSize), 0);

		while (true)
		{
			int nSize = 0x1000;//4096�ֽ�
			char* szSend = new char[nSize];

			char data[100] = {0};
			cin >> data;
			nRet = send(ClientSocket, data, sizeof(data), 0);

		}


	

		cout << "oksend" << endl;
	}
}