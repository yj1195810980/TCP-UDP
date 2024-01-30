//
// Created by operatorp on 2024-01-29.
//
#include <iostream>
#include "CSockinit.h"


#pragma pack(push)
#pragma pack(1)
struct MyIcmp {
	BYTE type;//类型
	BYTE code;//错误码
	WORD checksum;//校验值 在icmp包中，这个校验值是对整个包的校验值，而不是单单data
	WORD ident;//id
	WORD seq;//序号
	BYTE data[32];//数据32个字节
};

struct ip_hdr
{
	unsigned char h_len : 4;
	unsigned char version : 4;
	unsigned char tos;
	unsigned short total_len;
	unsigned short ident;
	unsigned short frag_and_flags;
	unsigned char ttl;
	unsigned char proto;
	unsigned short checksum;
	unsigned int sorceIP;
	unsigned int destIP;
};
#pragma pack(pop)

#ifdef CRCPP_USE_NAMESPACE
using ::CRCPP::CRC;
#endif
#define CRCPP_USE_CPP11
using namespace std;



//计算校验和
unsigned short icmp_calc_checksum(char* icmp_packet, int size)
{
	unsigned short* sum = (unsigned short*)icmp_packet;
	unsigned int checksum = 0;
	while (size > 1) {
		checksum += ntohs(*sum++);
		size -= sizeof(unsigned short);
	}
	if (size) {
		*sum = *((unsigned char*)sum);
		checksum += ((*sum << 8) & 0xFF00);
	}

	checksum = (checksum >> 16) + (checksum & 0xffff);
	checksum += checksum >> 16;

	return (unsigned short)(~checksum);

}
int main() {

	/**
	 * AF_INET 类型
	 * SOCK_DGRAM 数据报
	 * IPPROTO_RAW 原始数据报
	 */
	SOCKET sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sock == INVALID_SOCKET) {
		cout << "socket ERR" << endl;
		return 0;
	}

	/**
	 * 发送icmp
	 */
	MyIcmp icmp;
	icmp.type = 8;
	icmp.code = 0;
	icmp.checksum = 0;//先设为0，让下面生成crc，发送到对面后，对面先将crc值取出，然后将该位置设置为0，这里的0是双方规定的，一样就行
	icmp.ident = 0x12;//id随便
	icmp.seq = 0x45;
	BYTE aryData[32] = { "0123456789qwertyuiopasdfghjklzx" };
	memcpy(icmp.data, aryData, sizeof(aryData));
	WORD checkSum=	icmp_calc_checksum((char*)&icmp, sizeof(icmp));//计算校验和
	icmp.checksum= htons(checkSum);//转换为网络字节序


	sockaddr_in si;
	si.sin_family = AF_INET;
	si.sin_port = 0; //在网络层没有端口的概念，对面应该是直接到网卡
	si.sin_addr.S_un.S_addr = inet_addr("101.226.4.6");


	sendto(sock, (char*)&icmp, sizeof(icmp), 0, (sockaddr*)&si, sizeof(si));

	
	
	sockaddr_in siRecv;
	int nLen = sizeof(siRecv);
	BYTE aryBuf[0x1000] = {};
	int nRet = recvfrom(sock, (char*)aryBuf, sizeof(aryBuf), 0, (sockaddr*)&siRecv, &nLen);

	ip_hdr* pIp = (ip_hdr*)aryBuf;
	MyIcmp* pIcmRecv = (MyIcmp*)(aryBuf + sizeof(ip_hdr));

	

	return 0;
}