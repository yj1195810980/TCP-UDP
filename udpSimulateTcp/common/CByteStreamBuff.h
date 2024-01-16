#pragma once
#include<vector>
using namespace std;

//�ֽ���������
class CByteStreamBuff {

public:
	CByteStreamBuff();
	~CByteStreamBuff();
public:
	/// <summary>
	/// д�뻺�����������ڻ�����ĩβ
	/// </summary>
	/// <param name="pBuff"></param>
	/// <param name="nSize"></param>
	void Write(const char* pBuff, int nSize);

	/// <summary>
	/// �ӻ�������ָ���ֽ�������ȡ�������Զ��ӻ�����ɾ��
	/// </summary>
	/// <param name="pBuff"></param>
	/// <param name="nSize"></param>
	void Read(char* pBuff, int nSize);


	/// <summary>
	/// �ӻ�������ָ���ֽ�������ȡ�����ݲ���ӻ�����ɾ��
	/// </summary>
	/// <param name="pBuff"></param>
	/// <param name="nSize"></param>
	void Peek(char* pBuff, int nSize);

	/// <summary>
	/// ��ȡ�������е����ݴ�С
	/// </summary>
	/// <returns></returns>
	int GetSize()const;

private:
	
	vector<char>m_vctBuff;
};