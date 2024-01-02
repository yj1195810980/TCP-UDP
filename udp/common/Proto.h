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
	PT_PRIVATE,//˽��
	PT_HEARTBEAT//������
};


#define NAMELEN 64 
/// <summary>
/// �ͻ�����Ϣ
/// </summary>
struct Client
{
	bool operator==(const Client& obj)
	{
		if (m_si.sin_addr.S_un.S_addr!=obj.m_si.sin_addr.S_un.S_addr)
		{
			return false;
		}
		if (m_si.sin_port!=obj.m_si.sin_port)
		{
			return false;
		}
		if (strcmp(m_szName,obj.m_szName)!=0)
		{
			return false;
		}
		return true;
	}


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
	CPackge(packageType py, Client& client, char* msg = nullptr)
		:m_pt(py), m_client(client)
	{
		if (msg != nullptr)
		{
			strcpy(m_szMsg, msg);
		}
	}
	CPackge(packageType py, Client& ci,Client&ciDst, char* msg = nullptr)
		:m_pt(py), m_client(ci), m_ciDst(ciDst)
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
	Client m_client;//�ͻ�����Ϣ
	Client m_ciDst;//˽�Ŀͻ��˵���Ϣ
	char m_szMsg[MASLEN];//��Ϣ
};