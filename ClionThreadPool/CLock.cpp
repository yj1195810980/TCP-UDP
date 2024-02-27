#include"CLock.h"


CLock::CLock()
{
	InitializeCriticalSection(&m_cs);/*��ʼ���ٽ�����*/

}

CLock::~CLock()
{
	DeleteCriticalSection(&m_cs);/*�ͷ��ٽ�����*/
}

void CLock::Lock()
{
	EnterCriticalSection(&m_cs);/*����*/
}

void CLock::Unlock()
{
	LeaveCriticalSection(&m_cs);/*����*/
}
