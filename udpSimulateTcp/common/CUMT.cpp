#include "CUMT.h"

BOOL CUMT::Accept(LPTSTR szIp, USHORT nPort)
{
	//服务端
	m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_sock == INVALID_SOCKET)
	{
		std::cout << "socket 失败" << std::endl;
		return false;
	}
	sockaddr_in service = {};
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr(szIp);
	service.sin_port = htons(nPort);
	int nRet = bind(m_sock, (SOCKADDR*)&service, sizeof(service));

	if (nRet == SOCKET_ERROR)
	{
		std::cout << "bind 失败" << std::endl;
		closesocket(m_sock);
		return false;
	}

	while (true)
	{
		//收第一个包
		CPackage pkg;
		int nLen = sizeof(m_siDst);
		nRet = recvfrom(m_sock, (char*)&pkg, sizeof(pkg), 0, (sockaddr*)&m_siDst, &nLen);
		if (nRet == 0 || nRet == SOCKET_ERROR || pkg.m_nPt != PT_SYN || pkg.m_nSeq != m_nNextRecvSeq)
		{
			std::cout << "recvfrom错误" << std::endl;
			continue;
		}

		//回第一个包
		CPackage pkgSend(PT_SYN | PT_ACK, m_nNextSendSeq);
		nRet = sendto(m_sock, (char*)&pkgSend, sizeof(pkgSend), 0, (sockaddr*)&m_siDst, sizeof(m_siDst));
		if (nRet == SOCKET_ERROR)
		{
			std::cout << "sendto错误" << std::endl;
			continue;
		}

		//收第二个包
		nRet = recvfrom(m_sock, (char*)&pkg, sizeof(pkg), 0, (sockaddr*)&m_siDst, &nLen);
		if (nRet == 0 || nRet == SOCKET_ERROR || pkg.m_nPt != PT_ACK || pkg.m_nSeq != m_nNextRecvSeq)
		{
			std::cout << "recvfrom错误" << std::endl;
			continue;
		}

		//连接建立
		break;
	}

	return Init();
}

BOOL CUMT::Connect(LPTSTR szIp, USHORT nPort)
{
	//客户端
	m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_sock == INVALID_SOCKET)
	{
		std::cout << "socket 失败" << std::endl;
		return false;
	}

	m_siDst.sin_family = AF_INET;
	m_siDst.sin_addr.s_addr = inet_addr(szIp);
	m_siDst.sin_port = htons(nPort);

	//客户端向服务端发第一个包
	CPackage pkgSend(PT_SYN, m_nNextSendSeq);
	int nRet = sendto(m_sock, (char*)&pkgSend, sizeof(pkgSend), 0, (sockaddr*)&m_siDst, sizeof(m_siDst));
	if (nRet == SOCKET_ERROR)
	{
		std::cout << "sendto错误" << std::endl;
		Clear();
		return FALSE;
	}


	//客户端收第一个包
	CPackage pkg;
	int nLen = sizeof(m_siDst);
	nRet = recvfrom(m_sock, (char*)&pkg, sizeof(pkg), 0, (sockaddr*)&m_siDst, &nLen);
	if (nRet == 0 || nRet == SOCKET_ERROR || pkg.m_nPt != (PT_SYN | PT_ACK) || pkg.m_nSeq != m_nNextRecvSeq)
	{
		std::cout << "recvfrom错误" << std::endl;
		Clear();
		return FALSE;
	}



	//客户端发第二个包
	CPackage pkgSendAck(PT_ACK, m_nNextSendSeq);
	nRet = sendto(m_sock, (char*)&pkgSendAck, sizeof(pkgSendAck), 0, (sockaddr*)&m_siDst, sizeof(m_siDst));
	if (nRet == SOCKET_ERROR)
	{
		std::cout << "sendto错误" << std::endl;
		Clear();
		return FALSE;
	}

	//连接建立
	return Init();
}

DWORD CUMT::Send(LPBYTE pBuff, DWORD dwBufLne)
{
	//限制发包容器中的包大概为100个
	while (true)
	{
		m_lckForSendMp.Lock();
		DWORD dwSize = m_mpSend.size();
		m_lckForSendMp.Unlock();
		if (dwSize > 100)
		{
			Sleep(1);
			continue;
		}
		else
		{
			break;
		}
	}

	m_lckForSendMp.Lock();//加锁
	//拆包->丢容器->线程容器取包，发送
	DWORD dwCnt = (dwBufLne % DATALEN == 0 ? dwBufLne / DATALEN : (dwBufLne / DATALEN + 1));//计算出有多少个包
	for (DWORD i = 0; i < dwCnt; ++i)
	{
		DWORD dwDataLen = DATALEN;
		if (i == dwCnt - 1)//如果是最后一个包
		{
			dwDataLen = (dwBufLne - i * DATALEN);
		}
		CPackage pkg(PT_DATA, m_nNextSendSeq, pBuff + i * DATALEN, dwDataLen);
		m_mpSend[m_nNextSendSeq] = CPackageInfo(0, pkg);//发送的时候加上时间 将其写成0时间
		Log("[umt]:package ==> map seq:%d", m_nNextSendSeq);
		++m_nNextSendSeq;
	}
	m_lckForSendMp.Unlock();//解锁
	return dwBufLne;
}

DWORD CUMT::Recv(LPBYTE pBuff, DWORD dwBufLne)
{
	while (true)
	{
		m_lckForbufRecv.Lock();
		DWORD dwSize = m_bufRecv.GetSize();
		m_lckForbufRecv.Unlock();
		if (dwSize <= 0)
		{
			//无数据，阻塞住，线程切出去
			Sleep(1);
		}
		else
		{
			//有数据，关闭退出循环
			break;
		}
	}

	m_lckForbufRecv.Lock();

	//如果数据长度大于要的长度就给要的长度的数据，否则就给现有数据的长度数据
	DWORD dwDataLen = (m_bufRecv.GetSize() > dwBufLne) ? dwBufLne : m_bufRecv.GetSize();//还有比他多的情况
	m_bufRecv.Read(pBuff, dwDataLen);

	m_lckForbufRecv.Unlock();
	return dwDataLen;
}

VOID CUMT::Close()
{
	return VOID();
}

VOID CUMT::Clear()
{
	closesocket(m_sock);
	m_bWorking = FALSE;
	if (m_hSnedThread != NULL)
	{
		CloseHandle(m_hSnedThread);
	}
	if (m_RecvThread != NULL)
	{
		CloseHandle(m_RecvThread);
	}
	if (m_HandleRecvPkgThread != NULL)
	{
		CloseHandle(m_HandleRecvPkgThread);
	}

}

BOOL CUMT::Init()
{
	m_nNextRecvSeq++;
	m_nNextSendSeq++;
	m_bWorking = TRUE;
	m_hSnedThread = CreateThread(NULL, 0, SnedThread, this, 0, NULL);
	if (m_hSnedThread == NULL)
	{
		Clear();
		return false;
	}
	m_RecvThread = CreateThread(NULL, 0, RecvThread, this, 0, NULL);
	if (m_RecvThread == NULL)
	{
		Clear();
		return false;
	}
	m_HandleRecvPkgThread = CreateThread(NULL, 0, HandleRecvPkgThread, this, 0, NULL);
	if (m_HandleRecvPkgThread == NULL)
	{
		Clear();
		return false;
	}
	return true;
}

VOID CUMT::Log(const char* szFmt, ...)
{
	char szBuff[MAXWORD] = {};
	va_list vl;
	va_start(vl, szFmt);
	vsprintf(szBuff, szFmt, vl);
	va_end(vl);
	OutputDebugString(szBuff);
}

DWORD CUMT::SnedThread(LPVOID IpParam)
{
	CUMT* pThis = (CUMT*)IpParam;
	while (pThis->m_bWorking)
	{
		pThis->m_lckForSendMp.Lock();
		ULONGLONG tmCurrent = GetTickCount64();
		for (auto& pi : pThis->m_mpSend)
		{
			//发送没有发出去的包
			//重发超时的包
			//没有发送的包时间为0他也算做超时的，反正都是需要重发
			if ((tmCurrent - pi.second.m_tmLastTime) > pThis->m_tmElapse)//当前时间减去发送的时间大于规定时间则发送
			{
				sendto(pThis->m_sock,
					(char*)&pi.second.m_pkg,
					sizeof(pi.second.m_pkg),
					0,
					(sockaddr*)&pThis->m_siDst,
					sizeof(pThis->m_siDst));
				pi.second.m_tmLastTime = tmCurrent;//更新时间
				pThis->Log("[umt]:package ==> net seq:%d", pi.second.m_pkg.m_nSeq);
			}

		}
		pThis->m_lckForSendMp.Unlock();
	}
	return 0;
}

DWORD CUMT::RecvThread(LPVOID IpParam)
{
	CUMT* pThis = (CUMT*)IpParam;
	while (pThis->m_bWorking)
	{
		sockaddr_in si = {};
		int nLen = sizeof(si);
		CPackage pkg;
		int nRet = recvfrom(pThis->m_sock,
			(char*)&pkg,
			sizeof(pkg),
			0,
			(sockaddr*)&si, &nLen);
		if (nRet == 0 || nRet == SOCKET_ERROR)
		{
			connect;
		}

		//判断包的类型
		switch (pkg.m_nPt)
		{
		case PackageType::PT_ACK:
		{
			pThis->m_lckForSendMp.Lock();
			pThis->Log("[umt]:package ==> ack seq:%d", pkg.m_nSeq);
			pThis->m_mpSend.erase(pkg.m_nSeq);//移除
			pThis->m_lckForSendMp.Unlock();
			break;
		}
		case PackageType::PT_DATA:
		{
			//校验
			Crc crc;
			DWORD  dwCheck = crc.crc32(pkg.m_aryData, pkg.m_nLen);


			if (dwCheck != pkg.m_nCheck)
			{
				//校验失败丢弃包
				break;
			}

			//校验成功回复ack
			CPackage pkgACK(PT_ACK, pkg.m_nSeq);
			int nRet = sendto(pThis->m_sock,
				(char*)&pkgACK,
				sizeof(pkgACK),
				0,
				(sockaddr*)&pThis->m_siDst,
				sizeof(pThis->m_siDst)
			);
			if (nRet == 0 || nRet == SOCKET_ERROR)
			{
				//目前还未想清楚失败如何做

			}
			//包进容器
			pThis->m_lckForRecvMp.Lock();
			if (pThis->m_mpRecv.find(pkg.m_nSeq) != pThis->m_mpRecv.end() //容器中此序号包已经存在
				|| pkg.m_nSeq < pThis->m_nNextRecvSeq)//此序号包中的序号已经进入缓冲区
			{
				pThis->m_lckForRecvMp.Unlock();
				break;
			}
			pThis->m_mpRecv[pkg.m_nSeq] = pkg;//存包
			pThis->Log("[umt]:package net ==> map seq:%d", pkg.m_nSeq);
			pThis->m_lckForRecvMp.Unlock();
			break;
		}
		default:
			break;
		}
	}
	return 0;
}

DWORD CUMT::HandleRecvPkgThread(LPVOID IpParam)
{
	CUMT* pThis = (CUMT*)IpParam;
	while (pThis->m_bWorking)
	{
		pThis->m_lckForbufRecv.Lock();

		while (true)
		{
			pThis->m_lckForRecvMp.Lock();
			auto nRet = pThis->m_mpRecv.find(pThis->m_nNextRecvSeq);
			if (nRet != pThis->m_mpRecv.end())
			{
				//如果序号能顺序，那么数据进缓冲区
				CPackage pkg = pThis->m_mpRecv[pThis->m_nNextRecvSeq];
				pThis->m_bufRecv.Write(pkg.m_aryData, pkg.m_nLen);
				pThis->Log("[umt]:package ==> buf seq:%d", pThis->m_nNextRecvSeq);
				//从收包容器移除包
				pThis->m_mpRecv.erase(pThis->m_nNextRecvSeq);

				//更新包
				++pThis->m_nNextRecvSeq;
			}
			else
			{
				pThis->m_lckForRecvMp.Unlock();
				break;
			}
			pThis->m_lckForRecvMp.Unlock();
		}
		pThis->m_lckForbufRecv.Unlock();

		//切出线程
		Sleep(1);
	}
	return 0;
}
