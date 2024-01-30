//
// Created by operatorp on 2024-01-30.
//

#ifndef MYICMP_SENDICMP_H
#define MYICMP_SENDICMP_H


//对齐方式1字节
#pragma pack(push)//存储之前的对齐方式
#pragma pack(1)//改变对齐方式
/**
 * icmp包
 */
struct IcmpPack {
    BYTE m_type;//类型
    BYTE m_code;//错误码
    WORD m_checksum;//校验值 在icmp包中，这个校验值是对整个包的校验值，而不是单单data
    WORD m_ident;//id
    WORD m_seq;//序号
    BYTE m_data[32];//数据32个字节

public:
    /**
     * 构造函数
     * @param type  类型
     * @param code  错误码
     * @param ident id
     * @param seq  序号
     * @param data 数据指针
     */
    IcmpPack(unsigned char type, unsigned char code, unsigned short ident, unsigned short seq, unsigned char *data);

};

/**
 * ip层包
 */
struct ip_hdrPack
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
#pragma pack(pop)//弹出报错的对齐方式

namespace ICMP {
/**
 * 该类用于创建ICMP包发送回显对方发送回来的ICMP包
 */

    class SendICMP {

    public:





    private:



    private:
        /**
         * 生成校验和
         * @param icmp_packet 数据指针
         * @param size 数据长度
         * @return 小端字节序的16位校验和
         */
        unsigned short icmp_calc_checksum(char* icmp_packet, int size);

        /**
         * 将小端字节序转换为大端字节序
         * @param da 数据
         * @return 大端字节序
         */
        unsigned short  littleNet2BigNet(unsigned short da);

    };

} // ICMP

#endif //MYICMP_SENDICMP_H
