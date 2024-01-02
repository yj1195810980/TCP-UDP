// ChatServer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
#include <iostream>
#include <list>

#include "../common/Proto.h"
#include "../common/CSockinit.h"
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <time.h>
using namespace std;


struct CClientInfo
{
	time_t m_nLastTime = 0;
	Client m_client;
};



void OnLine(SOCKET sock, CPackge& pkg);
void OffLine(SOCKET sock, CPackge& pkg);
void OnPublic(SOCKET sock, CPackge& pkg);
void OnPrivaet(SOCKET sock, CPackge& pkg);
void OnHearBeat(SOCKET sock, CPackge& pkg);


//存储所有上线的客户端
std::list<CClientInfo>g_lstCIs;

/*心跳包检测线程*/
DWORD WINAPI CheckHeartBeatThreadFunc(LPVOID obj)
{
	const time_t nTmElappse = 5;
	SOCKET sockServer = (SOCKET)obj;
	while (true)
	{
		time_t tmCurrent = time(NULL);/*获取当前时间*/
		for (auto it = g_lstCIs.begin(); it != g_lstCIs.end(); ++it)
		{
			printf("[log]:%s %d %s upEartBeatData\r\n", inet_ntoa(it->m_client.m_si.sin_addr),
				it->m_client.m_si.sin_port,
				it->m_client.m_szName);
			if (tmCurrent - it->m_nLastTime >= nTmElappse)/*单位秒*/
			{
				/*如果时间间隔大于设定的秒说明下线了*/
				CPackge pkg(PT_OFFLINE, it->m_client);
				OffLine(sockServer, pkg);
				break;
			}
		}

	}

	return 0;
}


int main()
{

	/*创建套接字*/
	SOCKET sckt = ::socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sckt == INVALID_SOCKET)
	{
		cout << "创建套接字失败";
		return 0;
	}

	sockaddr_in si;
	si.sin_family = AF_INET;
	si.sin_port = htons(0x7788);//大尾转小尾，本机字节序转网络字节序
	in_addr in = { 0 };
	if (inet_pton(AF_INET, "127.0.0.1", &in) == 1)//将127.0.0.1转换为小端
	{
		si.sin_addr.S_un.S_addr = in.S_un.S_addr;//小尾地址使用inet_pton将其转换为字符串转换为数值，然后再转换为小尾
		int nRet = ::bind(sckt, (sockaddr*)&si, sizeof(si));
		if (nRet == SOCKET_ERROR)
		{
			cout << "bind失败";
			return 0;
		}


		HANDLE m_hThread;//多线程心跳包线程
		SECURITY_ATTRIBUTES se = {};
		se.nLength = sizeof(se);
		se.bInheritHandle = NULL;
		m_hThread = ::CreateThread(&se, 0, CheckHeartBeatThreadFunc, (LPVOID)sckt, 0, NULL);
		if (m_hThread == NULL)
		{
			cout << "心跳线程失败" << endl;
			::closesocket(sckt);
			return 0;
		}

		//收数据
		sockaddr_in siFrom = {};
		int nSizeofSi = sizeof(sockaddr_in);

		while (true)
		{
			CPackge pack;//接收的是一个自定义的包
			//将接收的数据按照char类型进行存放
			nRet = ::recvfrom(sckt, (char*)&pack, sizeof(pack), 0, (sockaddr*)&siFrom, &nSizeofSi);
			if (nRet == 0 || nRet == SOCKET_ERROR)
			{
				cout << "接收数据失败";
				return 0;
			}
			else
			{
				//接收数据
				switch (pack.m_pt)
				{
				case PT_ONLINE:
					OnLine(sckt, pack);
					break;
				case PT_OFFLINE:
					OffLine(sckt, pack);
					break;
				case PT_PUBLIC:
					OnPublic(sckt, pack);
					break;
				case PT_PRIVATE:
					OnPrivaet(sckt, pack);
					break;
				case PT_HEARTBEAT:
					OnHearBeat(sckt, pack);
					break;
				default:
					break;
				}
			}
			//给客户端发消息
			char szSend[] = { "hello client" };
			nRet = ::sendto(sckt, szSend, sizeof(szSend), 0, (sockaddr*)&siFrom, sizeof(siFrom));
			if (nRet == SOCKET_ERROR)
			{
				cout << "sendto 失败";
				return 0;
			}
		}
	}
	return 0;
}


void OnLine(SOCKET sock, CPackge& pkg)
{
	//上线

	printf("[log]:%s %d %s online\r\n", inet_ntoa(pkg.m_client.m_si.sin_addr),
		pkg.m_client.m_si.sin_port,
		pkg.m_client.m_szName);

	for (auto& ci : g_lstCIs)
	{
		CPackge pkgSend(PT_ONLINE, ci.m_client);
		//发送给刚上线的客户端他之前有哪些客户端上线了
		sendto(sock, (char*)&pkgSend, sizeof(pkgSend), 0, (sockaddr*)&pkg.m_client.m_si, sizeof(pkg.m_client.m_si));
		//sock ,buff,sizeofbuff,0,地址和端口,地址和端口的大小
	}


	//保存登录的客户端列表
	g_lstCIs.push_back(CClientInfo{ time(NULL),pkg.m_client });
	//通知其他客户端，有新客户端登录
	CPackge pkgSend(PT_ONLINE, pkg.m_client);
	for (auto& ci : g_lstCIs)
	{
		//发送给之前上线的客户端，现在有新上线了
		sendto(sock, (char*)&pkgSend, sizeof(pkgSend), 0, (sockaddr*)&ci.m_client.m_si, sizeof(ci.m_client.m_si));
	}
}



void OffLine(SOCKET sock, CPackge& pkg)
{
	printf("[log]:%s %d %s offline\r\n", inet_ntoa(pkg.m_client.m_si.sin_addr),
		pkg.m_client.m_si.sin_port,
		pkg.m_client.m_szName);

	g_lstCIs.remove_if([&](const CClientInfo& item) {return pkg.m_client == item.m_client; });/*删除下线的客户端*/

	for (auto& ci : g_lstCIs)
	{
		sendto(sock, (char*)&pkg, sizeof(pkg), 0, (sockaddr*)&ci.m_client.m_si, sizeof(ci.m_client.m_si));
	}

}


void OnPublic(SOCKET sock, CPackge& pkg)
{
	//群发
	for (auto& ci : g_lstCIs)
	{
		sendto(sock, (char*)&pkg, sizeof(pkg), 0, (sockaddr*)&ci.m_client.m_si, sizeof(ci.m_client.m_si));
	}
}

void OnHearBeat(SOCKET sock, CPackge& pkg)
{
	/*使用心跳包更新心跳时间*/
	/*后续可能会修改list为map，提高效率*/
	for (auto& it : g_lstCIs)
	{
		if (it.m_client == pkg.m_client)
		{
			it.m_nLastTime = time(NULL);
			break;
		}
	}

}


void OnPrivaet(SOCKET sock, CPackge& pkg)
{
	//私聊
	sendto(sock, (char*)&pkg, sizeof(pkg), 0, (sockaddr*)&pkg.m_ciDst, sizeof(pkg.m_ciDst));
}