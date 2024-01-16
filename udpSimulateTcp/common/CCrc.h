#pragma once
#include <iostream>
#include <cstdint>
#include <fstream>
#include <vector>


class Crc {
public:
	Crc();
	unsigned long crc32( void* input, int len, unsigned long crc = 0xFFFFFFFF);
private:
	void crc32_init(unsigned long poly);
	unsigned long bitrev(unsigned long input, int bw);
private:
	unsigned long table[256];
};