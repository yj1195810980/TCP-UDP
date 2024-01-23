#pragma once
#include<WinSock2.h>
#include<Windows.h>
#include<map>
#include<iostream>
#include"../common/CByteStreamBuff.h"
#include"../common/CCrc.h"
#include"../common/CLock.h"
#include"../common/CSockinit.h"
#include"../common/CLock.h"
using namespace std;


#pragma comment(lib,"ws2_32.lib")

class CUMT
{
public:
	/// <summary>
	/// �ȴ��ͻ�������
	/// </summary>
	/// <param name="szIp"></param>
	/// <param name="nPort"></param>
	/// <returns></returns>
	BOOL Accept(LPTSTR szIp, USHORT nPort);

	/// <summary>
	/// �ͻ������ӷ�����
	/// </summary>
	BOOL Connect(LPTSTR szIp, USHORT nPort);

	/// <summary>
	/// ��������
	/// </summary>
	/// <param name="pBuff"></param>
	/// <param name="dwBufLne"></param>
	/// <returns></returns>
	DWORD Send(LPBYTE pBuff, DWORD dwBufLne);


	/// <summary>
	/// ��������
	/// </summary>
	/// <param name="pBuff"></param>
	/// <param name="dwBufLne"></param>
	/// <returns></returns>
	DWORD Recv(LPBYTE pBuff, DWORD dwBufLne);

	/// <summary>
	/// �ر�
	/// </summary>
	/// <returns></returns>
	VOID Close();

#pragma region �ṹ�嶨��

private:
#define DATALEN 1460
	//������
	enum PackageType
	{
		PT_SYN = 4,
		PT_ACK = 2,
		PT_DATA = 1,
		PT_FIN
	};

	/*��*/
#pragma pack(push)
#pragma pack(1)
	struct CPackage {
		CPackage() {}
		CPackage(WORD pt, DWORD dwSeq, LPVOID pData = NULL, WORD nDataLen = 0)
		{
			m_nPt = pt;
			m_nSeq = dwSeq;
			m_nLen = 0;
			m_nCheck = 0;
			if (pData != NULL)
			{
				memcpy(m_aryData, pData, nDataLen);
				m_nLen = nDataLen;
				Crc crc;
				m_nCheck= crc.crc32(pData, nDataLen);
			}

		}

		WORD m_nPt; //������
		WORD m_nLen; //���ݳ���
		DWORD m_nSeq;// a�����
		DWORD m_nCheck;// У��ֵ
		BYTE m_aryData[DATALEN];//��������
	};
#pragma pack(pop)
	const ULONGLONG m_tmElapse = 100;//��ʱ��ʱ��
	struct CPackageInfo
	{
		CPackageInfo() {};
		CPackageInfo(ULONGLONG tm, CPackage pkg) :m_tmLastTime(tm), m_pkg(pkg)
		{

		}
		ULONGLONG m_tmLastTime;//ʱ��
		CPackage m_pkg;
	};
#pragma endregion 
private:
	VOID Clear();
	BOOL Init();
	VOID Log(const char* szFmt, ...);
private:
	SOCKET m_sock;

	sockaddr_in m_siDst = {};//�Է��ĵ�ַ
	sockaddr_in m_siSrc = {};//�Լ��ĵ�ַ
	
private:
	DWORD m_nNextSendSeq = 0;//��һ�β���Ŀ�ʼ���
	DWORD m_nNextRecvSeq = 0;//��һ�δ��뻺�����İ������

private:
	/// <summary>
	/// �洢���Ͱ�����
	/// </summary>
	map<DWORD, CPackageInfo>m_mpSend;
	CLock m_lckForSendMp;//ͬ������m_mpSend�Ķ��߳�ͬ��

	/// <summary>
	/// �洢�յ��İ�
	/// </summary>
	map<DWORD, CPackage>m_mpRecv;
	CLock m_lckForRecvMp;//ͬ������m_mpRecv�Ķ��߳�ͬ��

	/// <summary>
	/// �������ݵĻ�����
	/// </summary>
	CByteStreamBuff m_bufRecv;
	CLock m_lckForbufRecv;//ͬ������m_bufRecv�Ķ��߳�ͬ��
private:
	HANDLE m_hSnedThread = NULL;
	HANDLE m_RecvThread = NULL;
	HANDLE m_HandleRecvPkgThread = NULL;
	BOOL m_bWorking = FALSE;
	/// <summary>
	/// �����߳�
	/// </summary>
	/// <param name="IpParam"></param>
	/// <returns></returns>
	static DWORD CALLBACK SnedThread(LPVOID IpParam);//�����߳�

	/// <summary>
	/// �հ��߳�
	/// </summary>
	/// <param name="IpParam"></param>
	/// <returns></returns>
	static DWORD CALLBACK RecvThread(LPVOID IpParam);
	/// <summary>
	/// �������հ��������뻺�������߳�
	/// </summary>
	/// <param name="IpParam"></param>
	/// <returns></returns>
	static DWORD CALLBACK HandleRecvPkgThread(LPVOID IpParam);
};