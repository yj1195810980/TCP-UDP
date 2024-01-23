#include<iostream>
#include"../common/CUMT.h"
#include<windows.h>
int main(int argn, char** argc) {

	CUMT umt;
	umt.Accept("127.0.0.1", 5566);

	HANDLE hFile = CreateFile("D:\\kerui\\test\\dis\\ProcessHacker.zip",                // name of the write
		GENERIC_WRITE,          // open for writing
		0,                      // do not share
		NULL,                   // default security
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,  // normal file
		NULL);                  // no attr. template



	DWORD dwFileSize = 0;
	umt.Recv((LPBYTE)&dwFileSize, sizeof(dwFileSize));
	DWORD dwBytesRecv = 0;
	LPBYTE pBuff = new  BYTE[0x10000];
	while (dwBytesRecv < dwFileSize)
	{
		DWORD nRet = umt.Recv(pBuff, 0x10000);
		DWORD dwBytesWrited = 0;
		WriteFile(hFile, pBuff, nRet, &dwBytesWrited, NULL);
		dwBytesRecv += nRet;
	}
	CloseHandle(hFile);
	system("pause");
	return 0;
}