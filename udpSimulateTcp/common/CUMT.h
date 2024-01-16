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
	/// 等待客户端连接
	/// </summary>
	/// <param name="szIp"></param>
	/// <param name="nPort"></param>
	/// <returns></returns>
	BOOL Accept(LPTSTR szIp, USHORT nPort);

	/// <summary>
	/// 客户端连接服务器
	/// </summary>
	BOOL Connect(LPTSTR szIp, USHORT nPort);

	/// <summary>
	/// 发送数据
	/// </summary>
	/// <param name="pBuff"></param>
	/// <param name="dwBufLne"></param>
	/// <returns></returns>
	DWORD Send(LPBYTE pBuff, DWORD dwBufLne);


	/// <summary>
	/// 接收数据
	/// </summary>
	/// <param name="pBuff"></param>
	/// <param name="dwBufLne"></param>
	/// <returns></returns>
	DWORD Recv(LPBYTE pBuff, DWORD dwBufLne);

	/// <summary>
	/// 关闭
	/// </summary>
	/// <returns></returns>
	VOID Close();

#pragma region 结构体定义

private:
#define DATALEN 1460
	//包类型
	enum PackageType
	{
		PT_SYN = 4,
		PT_ACK = 2,
		PT_DATA = 1,
		PT_FIN
	};

	/*包*/
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

		WORD m_nPt; //包类型
		WORD m_nLen; //数据长度
		DWORD m_nSeq;// a包序号
		DWORD m_nCheck;// 校验值
		BYTE m_aryData[DATALEN];//承载数据
	};
#pragma pack(pop)
	struct CPackageInfo
	{
		CPackageInfo() {};
		CPackageInfo(time_t tm, CPackage pkg) :m_tmLastTime(tm), m_pkg(pkg)
		{

		}
		time_t m_tmLastTime;//时间
		CPackage m_pkg;
	};
#pragma endregion 
private:
	VOID Clear();
	BOOL Init();
private:
	SOCKET m_sock;
	sockaddr_in m_siClient = {};
	sockaddr_in m_siService = {};
	
private:
	DWORD m_nNextSendSeq = 0;//下一次拆包的开始序号
	DWORD m_nNextRecvSeq = 0;//下一次存入缓冲区的包的序号

private:
	/// <summary>
	/// 存储发送包容器
	/// </summary>
	map<DWORD, CPackageInfo>m_mpSend;
	CLock m_lckForSendMp;//同步对象，m_mpSend的多线程同步

	/// <summary>
	/// 存储收到的包
	/// </summary>
	map<DWORD, CPackage>m_mpRecv;
	CLock m_lckForRecvMp;//同步对象，m_mpRecv的多线程同步

	/// <summary>
	/// 接收数据的缓冲区
	/// </summary>
	CByteStreamBuff m_bufRecv;
	CLock m_lckForbufRecv;//同步对象，m_bufRecv的多线程同步
private:
	HANDLE m_hSnedThread = NULL;
	HANDLE m_RecvThread = NULL;
	HANDLE m_HandleRecvPkgThread = NULL;
	BOOL m_bWorking = FALSE;
	/// <summary>
	/// 发包线程
	/// </summary>
	/// <param name="IpParam"></param>
	/// <returns></returns>
	static DWORD CALLBACK SnedThread(LPVOID IpParam);//发包线程

	/// <summary>
	/// 收包线程
	/// </summary>
	/// <param name="IpParam"></param>
	/// <returns></returns>
	static DWORD CALLBACK RecvThread(LPVOID IpParam);
	/// <summary>
	/// 将包从收包容器放入缓存区的线程
	/// </summary>
	/// <param name="IpParam"></param>
	/// <returns></returns>
	static DWORD CALLBACK HandleRecvPkgThread(LPVOID IpParam);
};