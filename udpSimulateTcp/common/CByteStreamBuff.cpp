#include "CByteStreamBuff.h"

CByteStreamBuff::CByteStreamBuff()
{
}

CByteStreamBuff::~CByteStreamBuff()
{
}

void CByteStreamBuff::Write(LPBYTE pBuff, int nSize)
{
	for (int i = 0; i < nSize; ++i)
	{
		m_vctBuff.push_back(pBuff[i]);
	}
}

void CByteStreamBuff::Read(LPBYTE pBuff, int nSize)
{
	memcpy(pBuff, m_vctBuff.data(), nSize);
	auto itrBegin = m_vctBuff.begin();
	auto iteEnd = itrBegin + nSize;
	m_vctBuff.erase(itrBegin, iteEnd);
}

void CByteStreamBuff::Peek(char* pBuff, int nSize)
{
	memcpy(pBuff, m_vctBuff.data(), nSize);
}

int CByteStreamBuff::GetSize() const
{
	return m_vctBuff.size();
}
