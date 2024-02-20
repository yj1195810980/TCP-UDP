#include <iostream>
#include<WinSock2.h>
#include<ws2tcpip.h>
#include<Windows.h>
#include"CSockinit.h"

#define WIN32_LEAN_AND_MEAN
#pragma comment(lib, "Ws2_32.lib")

//使用tcp去模拟http


int main() {

	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		std::cout << "sock err" << std::endl;
	}


	sockaddr_in si;
	si.sin_family = AF_INET;
	si.sin_addr.S_un.S_addr = inet_addr("219.129.216.112");
	si.sin_port = htons(80);
	int nRet = connect(sock, (sockaddr*)&si, sizeof(si));
	if (nRet == SOCKET_ERROR) {
		std::cout << "connect err" << std::endl;
	}


	char szBuff[] = {
				"GET / HTTP/1.1\r\n"
				"Host: tool.webmasterhome.cn\r\n"
				"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/121.0.0.0 Safari/537.36 Edg/121.0.0.0\r\n"
				"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7\r\n"
				//"Accept-Encoding: gzip, deflate\r\n"去掉后 就相当于给服务器说发给我的返回包不能压缩
				"Accept-Language: zh,zh-CN;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6\r\n"
				"Cookie: __51uvsct__1v488Xpw9ngCTUiM=1; __51vcke__1v488Xpw9ngCTUiM=62deda20-03b7-5f44-9159-dd76c5067d44; __51vuft__1v488Xpw9ngCTUiM=1706798463795; _ga=GA1.1.282292209.1706798464; FCCDCF=%5Bnull%2Cnull%2Cnull%2C%5B%22CP5TLYAP5TLYAEsACBENAlEgAAAAAEPgAARoAAAQaQD2F2K2kKFkPCmQWYAQBCijYEAhQAAAAkCBIAAgAUgQAgFIIAgAIFAAAAAAAAAQEgCQAAQABAAAIACgAAAAAAIAAAAAAAQQAAAAAIAAAAAAAAEAAAAAAAQAAAAIAABEhCAAQQAEAAAAAAAQAAAAAAAAAAABAAAAAAAAAAAAAAAAAAAAgAA%22%2C%222~~dv.2072.70.89.93.108.122.149.196.2253.2299.259.2357.311.313.323.2373.338.358.2415.415.449.2506.2526.482.486.494.495.2568.2571.2575.540.574.2624.609.2677.864.981.1029.1048.1051.1095.1097.1201.1205.1211.1276.1301.1344.1365.1415.1423.1449.1451.1570.1577.1598.1651.1716.1735.1753.1765.1870.1878.1889.1958%22%2C%220F589E73-96C9-42C1-A243-29B22FC828C3%22%5D%5D; _ga_QFFHC336H3=GS1.1.1706798464.1.1.1706798511.13.0.0\r\n"
				"Upgrade-Insecure-Requests: 1\r\n\r\n"
	};

	send(sock, szBuff, sizeof(szBuff), 0);


	char szRecvBuff[MAXWORD] = {};
	nRet = recv(sock, szRecvBuff, sizeof(szRecvBuff), 0);
	if (nRet == 0 || nRet == SOCKET_ERROR) {
		std::cout << "recv err" << std::endl;
	}
	closesocket(sock);
	std::cout << szRecvBuff << std::endl;
	return 0;
}
