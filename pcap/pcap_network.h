#ifndef INCLUDED_PCAP_NETWORK
#define INCLUDED_PCAP_NETWORK

#include "pcap_pcap.h"
#include "pcap_datalink.h"

#define IP_ICMP 1
#define IP_TCP 6
#define IP_UDP 17
#define ARP_ETHER 1
#define ARP_IEEE802_ 6
#define ARP_REQUEST 1
#define ARP_REPLY 2

/* 抽象クラス */
class Network {
public:
	std::string getSAddr (void); /* 送信元アドレスを返す */
	std::string getDAddr (void); /* 宛先アドレスを返す */
	std::string getNetworkDetail (void); /* ネットワーク層詳細を返す */
};

class IPPacket : public Network {
    	u_int8_t hlen;			/* ヘッダ長 */
    	u_int8_t ver;				/* バージョン */
    	u_int8_t tos;				/* Type of Service */
    	u_short len;				/* IPパケット長 */
    	u_short id;				/* 識別子 */
    	u_short off;				/* フラグメントオフセット */
    	u_int8_t ttl;				/* Time to Live */
    	u_int8_t proto;			/* プロトコル */
    	u_short sum;				/* チェックサム */
	u_int16_t *checkhdr;			/* チェックサム用データ */
    	struct in_addr saddr, daddr;	/* IPアドレス */
	struct pseudohdr *pseudohdr;		/* 疑似ヘッダ */
public:
	IPPacket (const u_char *raw); /* コンストラクタ */
	std::string getSAddr (void); /* 送信元アドレスを返す */
	std::string getDAddr (void); /* 宛先アドレスを返す */
	std::string getNetworkDetail (void); /* ネットワーク層詳細を返す */
	u_int32_t getIPHeaderLen (void); /* ヘッダ長を返す*/
	u_int8_t getProtocol (void); /* プロトコル番号を返す */
	bool checkSum (void); /* チェックサムを計算する */
	struct pseudohdr *getPseudoHeader (void);	/* 疑似ヘッダを返す */
};

class ARPPacket : public Network {
	u_int16_t htype;	/* ハードウェアタイプ */
	u_int16_t ptype;	/* 解決の要求元プロトコル */
	u_int16_t opec;	/* オペレーションコード */
	std::string sha;	/* 送信元のハードウェアアドレス */
	std::string spa;	/* 送信元のIPアドレス */
 	std::string dha;	/* 宛先のハードウェアアドレス */
	std::string dpa;	/* 宛先のIPアドレス */
public:
	ARPPacket (const u_char *raw); /* コンストラクタ */
	std::string getSAddr (void); /* 送信元アドレスを返す */
	std::string getDAddr (void); /* 宛先アドレスを返す */
	std::string getNetworkDetail (void); /* ネットワーク層詳細を返す */
	std::string getPacketDetail(void); /* 備考を返す */
};



/* ARPヘッダ */
struct arp {
       u_int16_t htype;	/* ハードウェアタイプ */
       u_int16_t ptype;	/* 解決の要求元プロトコル */
       u_int8_t hlen;	/* ハードウェアアドレス長 */
       u_int8_t plen;	/* プロトコルアドレス長 */
       u_int16_t opec;	/* オペレーションコード */
       u_int8_t sha[6];	/* 送信元のハードウェアアドレス */
       u_int8_t spa[4];	/* 送信元のIPアドレス */
       u_int8_t dha[6];	/* 宛先のハードウェアアドレス */
       u_int8_t dpa[4];	/* 宛先のIPアドレス */
};

#endif
