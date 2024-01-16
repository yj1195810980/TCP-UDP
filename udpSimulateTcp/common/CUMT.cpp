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
		int nLen = sizeof(m_siClient);
		nRet = recvfrom(m_sock, (char*)&pkg, sizeof(pkg), 0, (sockaddr*)&m_siClient, &nLen);
		if (nRet == 0 || nRet == SOCKET_ERROR || pkg.m_nPt != PT_SYN || pkg.m_nSeq != m_nNextRecvSeq)
		{
			std::cout << "recvfrom����" << std::endl;
			continue;
		}

		//�ص�һ����
		CPackage pkgSend(PT_SYN | PT_ACK, m_nNextSendSeq);
		nRet = sendto(m_sock, (char*)&pkgSend, sizeof(pkgSend), 0, (sockaddr*)&m_siClient, sizeof(m_siClient));
		if (nRet == SOCKET_ERROR)
		{
			std::cout << "sendto����" << std::endl;
			continue;
		}

		//�յڶ�����
		nRet = recvfrom(m_sock, (char*)&pkg, sizeof(pkg), 0, (sockaddr*)&m_siClient, &nLen);
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

	m_siService.sin_family = AF_INET;
	m_siService.sin_addr.s_addr = inet_addr(szIp);
	m_siService.sin_port = htons(nPort);

	//�ͻ��������˷���һ����
	CPackage pkgSend(PT_SYN, m_nNextSendSeq);
	int nRet = sendto(m_sock, (char*)&pkgSend, sizeof(pkgSend), 0, (sockaddr*)&m_siService, sizeof(m_siService));
	if (nRet == SOCKET_ERROR)
	{
		std::cout << "sendto����" << std::endl;
		Clear();
		return FALSE;
	}


	//�ͻ����յ�һ����
	CPackage pkg;
	int nLen = sizeof(m_siClient);
	nRet = recvfrom(m_sock, (char*)&pkg, sizeof(pkg), 0, (sockaddr*)&m_siClient, &nLen);
	if (nRet == 0 || nRet == SOCKET_ERROR || pkg.m_nPt != (PT_SYN | PT_ACK) || pkg.m_nSeq != m_nNextRecvSeq)
	{
		std::cout << "recvfrom����" << std::endl;
		Clear();
		return FALSE;
	}



	//�ͻ��˷��ڶ�����
	CPackage pkgSendAck(PT_ACK, m_nNextSendSeq);
	nRet = sendto(m_sock, (char*)&pkgSendAck, sizeof(pkgSendAck), 0, (sockaddr*)&m_siClient, sizeof(m_siClient));
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
	return 0;
}

DWORD CUMT::Recv(LPBYTE pBuff, DWORD dwBufLne)
{
	return 0;
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

DWORD CUMT::SnedThread(LPVOID IpParam)
{
	CUMT* pThis = (CUMT*)IpParam;
	while (pThis->m_bWorking)
	{

	}


	return 0;
}

DWORD CUMT::RecvThread(LPVOID IpParam)
{
	CUMT* pThis = (CUMT*)IpParam;
	while (pThis->m_bWorking)
	{

	}
	return 0;
}

DWORD CUMT::HandleRecvPkgThread(LPVOID IpParam)
{
	CUMT* pThis = (CUMT*)IpParam;
	while (pThis->m_bWorking)
	{

	}
	return 0;
}
