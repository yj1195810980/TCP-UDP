// ChatServer.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
#include <iostream>
#include <list>

#include "../common/Proto.h"
#include "CSockinit.h"
#include <WinSock2.h>
#include <ws2tcpip.h>
#include "CSockinit.h"
using namespace std;

void OnLine(SOCKET sock, CPackge& pkg);
void OffLine(SOCKET sock, CPackge& pkg);
void OnPublic(SOCKET sock, CPackge& pkg);
void OnPrivaet(SOCKET sock, CPackge& pkg);


//存储所有上线的客户端
std::list<ClientInfo>g_lstCIs;

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

	printf("[log]:%s %d %s online\r\n", inet_ntoa(pkg.m_ci.m_si.sin_addr),
		pkg.m_ci.m_si.sin_port,
		pkg.m_ci.m_szName);

	for (auto& ci : g_lstCIs)
	{
		CPackge pkgSend(PT_ONLINE, ci);
		//发送给刚上线的客户端他之前有哪些客户端上线了
		sendto(sock, (char*)&pkgSend, sizeof(pkgSend), 0, (sockaddr*)&pkg.m_ci.m_si, sizeof(pkg.m_ci.m_si));
		//sock ,buff,sizeofbuff,0,地址和端口,地址和端口的大小
	}


	//保存登录的客户端列表
	g_lstCIs.push_back(pkg.m_ci);
	//通知其他客户端，有新客户端登录
	CPackge pkgSend(PT_ONLINE,pkg.m_ci);
	for (auto& ci : g_lstCIs)
	{
		//发送给之前上线的客户端，现在有新上线了
		sendto(sock, (char*)&pkgSend, sizeof(pkgSend), 0, (sockaddr*)&ci.m_si, sizeof(ci.m_si));
	}
	
}

void OffLine(SOCKET sock, CPackge& pkg)
{
	////下线
	//printf("[log]:%s %d %s offline\r\n", inet_ntoa(pkg.m_ci.m_si.sin_addr),
	//	pkg.m_ci.m_si.sin_port,
	//	pkg.m_ci.m_szName);
	//g_lstCIs.remove(pkg.m_ci);
	//CPackge pkgSend(PT_OFFLINE, pkg.m_ci);
	//for (auto& ci : g_lstCIs)
	//{
	//	sendto(sock, (char*)&pkgSend, sizeof(pkgSend), 0, (sockaddr*)&ci.m_si, sizeof(ci.m_si));
	//}
}


void OnPublic(SOCKET sock, CPackge& pkg)
{
	//群发
	for (auto& ci : g_lstCIs)
	{
		sendto(sock, (char*)&pkg, sizeof(pkg), 0, (sockaddr*)&ci.m_si, sizeof(ci.m_si));
	}
}

void OnPrivaet(SOCKET sock, CPackge& pkg)
{
	//私聊

}