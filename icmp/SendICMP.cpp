//
// Created by operatorp on 2024-01-30.
//

#include "SendICMP.h"

namespace ICMP {
    unsigned short SendICMP::icmp_calc_checksum(char *icmp_packet, int size) {
        unsigned short *sum = (unsigned short *) icmp_packet;
        unsigned int checksum = 0;
        while (size > 1) {
            checksum += ntohs(*sum++);
            size -= sizeof(unsigned short);
        }
        if (size) {
            *sum = *((unsigned char *) sum);
            checksum += ((*sum << 8) & 0xFF00);
        }

        checksum = (checksum >> 16) + (checksum & 0xffff);
        checksum += checksum >> 16;

        return (unsigned short) (~checksum);
    }

    unsigned short SendICMP::littleNet2BigNet(unsigned short da) {
        return htons(da);//转换为网络字节序
    }
} // ICMP
IcmpPack::IcmpPack(unsigned char type, unsigned char code, unsigned short ident, unsigned short seq,
                   unsigned char *data) {




}
