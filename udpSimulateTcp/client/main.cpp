#include<iostream>
#include"../common/CUMT.h"
#include<windows.h>
int main(int argn, char** argc) {

	CUMT umt;
	umt.Connect("127.0.0.1", 5566);

	HANDLE hFile = CreateFile("D:\\kerui\\test\\src\\ProcessHacker.zip",                // name of the write
		GENERIC_READ,          // open for writing
		0,                      // do not share
		NULL,                   // default security
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,  // normal file
		NULL);                  // no attr. template

	if (hFile == INVALID_HANDLE_VALUE)
	{
		cout << "文件打开失败";
		return 0;
	}

	DWORD dwFileSize = GetFileSize(hFile, NULL);//获取文件大小
	DWORD dwBytesSend = 0;
	umt.Send((LPBYTE)&dwFileSize, sizeof(dwFileSize));

	LPBYTE pBuff = new BYTE[0x10000];
	while (dwBytesSend < dwFileSize)
	{
		DWORD dwBytesRead = 0;
		::ReadFile(hFile, pBuff, 0x10000, &dwBytesRead,NULL);
		umt.Send(pBuff, dwBytesRead);
		dwBytesRead += dwBytesRead;
	}
	CloseHandle(hFile);


	system("pause");
	return 0;
}