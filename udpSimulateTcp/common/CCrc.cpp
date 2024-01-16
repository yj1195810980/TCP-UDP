#include "CCrc.h"
#include <stdio.h>

/*
* CRC校验算法,查表法
*/
using namespace std;
//位翻转
unsigned long Crc::bitrev(unsigned long input, int bw)
{
	int i;
	unsigned long var;
	var = 0;
	for (i = 0; i < bw; i++)
	{
		if (input & 0x01)
		{
			var |= 1 << (bw - 1 - i);
		}
		input >>= 1;
	}
	return var;
}

//码表生成
//如:X32+X26+...X1+1,poly=(1<<26)|...|(1<<1)|(1<<0)
void Crc::crc32_init(unsigned long poly)
{
	int i;
	int j;
	unsigned long c;

	poly = bitrev(poly, 32);
	for (i = 0; i < 256; i++)
	{
		c = i;
		for (j = 0; j < 8; j++)
		{
			if (c & 1)
			{
				c = poly ^ (c >> 1);
			}
			else
			{
				c = c >> 1;
			}
		}
		table[i] = c;
	}
}

unsigned long Crc::crc32(void* input, int len, unsigned long crc)
{
	int i;
	unsigned char index;
	unsigned char* pch;
	pch = (unsigned char*)input;
	for (i = 0; i < len; i++)
	{
		index = (unsigned char)(crc ^ *pch);
		crc = (crc >> 8) ^ table[index];
		pch++;
	}

	crc ^= 0xFFFFFFFF;
	return crc;
}

Crc::Crc()
{
	crc32_init(0x4C11DB7);
}
