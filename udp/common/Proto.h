#pragma once
#include <WinSock2.h>
#include <ws2tcpip.h>
/// <summary>
/// �����Ͷ���
/// </summary>
enum packageType
{
	PT_ONLINE,//����
	PT_OFFLINE,//����
	PT_PUBLIC,//Ⱥ��
	PT_PRIVATE//˽��
};


#define NAMELEN 64 
/// <summary>
/// �ͻ�����Ϣ
/// </summary>
struct ClientInfo
{
	sockaddr_in m_si;//ip��ַ�Ͷ˿ں�
	char m_szName[NAMELEN];//����
};


//��ʱ����������ȣ�1472һ��udp���1472-98=1374����ʱ�����������ͷ����������ô��Ҫ�����Ĵ�С������
//1472��
#define MASLEN 1200
/// <summary>
/// ��
/// </summary>
struct CPackge
{
	CPackge(packageType py, ClientInfo& ci, char* msg = nullptr)
		:m_pt(py), m_ci(ci)
	{
		if (msg != nullptr)
		{
			strcpy(m_szMsg, msg);
		}
	}
	CPackge(packageType py, ClientInfo& ci,ClientInfo&ciDst, char* msg = nullptr)
		:m_pt(py), m_ci(ci), m_ciDst(ciDst)
	{
		if (msg != nullptr)
		{
			strcpy(m_szMsg, msg);
		}
	}

	CPackge()
	{

	}

	packageType m_pt;//������
	ClientInfo m_ci;//�ͻ�����Ϣ
	ClientInfo m_ciDst;//˽�Ŀͻ��˵���Ϣ
	char m_szMsg[MASLEN];//��Ϣ
};