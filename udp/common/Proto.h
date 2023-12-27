#pragma once
#include <WinSock2.h>
#include <ws2tcpip.h>
/// <summary>
/// 包类型定义
/// </summary>
enum packageType
{
	PT_ONLINE,//上线
	PT_OFFLINE,//下线
	PT_PUBLIC,//群聊
	PT_PRIVATE//私聊
};


#define NAMELEN 64 
/// <summary>
/// 客户端信息
/// </summary>
struct ClientInfo
{
	bool operator==(const ClientInfo& obj)
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


	sockaddr_in m_si;//ip地址和端口号
	char m_szName[NAMELEN];//名称
};


//暂时不考虑组包等，1472一个udp最大，1472-98=1374，此时如果不想组包和封包操作，那么需要将包的大小控制在
//1472内
#define MASLEN 1200
/// <summary>
/// 包
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

	packageType m_pt;//包类型
	ClientInfo m_ci;//客户端信息
	ClientInfo m_ciDst;//私聊客户端的信息
	char m_szMsg[MASLEN];//消息
};