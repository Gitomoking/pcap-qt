#ifndef INCLUDED_PCAP_APPLICATION
#define INCLUDED_PCAP_APPLICATION

#include "pcap_pcap.h"


class Application {
public:
	std::string getPacketDetail (void); /* 備考を返す */
	std::string getApplicationDetail (void); /* アプリケーション層詳細を返す */
};

class DNSPacket : public Application {
	u_int16_t id;      	/* 識別 */
	bool qr;		/* QR */
	u_int16_t opcode;	/* Opcode */
	bool aa;		/* AA */
	bool tc;		/* TC */
	bool rd;		/* RD */
	bool ra;		/* RA */
	u_int16_t rcode;	/* RCode */
	u_int16_t qdcount; 	/* 質問セクションの項目数 */
	u_int16_t ancount; 	/* 回答セクションの項目数 */
	u_int16_t nscount; 	/* オーソリティセクションの項目数 */
	u_int16_t arcount; 	/* 追加情報セクションの項目数 */
	std::string section;	/* 各セクションの情報 */
	std::string pdetail;	/* パケット備考 */
public:
	DNSPacket (const u_char *raw); /* コンストラクタ */
	std::string getPacketDetail (void); /* 備考を返す */
	std::string getApplicationDetail (void); /* アプリケーション層詳細を返す */
	std::string getQDSection (const u_char *raw, u_int16_t *startptr); /* 質問セクションを取得 */
	std::string getResourceRecode (const u_char *raw, u_int16_t *startptr); /* リソースレコードを取得 */
	std::string getDomainName (const u_char *raw, u_int16_t *startptr); /* 名前を取得 */
};

class HTTPPacket : public Application {
	std::string pdetail;	/* パケット備考 */
	std::string adetail; /* アプリケーション層詳細*/
public:
	HTTPPacket (const u_char *raw); /* コンストラクタ */
	std::string getPacketDetail (void); /* 備考を返す */
	std::string getApplicationDetail (void); /* アプリケーション層詳細を返す */
};

/* DNSヘッダ */
struct dnshdr {
       u_int16_t id;      /* 識別 */
# if __BYTE_ORDER == __LITTLE_ENDIAN
       u_int32_t ra : 1;        /* 1:再帰サービスをサーバが提供可 */
       u_int32_t res : 3;       /* 未使用 */
       u_int32_t rcode : 4;     /* 回答の種類 */
       u_int32_t qr : 1;        /* 1:応答，0:問い合わせ */
       u_int32_t opcode : 4;    /* 問い合わせのタイプ */
       /* opcodeの値
       * 0:標準問い合わせ-QUERY 1:逆問い合わせ-IQUERY
       * 2:サーバ状態要求-STATUS 3-15:将来使用 */
       u_int32_t aa : 1;        /* 1:オーソリティ */
       u_int32_t tc : 1;        /* 1:メッセージ短縮 */
       u_int32_t rd : 1;        /* 1:再帰問い合わせ */
#  elif __BYTE_ORDER == __BIG_ENDIAN
       u_int32_t qr : 1;        /* 1:応答，0:問い合わせ */
       u_int32_t opcode : 4;    /* 問い合わせのタイプ */
       u_int32_t aa : 1;        /* 1:オーソリティ */
       u_int32_t tc : 1;        /* 1:メッセージ短縮 */
       u_int32_t rd : 1;        /* 1:再帰問い合わせ */
       u_int32_t ra : 1;        /* 1:再帰サービスをサーバが提供可 */
       u_int32_t res : 3;       /* 未使用 */
       u_int32_t rcode : 4;     /* 回答の種類 */
       /* rcodeの値
        * 0:エラーなし 1:フォーマットエラー
        * 2:サーバの障害 3:名前エラー
        * 4:未実装（サポートされていない）
        * 5:拒絶 6-15:将来使用*/
#  else
#   error "Adjust your <bits/endian.h> defines"
#  endif
       u_int16_t qdcount; /* 質問セクションの項目数 */
       u_int16_t ancount; /* 回答セクションの項目数 */
       u_int16_t nscount; /* オーソリティセクションの項目数 */
       u_int16_t arcount; /* 追加情報セクションの項目数 */
};

struct dns_query {
       u_int16_t qtype;   /* QType */
       u_int16_t qclass;  /* QClass */
};

struct dns_resource {
       u_int16_t qtype;	/* QType */
       u_int16_t qclass;	/* QClass */
       u_int32_t ttl;	/* TTL */
       u_int16_t rdlen;	/* RDLength */
};

struct dns_soa {
	u_int32_t serial;	/* 連続番号 */
	u_int32_t refresh;	/* 更新 */
	u_int32_t retry;	/* 再試行 */
	u_int32_t expire;	/* 終了 */
	u_int32_t mini;	/* 最小 */
};

#endif
