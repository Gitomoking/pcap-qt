#ifndef INCLUDED_PCAP_TRANSPORT
#define INCLUDED_PCAP_TRANSPORT

#include "pcap_pcap.h"

#define TCP_FTP_DATA 20
#define TCP_FTP 21
#define TCP_SSH 22
#define TCP_TELNET 23
#define TCP_SMTP 25
#define TCP_DNS 53
#define TCP_HTTP 80
#define TCP_HTTPS 443
#define UDP_DNS 53
#define UDP_DHCP_SERVER 67
#define UDP_DHCP_CLIENT 68
#define UDP_NTP 123
#define UDP_HTTPS 443

/* トランスポート層の抽象クラス */
class Transport {
public:
	std::string getPacketDetail (void); /* 備考を返す */
	std::string getTransportDetail (void); /* トランスポート層詳細を返す */
	bool checkSum (void); /* チェックサムを計算する */
};

class TCPPacket : public Transport {
	u_int16_t sport;	/* 送信元ポート番号 */
	u_int16_t dport;	/* 宛先ポート番号 */
	u_int32_t seq;	/* シーケンス番号 */
	u_int32_t ack_seq;	/* ACKシーケンス番号 */
	u_int16_t len;	/* セグメント長 */
	bool urg;		/* URGフラグ */
	bool ack;		/* ACKフラグ */
	bool psh;		/* PSHフラグ */
	bool rst;		/* RSTフラグ */
	bool syn;		/* SYNフラグ */
	bool fin;		/* FINフラグ */
	u_int16_t window;	/* ウィンドウサイズ */
	u_int16_t check;	/* チェックサム */
	u_int16_t urg_ptr;	/* 緊急ポインタ */
	std::string opts;	/* オプション */
	struct pseudohdr *pseudo;	/* 疑似ヘッダ */
	u_int16_t *checkhdr;	/* チェックサム用ヘッダ */
public:
	/* コンストラクタ */
	TCPPacket (const u_char *raw, struct pseudohdr *pseudohdr);
	u_int16_t getHeaderLen (void); /* セグメント長を返す */
	u_int16_t getSPort (void); /* 送信元ポート番号を返す */
	u_int16_t getDPort (void); /* 宛先ポート番号を返す */
	std::string getPacketDetail (void); /* 備考を返す */
	std::string getTransportDetail (void); /* トランスポート層詳細を返す */
	bool checkSum (void); /* チェックサムを計算する */
};

class UDPPacket : public Transport {
	u_int16_t sport;	/* 送信元ポート番号 */
	u_int16_t dport;	/* 宛先ポート番号 */
	u_int16_t len;	/* データグラム長 */
	u_int16_t check;	/* チェックサム */
	struct pseudohdr *pseudo;	/* 疑似ヘッダ */
	u_int16_t *checkhdr;	/* チェックサム用ヘッダ */
public:
	UDPPacket (const u_char *raw, struct pseudohdr *pseudohdr);
	u_int16_t getHeaderLen (void); /* データグラム長を返す */
	u_int16_t getSPort (void); /* 送信元ポート番号を返す */
	u_int16_t getDPort (void); /* 宛先ポート番号を返す */
	std::string getPacketDetail (void); /* 備考を返す */
	std::string getTransportDetail (void); /* トランスポート層詳細を返す */
	bool checkSum (void); /* チェックサムを計算する */
};

class ICMPPacket : public Transport {
	u_int8_t type;	/* ICMPタイプ */
	u_int8_t code;	/* コード */
	u_int16_t check;	/* チェックサム */
public:
	ICMPPacket (const u_char *raw); /* コンストラクタ */
	std::string getPacketDetail (const u_char *raw); /* 備考を返す */
	std::string getTransportDetail (const u_char *raw); /* トランスポート層詳細を返す */
	bool checkSum (void); /* チェックサムを計算する */
};

/* TCPオプション */
struct tcpopts {
       u_int8_t type;       /* オプションタイプ */
       u_int8_t len;        /* オプション長 */
       u_int16_t value;     /* オプション値 */
};

/* ICMPヘッダ */
struct basic_icmp {
       u_int8_t type;       /* ICMPタイプ8ビット */
       u_int8_t code;       /* コード8ビット */
       u_int16_t check;     /* チェックサム16ビット */
	u_int32_t data;	/* ICMPデータ */
};

/* ICMPタイムスタンプデータ */
struct icmp_timestamp_data {
	u_int16_t id;		/* 識別子 */
	u_int16_t seq;	/* シーケンス番号 */
	u_int32_t startts;	/* 開始タイムスタンプ */
	u_int32_t receivets;	/* 受信タイムスタンプ */
	u_int32_t sendts;	/* 送出タイムスタンプ */
};

struct icmp_addressmask_data {
	u_int16_t id;		/* 識別子 */
	u_int16_t seq;	/* シーケンス番号 */
	struct in_addr mask;	/* サブネットマスク */
};

#endif
