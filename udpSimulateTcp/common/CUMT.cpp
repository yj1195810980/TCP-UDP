#include "CUMT.h"

BOOL CUMT::Accept(LPTSTR szIp, USHORT nPort)
{
	//�����
	m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_sock == INVALID_SOCKET)
	{
		std::cout << "socket ʧ��" << std::endl;
		return false;
	}
	sockaddr_in service = {};
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = inet_addr(szIp);
	service.sin_port = htons(nPort);
	int nRet = bind(m_sock, (SOCKADDR*)&service, sizeof(service));

	if (nRet == SOCKET_ERROR)
	{
		std::cout << "bind ʧ��" << std::endl;
		closesocket(m_sock);
		return false;
	}

	while (true)
	{
		//�յ�һ����
		CPackage pkg;
		int nLen = sizeof(m_siDst);
		nRet = recvfrom(m_sock, (char*)&pkg, sizeof(pkg), 0, (sockaddr*)&m_siDst, &nLen);
		if (nRet == 0 || nRet == SOCKET_ERROR || pkg.m_nPt != PT_SYN || pkg.m_nSeq != m_nNextRecvSeq)
		{
			std::cout << "recvfrom����" << std::endl;
			continue;
		}

		//�ص�һ����
		CPackage pkgSend(PT_SYN | PT_ACK, m_nNextSendSeq);
		nRet = sendto(m_sock, (char*)&pkgSend, sizeof(pkgSend), 0, (sockaddr*)&m_siDst, sizeof(m_siDst));
		if (nRet == SOCKET_ERROR)
		{
			std::cout << "sendto����" << std::endl;
			continue;
		}

		//�յڶ�����
		nRet = recvfrom(m_sock, (char*)&pkg, sizeof(pkg), 0, (sockaddr*)&m_siDst, &nLen);
		if (nRet == 0 || nRet == SOCKET_ERROR || pkg.m_nPt != PT_ACK || pkg.m_nSeq != m_nNextRecvSeq)
		{
			std::cout << "recvfrom����" << std::endl;
			continue;
		}

		//���ӽ���
		break;
	}

	return Init();
}

BOOL CUMT::Connect(LPTSTR szIp, USHORT nPort)
{
	//�ͻ���
	m_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (m_sock == INVALID_SOCKET)
	{
		std::cout << "socket ʧ��" << std::endl;
		return false;
	}

	m_siDst.sin_family = AF_INET;
	m_siDst.sin_addr.s_addr = inet_addr(szIp);
	m_siDst.sin_port = htons(nPort);

	//�ͻ��������˷���һ����
	CPackage pkgSend(PT_SYN, m_nNextSendSeq);
	int nRet = sendto(m_sock, (char*)&pkgSend, sizeof(pkgSend), 0, (sockaddr*)&m_siDst, sizeof(m_siDst));
	if (nRet == SOCKET_ERROR)
	{
		std::cout << "sendto����" << std::endl;
		Clear();
		return FALSE;
	}


	//�ͻ����յ�һ����
	CPackage pkg;
	int nLen = sizeof(m_siDst);
	nRet = recvfrom(m_sock, (char*)&pkg, sizeof(pkg), 0, (sockaddr*)&m_siDst, &nLen);
	if (nRet == 0 || nRet == SOCKET_ERROR || pkg.m_nPt != (PT_SYN | PT_ACK) || pkg.m_nSeq != m_nNextRecvSeq)
	{
		std::cout << "recvfrom����" << std::endl;
		Clear();
		return FALSE;
	}



	//�ͻ��˷��ڶ�����
	CPackage pkgSendAck(PT_ACK, m_nNextSendSeq);
	nRet = sendto(m_sock, (char*)&pkgSendAck, sizeof(pkgSendAck), 0, (sockaddr*)&m_siDst, sizeof(m_siDst));
	if (nRet == SOCKET_ERROR)
	{
		std::cout << "sendto����" << std::endl;
		Clear();
		return FALSE;
	}

	//���ӽ���
	return Init();
}

DWORD CUMT::Send(LPBYTE pBuff, DWORD dwBufLne)
{
	//���Ʒ��������еİ����Ϊ100��
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

	m_lckForSendMp.Lock();//����
	//���->������->�߳�����ȡ��������
	DWORD dwCnt = (dwBufLne % DATALEN == 0 ? dwBufLne / DATALEN : (dwBufLne / DATALEN + 1));//������ж��ٸ���
	for (DWORD i = 0; i < dwCnt; ++i)
	{
		DWORD dwDataLen = DATALEN;
		if (i == dwCnt - 1)//��������һ����
		{
			dwDataLen = (dwBufLne - i * DATALEN);
		}
		CPackage pkg(PT_DATA, m_nNextSendSeq, pBuff + i * DATALEN, dwDataLen);
		m_mpSend[m_nNextSendSeq] = CPackageInfo(0, pkg);//���͵�ʱ�����ʱ�� ����д��0ʱ��
		Log("[umt]:package ==> map seq:%d", m_nNextSendSeq);
		++m_nNextSendSeq;
	}
	m_lckForSendMp.Unlock();//����
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
			//�����ݣ�����ס���߳��г�ȥ
			Sleep(1);
		}
		else
		{
			//�����ݣ��ر��˳�ѭ��
			break;
		}
	}

	m_lckForbufRecv.Lock();

	//������ݳ��ȴ���Ҫ�ĳ��Ⱦ͸�Ҫ�ĳ��ȵ����ݣ�����͸��������ݵĳ�������
	DWORD dwDataLen = (m_bufRecv.GetSize() > dwBufLne) ? dwBufLne : m_bufRecv.GetSize();//���б���������
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

	//��2���հ��߳�
	m_RecvThread = CreateThread(NULL, 0, RecvThread, this, 0, NULL);
	if (m_RecvThread == NULL)
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
			//����û�з���ȥ�İ�
			//�ط���ʱ�İ�
			//û�з��͵İ�ʱ��Ϊ0��Ҳ������ʱ�ģ�����������Ҫ�ط�
			ULONGLONG tmCurrent = GetTickCount64();
			if ((tmCurrent - pi.second.m_tmLastTime) > pThis->m_tmElapse)//��ǰʱ���ȥ���͵�ʱ����ڹ涨ʱ������
			{
				if (pi.second.m_tmLastTime == 0)
				{
					//����״ΰ���־
					pThis->Log("[umt]:package ==>first  net  seq:%d", pi.second.m_pkg.m_nSeq);
				}
				else
				{
					//�����ʱ����־
					pThis->Log("[umt]:package ==>timeout net seq:%d", pi.second.m_pkg.m_nSeq);
				}

				sendto(pThis->m_sock,
					(char*)&pi.second.m_pkg,
					sizeof(pi.second.m_pkg),
					0,
					(sockaddr*)&pThis->m_siDst,
					sizeof(pThis->m_siDst));
				pi.second.m_tmLastTime = tmCurrent;//����ʱ��
			}
		}
		pThis->m_lckForSendMp.Unlock();

		//�г��߳�
		Sleep(1);

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

		//�жϰ�������
		switch (pkg.m_nPt)
		{
		case PackageType::PT_ACK:
		{
			pThis->m_lckForSendMp.Lock();
			pThis->Log("[umt]:package ==> ack seq:%d", pkg.m_nSeq);
			pThis->m_mpSend.erase(pkg.m_nSeq);//�Ƴ�
			pThis->m_lckForSendMp.Unlock();
			break;
		}
		case PackageType::PT_DATA:
		{
			//У��
			Crc crc;
			DWORD  dwCheck = crc.crc32(pkg.m_aryData, pkg.m_nLen);


			if (dwCheck != pkg.m_nCheck)
			{
				//У��ʧ�ܶ�����
				break;
			}

			//У��ɹ��ظ�ack
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
				//Ŀǰ��δ�����ʧ�������

			}
			//��������
			pThis->m_lckForRecvMp.Lock();
			if (pThis->m_mpRecv.find(pkg.m_nSeq) != pThis->m_mpRecv.end() //�����д���Ű��Ѿ�����
				|| pkg.m_nSeq < pThis->m_nNextRecvSeq)//����Ű��е�����Ѿ����뻺����
			{
				pThis->m_lckForRecvMp.Unlock();
				break;
			}
			pThis->m_mpRecv[pkg.m_nSeq] = pkg;//���
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
				//��������˳����ô���ݽ�������
				CPackage pkg = pThis->m_mpRecv[pThis->m_nNextRecvSeq];
				pThis->m_bufRecv.Write(pkg.m_aryData, pkg.m_nLen);
				pThis->Log("[umt]:package ==> buf seq:%d", pThis->m_nNextRecvSeq);
				//���հ������Ƴ���
				pThis->m_mpRecv.erase(pThis->m_nNextRecvSeq);

				//���°�
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

		//�г��߳�
		Sleep(1);
	}
	return 0;
}
