#pragma once
#include<windows.h>
#include<vector>
using namespace std;

//字节流缓冲区
class CByteStreamBuff {

public:
	CByteStreamBuff();
	~CByteStreamBuff();
public:
	/// <summary>
	/// 写入缓冲区，附加在缓冲区末尾
	/// </summary>
	/// <param name="pBuff"></param>
	/// <param name="nSize"></param>
	void Write(LPBYTE pBuff, int nSize);

	/// <summary>
	/// 从缓冲区读指定字节数，读取的数据自动从缓冲区删除
	/// </summary>
	/// <param name="pBuff"></param>
	/// <param name="nSize"></param>
	void Read(LPBYTE pBuff, int nSize);


	/// <summary>
	/// 从缓冲区读指定字节数，读取的数据不会从缓冲区删除
	/// </summary>
	/// <param name="pBuff"></param>
	/// <param name="nSize"></param>
	void Peek(char* pBuff, int nSize);

	/// <summary>
	/// 获取缓冲区中的数据大小
	/// </summary>
	/// <returns></returns>
	int GetSize()const;

private:
	
	vector<BYTE>m_vctBuff;
};