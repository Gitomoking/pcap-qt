#ifndef INCLUDED_PCAP_DATALINK
#define INCLUDED_PCAP_DATALINK

#ifndef INCLUDED_PCAP_PCAP
#include "pcap_pcap.h"
#endif

#define ETHER_IP 0x0800
#define ETHER_ARP 0x0806

/* 抽象クラス */
class Datalink {
public:
	std::string getDatalinkDetail (void); /* データリンク層詳細 */
};

class IEEE802_3 : public Datalink {
	std::string shost;	/* 送信元MACアドレス */
	std::string dhost;	/* 宛先MACアドレス */
	u_int16_t type;	/* イーサタイプ */
public:
	IEEE802_3 (const u_char *raw); /* コンストラクタ */
	u_int16_t getEtherType (void); /* イーサタイプを返す */
	std::string getDatalinkDetail (void); /* データリンク層詳細 */
};

#endif
