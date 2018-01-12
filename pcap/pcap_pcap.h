#ifndef INCLUDED_PCAP_PCAP
#define INCLUDED_PCAP_PCAP
#include <iomanip>
#include <string>
#include <sstream>
#include <cctype>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

/* 疑似ヘッダ
 * pcap_packet.cpp pcap_network.h pcap_transport.hで使用 */
struct pseudohdr {
       u_char saddr[4];     /* 送信元アドレス */
       u_char daddr[4];     /* 宛先アドレス */
       u_int8_t proto;      /* プロトコル番号 6 or 17 */
       u_int16_t len;       /* TCP/UDPパケット長 */
};

#endif
