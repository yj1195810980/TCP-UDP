#pragma once
#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment(lib,"Ws2_32.lib")
class CSockinit
{
private:
	CSockinit();
	~CSockinit();
public:
	static CSockinit m_si;//��̬��Ա������main����ִ��
};

