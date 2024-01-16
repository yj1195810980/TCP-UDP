#include"CLock.h"


CLock::CLock()
{
	InitializeCriticalSection(&m_cs);/*初始化临界区锁*/

}

CLock::~CLock()
{
	DeleteCriticalSection(&m_cs);/*释放临界区锁*/
}

void CLock::Lock()
{
	EnterCriticalSection(&m_cs);/*加锁*/
}

void CLock::Unlock()
{
	LeaveCriticalSection(&m_cs);/*解锁*/
}
