#include<iostream>
#include <string>
#include "curl/curl.h"

using namespace std;

//处理返回包的回调函数 声明
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {

    string &strRes = *(string *) userdata;
    strRes += ptr;
    return size * nmemb;
}


int main() {
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl = curl_easy_init();//创建curl句柄

    //设置url

    CURLcode nRet = nRet = curl_easy_setopt(curl, CURLOPT_URL,"https://tool.chinaz.com/");
    if (nRet == CURLE_OK) {
        printf("curl_easy_setopt -url --ok");
    } else {
        printf("curl_easy_setopt -url --CURLE_OUT_OF_MEMORY");
    }

    //下面这些数据是抓包抓到的
    struct curl_slist *slist = NULL;
    slist = curl_slist_append(slist, "host: tool.chinaz.com");
    slist = curl_slist_append(slist, "cache-control: max-age=0");
    slist = curl_slist_append(slist,"sec-ch-ua: \"Not A(Brand\";v=\"99\", \"Microsoft Edge\";v=\"121\", \"Chromium\";v=\"121\"");
    slist = curl_slist_append(slist, "sec-ch-ua-mobile: ?0");
    slist = curl_slist_append(slist, "sec-ch-ua-platform: \"Windows\"");
    slist = curl_slist_append(slist, "upgrade-insecure-requests: 1");
    slist = curl_slist_append(slist,"user-agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/121.0.0.0 Safari/537.36 Edg/121.0.0.0)");
    slist = curl_slist_append(slist,"accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7)");
    slist = curl_slist_append(slist, "sec-fetch-site: none");
    slist = curl_slist_append(slist, "sec-fetch-mode: navigate");
    slist = curl_slist_append(slist, "sec-fetch-user: ?1");
    slist = curl_slist_append(slist, "sec-fetch-dest: document");
    slist = curl_slist_append(slist, "accept-encoding: gzip, deflate, br");
    slist = curl_slist_append(slist, "accept-language: zh,zh-CN;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6");
    //slist = curl_slist_append(slist,"cookie: __bid_n=185bf21922dc91bcd24207; FPTOKEN=1H/2Tc+AI8f2Bp6f+qky7zMHESmYSOTr04bc4b4qcNNA1eaXoiXlMziX8IUeFTxJ6GZxXJBw37DLD1/nTK5b9KLNS0REA4F2D9sxTDeoNhQ/uqzHOrPlU+KvVlIiQtGA3RuiZSO2PoFiZs2iVh80R7oe806vVzZGzQFyfgKgqTyE10jw2Rd/WkpXllcMrKbo8XQmVFisG7DjiT0V2R43iGp6/epKdY9cClrrUNeeRmKpj3qU1GDrlg/eJExTIpGRpEcgEfC052xQHeg4+L1zq9lCiS5IVFDEGjVRvztzbdwzOzdpg9mdTxh2K0Sh77cORrhu59bNWAae6SbmWqVNVcaH/V2lFYVOo268353Oy7G0c6q3K7MPA8MEvv5xfjp8b3OgMpPs59cwH1l/Qnn5GQ==|0dRkjHu2aQE9LPE9mljkTBUxlgFbn7GMTDBRG7Wk/wA=|10|0599eb33ea448299ada650a5c1ba2d85; cz_statistics_visitor=df7882b2-6175-b5ea-aee4-039f2ab059e2; qHistory=aHR0cDovL2lwLnRvb2wuY2hpbmF6LmNvbV9JUC9JUHY25p+l6K+i77yM5pyN5Yqh5Zmo5Zyw5Z2A5p+l6K+ifGh0dHA6Ly90b29sLmNoaW5hei5jb21f56uZ6ZW/5bel5YW3; Hm_lvt_aecc9715b0f5d5f7f34fba48a3c511d6=1706854526; Hm_lvt_ca96c3507ee04e182fb6d097cb2a1a4c=1706798426,1708267757; _clck=tiuboa%7C2%7Cfjd%7C0%7C1492; Hm_lpvt_ca96c3507ee04e182fb6d097cb2a1a4c=1708267770; _clsk=17opgqz%7C1708267831332%7C2%7C0%7Cw.clarity.ms%2Fcollect");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);//设置包头


    //设置读取数据的回调函数
    std::string strRes;//将读取到的数据存到string中
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &strRes);//传入参数

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);//忽略证书检查
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    //为了让抓包工具可以抓到自己程序发送的包，那么需要设置代理
    //设置代理类型
    curl_easy_setopt(curl, CURLOPT_PROXYTYPE, CURLPROXY_HTTP);
    //设置代理
    curl_easy_setopt(curl, CURLOPT_PROXY, "127.0.0.1:8866");



    //自动匹配压缩格式解压缩
    curl_easy_setopt(curl,CURLOPT_ACCEPT_ENCODING,"");
    //发送请求
    auto retCode = curl_easy_perform(curl);//收到返回包才会返回

    curl_easy_cleanup(curl);//关闭curl句柄
    curl_global_cleanup();

    return 0;
}