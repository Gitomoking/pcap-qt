#include "pcap_application.h"

#include <iostream>

using namespace std;

#define DNS_TYPE_A 1
#define DNS_TYPE_AAAA 28
#define DNS_TYPE_CNAME 5
#define DNS_TYPE_HINFO 13
#define DNS_TYPE_MB 7
#define DNS_TYPE_MD 3
#define DNS_TYPE_MF 4
#define DNS_TYPE_MG 8
#define DNS_TYPE_MINFO 14
#define DNS_TYPE_MINFO_ 121
#define DNS_TYPE_MR 9
#define DNS_TYPE_MX 15
#define DNS_TYPE_MX_ 123
#define DNS_TYPE_NS 2
#define DNS_TYPE_NULL 10
#define DNS_TYPE_PTR 12
#define DNS_TYPE_PTR_ 120
#define DNS_TYPE_SOA 6
#define DNS_TYPE_TXT 16
#define DNS_TYPE_WKS 11
#define DNS_CLASS_IN 1
#define DNS_CLASS_CH 3

/* コンストラクタ :メンバ変数にデータ格納 */
DNSPacket::DNSPacket (const u_char *raw) {
	/* 変数宣言
	 * dnshdr :DNSヘッダセクション */
	struct dnshdr *dnshdr;
	dnshdr = (struct dnshdr *)raw;
	this->id = ntohs (dnshdr->id); /* 識別子を取得 */
	this->qr = (dnshdr->qr) ? true : false; /* QRを取得 */
	this->opcode = dnshdr->opcode; /* Opcodeを取得 */
	this->aa = (dnshdr->aa) ? true : false; /* AAを取得 */
	this->tc = (dnshdr->tc) ? true : false; /* TCを取得 */
	this->rd = (dnshdr->rd) ? true : false; /* RDを取得 */
	this->ra = (dnshdr->ra) ? true : false; /* RAを取得 */
	this->rcode = dnshdr->rcode; /* Rcodeを取得 */
	this->qdcount = ntohs (dnshdr->qdcount); /* 問い合わせ数を取得 */
	this->ancount = ntohs (dnshdr->ancount); /* 回答数を取得 */
	this->nscount = ntohs (dnshdr->nscount); /* オーソリティ数を取得 */
	this->arcount = ntohs (dnshdr->arcount); /* 追加情報数を取得 */
	this->pdetail = "";
	this->pdetail = this->getPacketDetail(); /* パケット備考を取得 */
	u_int16_t now = 12; /* 現在のポインタ */
	/* 質問セクション */
	for (int i = 0; i < this->qdcount; i++) {
		this->section += " > 質問　　　　 :";
		this->section += this->getQDSection (raw, &now);
	}
	/* 回答セクション */
	for (int i = 0; i < this->ancount; i++) {
		this->section += " > 回答　　　　 :";
		this->section += this->getResourceRecode (raw, &now);
	}
	/* オーソリティセクション */
	for (int i = 0; i < this->nscount; i++) {
		this->section += " > オーソリティ :";
		this->section += this->getResourceRecode (raw, &now);
	}
	/* 追加情報セクション */
	for (int i = 0; i < this->arcount; i++) {
		this->section += " > 追加情報　　 :";
		this->section += this->getResourceRecode (raw, &now);
	}
}

/* getPacketDetail()関数 :パケットの備考を返す関数 */
string DNSPacket::getPacketDetail (void) {
	stringstream ss;
	ss.str("");
	if (this->pdetail.empty()) {
		/* コンストラクタで呼び出した場合，基本情報を格納する */
		ss << "id=" << dec << this->id << ":";
		if (this->qr == 1) { /* DNSメッセージの種類 */
			ss << "[応答] ";
		}else {
			ss << "[問い合わせ(";
			switch (this->opcode) {
				case 0 : ss << "QUERY"; break;
				case 1 : ss << "IQUERY"; break;
				case 2 : ss << "STATUS"; break;
				default : ss << this->opcode; break;
			}
			ss << ")] ";
		}
		/* DNSメッセージのオプション */
		if (this->aa) ss << "オーソリティ ";
		if (this->tc) ss << "メッセージ短縮 ";
		if (this->rd) ss << "再帰希望 ";
		if (this->ra) ss << "再帰可能 ";
		if (this->qdcount) ss << "質問(" << this->qdcount << ") ";
		if (this->ancount) ss << "回答(" << this->ancount << ") ";
		if (this->nscount) ss << "オーソリティ(" << this->nscount << ") ";
		if (this->arcount) ss << "追加情報(" << this->arcount << ") ";
		return ss.str();
	}else {
		/* 外部からの呼び出しはパケット備考を返す */
		return this->pdetail;
	}
}

/* getApplicationDetail()関数 :アプリケーション層詳細を返す関数 */
string DNSPacket::getApplicationDetail (void) {
	stringstream ss;
	ss.str("");
	ss << dec;
	ss << "[DNS] ";
	ss << "id=" << dec << this->id << ":";
	if (this->qr == 1) { /* DNSメッセージの種類 */
		ss << "[応答] ";
	}else {
		ss << "[問い合わせ(";
		switch (this->opcode) {
			case 0 : ss << "QUERY"; break;
			case 1 : ss << "IQUERY"; break;
			case 2 : ss << "STATUS"; break;
			default : ss << this->opcode; break;
		}
		ss << ")] ";
	}
	/* DNSメッセージのオプション */
	if (this->aa) ss << "オーソリティ ";
	if (this->tc) ss << "メッセージ短縮 ";
	if (this->rd) ss << "再帰希望 ";
	if (this->ra) ss << "再帰可能 ";
	ss << "\n";
	ss << " > Rcode :" << this->rcode << "(";
	switch (this->rcode) {
		case 0 : ss << "エラーなし"; break;
		case 1 : ss << "エラー：フォーマットエラー"; break;
              case 2 : ss << "エラー：サーバの障害"; break;
              case 3 : ss << "エラー：名前エラー"; break;
              case 4 : ss << "エラー：未実装"; break;
              case 5 : ss << "エラー：拒絶された"; break;
              default : ss << "使用されない"; break;
	}
	ss << ")\n";
	ss << " > 問い合わせ数 :" << this->qdcount << ", ";
	ss << "回答数 :" << this->ancount << ", ";
	ss << "オーソリティ数 :" << this->nscount << ", ";
	ss << "追加情報数 :" << this->arcount << "\n";
	ss << this->section;
	return ss.str();
}

/* QDSection()関数
 * 概要 :質問セクションを返す関数
 * raw :質問セクションの開始ポインタ */
string DNSPacket::getQDSection (const u_char *raw, u_int16_t *startptr) {
	u_int16_t now = *startptr; /* 現在の位置 */
	stringstream ss;
	ss.str("");
	ss << this->getDomainName (raw, &now) << ", "; /* 名前の取得 */
	struct dns_query *query = (struct dns_query *)(raw + now);
	/* QType */
	ss << "QType = 0x" << setfill('0') << setw(4)
	   << hex << ntohs (query->qtype) << "(";
	switch (ntohs (query->qtype)) {
		case DNS_TYPE_A : ss << "A"; break;
		case DNS_TYPE_AAAA : ss << "AAAA"; break;
		case DNS_TYPE_CNAME : ss << "CNAME"; break;
		case DNS_TYPE_HINFO : ss << "HINFO"; break;
		case DNS_TYPE_MB : ss << "MB"; break;
		case DNS_TYPE_MD : ss << "MD"; break;
		case DNS_TYPE_MF : ss << "MF"; break;
		case DNS_TYPE_MG : ss << "MG"; break;
		case DNS_TYPE_MINFO :
		case DNS_TYPE_MINFO_ : ss << "MINFO"; break;
		case DNS_TYPE_MR : ss << "MR"; break;
		case DNS_TYPE_MX :
		case DNS_TYPE_MX_ : ss << "MX"; break;
		case DNS_TYPE_NS : ss << "NS"; break;
		case DNS_TYPE_NULL : ss << "NULL"; break;
		case DNS_TYPE_PTR :
		case DNS_TYPE_PTR_ : ss << "PTR"; break;
		case DNS_TYPE_SOA : ss << "SOA"; break;
		case DNS_TYPE_TXT : ss << "TXT"; break;
		case DNS_TYPE_WKS : ss << "WKS"; break;
	}
	ss << "), ";
	/* QClass */
	ss << "QClass = 0x" << setfill('0') << setw(4)
	<< hex << ntohs (query->qclass) << "(";
	switch (ntohs (query->qclass)) {
		case DNS_CLASS_IN :
			ss << "IN"; break;
		case DNS_CLASS_CH :
			ss << "CH"; break;
	}
	ss << ")\n";
	now += 4; /* QTypeとQClass分 4 */
	*startptr = now;
	return ss.str();
}

/* ResourceRecode()関数
 * 概要 :リソースレコード（回答，オーソリティ，追加情報セクション）を返す関数
 * raw :リソースレコードの開始ポインタ */
string DNSPacket::getResourceRecode (const u_char *raw, u_int16_t *startptr) {
	u_int16_t now = *startptr; /* 現在の位置 */
	stringstream ss;
	ss.str("");
	ss << this->getDomainName (raw, &now) << ", "; /* 名前の取得 */
	struct dns_resource *resource = (struct dns_resource *)(raw + now);
	/* QType */
	ss << "QType = 0x" << setfill('0') << setw(4)
	   << hex << ntohs (resource->qtype) << "(";
	switch (ntohs (resource->qtype)) {
		case DNS_TYPE_A : ss << "A"; break;
		case DNS_TYPE_AAAA : ss << "AAAA"; break;
		case DNS_TYPE_CNAME : ss << "CNAME"; break;
		case DNS_TYPE_HINFO : ss << "HINFO"; break;
		case DNS_TYPE_MB : ss << "MB"; break;
		case DNS_TYPE_MD : ss << "MD"; break;
		case DNS_TYPE_MF : ss << "MF"; break;
		case DNS_TYPE_MG : ss << "MG"; break;
		case DNS_TYPE_MINFO :
		case DNS_TYPE_MINFO_ : ss << "MINFO"; break;
		case DNS_TYPE_MR : ss << "MR"; break;
		case DNS_TYPE_MX :
		case DNS_TYPE_MX_ : ss << "MX"; break;
		case DNS_TYPE_NS : ss << "NS"; break;
		case DNS_TYPE_NULL : ss << "NULL"; break;
		case DNS_TYPE_PTR :
		case DNS_TYPE_PTR_ : ss << "PTR"; break;
		case DNS_TYPE_SOA : ss << "SOA"; break;
		case DNS_TYPE_TXT : ss << "TXT"; break;
		case DNS_TYPE_WKS : ss << "WKS"; break;
	}
	ss << "), ";
	/* QClass */
	ss << "QClass = 0x" << setfill('0') << setw(4)
	<< hex << ntohs (resource->qclass) << "(";
	switch (ntohs (resource->qclass)) {
		case DNS_CLASS_IN :
			ss << "IN"; break;
		case DNS_CLASS_CH :
			ss << "CH"; break;
	}
	ss << ")\n";
	/* RData */
	ss << "\t　　　　　　  RData : ";
	now += 10; /* QType(2), QClass(2), TTL(4), RDLength(2)分 */
	u_int16_t rdlen = ntohs (resource->rdlen);
	switch (ntohs (resource->qtype)) {
		case DNS_TYPE_A : /* Aタイプ : ホストの32ビットIPアドレス */
		{
			/* 複数のIPアドレスを持つ場合がある */
			for (int i = 0; i < (rdlen/4); i++) {
				struct in_addr *ipaddr; /* IPアドレス */
				ipaddr = (struct in_addr *)(raw + now);
				if (i) ss << ", ";
				ss << inet_ntoa (*ipaddr); /* ドット付き10進表記にして格納 */
				now += 4; /* 次ポインタ */
			}
		}break;
		case DNS_TYPE_AAAA : /* AAAAタイプ : ホストのIPv6アドレス */
/* 途中！ */		{
			struct in6_addr *ip6addr; /* iPv6アドレス */
			ip6addr = (struct in6_addr *)(raw + now);

		}break;
		case DNS_TYPE_CNAME : /* CNAMEタイプ : ホストの標準名 */
		{
			ss << this->getDomainName (raw, &now);
		}break;
		case DNS_TYPE_HINFO : /* HINFOタイプ : ホストに関する追加の情報, CPU と OS のタイプ*/
		{
			for (int i = 0; i < rdlen; i++) {
				if (raw[now]) {
					ss << raw[now]; /* CPU / OSタイプを格納 */
					now++;
				}else {
					if (i != rdlen - 1)
						ss << ", "; /* 文字列の終了 */
					now++;
				}
			}
		}break;
		case DNS_TYPE_MINFO :
		case DNS_TYPE_MINFO_ : /* MINFOタイプ : メーリングリストを担当するメールボックス */
		{
			/* 名前を取得 */
			ss << this->getDomainName (raw, &now) << ", ";
			ss << this->getDomainName (raw, &now);
		}break;
		case DNS_TYPE_MR : ss << "MR"; break;
		case DNS_TYPE_MX :
		case DNS_TYPE_MX_ : /* MXタイプ : そのドメインのメール交換局 */
		{
			u_int16_t *priority; /* 16ビットの優先度 */
			priority = (u_int16_t *)(raw + now);
			ss << "優先度(" << *priority << ") ";
			now += 3;
			ss << this->getDomainName (raw, &now); /* 名前の取得 */
		}break;
		case DNS_TYPE_NS : /* NSタイプ : オーソリティのホストの名前 */
		{
			ss << this->getDomainName (raw, &now);
		}break;
		case DNS_TYPE_PTR :
		case DNS_TYPE_PTR_ : /* PTRタイプ : IP アドレスの逆引き用のポインタ */
		{
			ss << this->getDomainName (raw, &now);
		}break;
		case DNS_TYPE_SOA : /* SOAタイプ : オーソリティゾーンの開始マーク */
		{
			ss << "MName " << this->getDomainName (raw, &now) << ", ";
			ss << "RName " << this->getDomainName (raw, &now) << "\n";
			struct dns_soa *soa = (dns_soa *)(raw + now);
			ss << "\t　　　　　　  連続番号 " << soa->serial << ", ";
			ss << "更新 " << soa->refresh << ", ";
			ss << "再試行 " << soa->retry << ", ";
			ss << "終了 " << soa->expire << ", ";
			ss << "最小 " << soa->mini;
			now += 20;
		}break;
		case DNS_TYPE_TXT : /* TXTタイプ : テキスト文字列 */
		{
			for (int i = 0; i < rdlen; i++) {
				ss << raw[now]; /* 文字列を取得 */
				now++;
			}
		}break;
		case DNS_TYPE_WKS : /* WKSタイプ : ウェルノウンサービス記述 */
		{
			struct in_addr *ipaddr;
			u_char proto;
			ipaddr = (struct in_addr *)(raw + now);
			ss << inet_ntoa (*ipaddr) << ", "; /* ドット付き10進表記にして格納 */
			now += 4;
			proto = raw[now];
			ss << dec << (u_int16_t)proto; /* プロトコル番号を格納 */
			now++;
			/* 可変長のビットマップを格納 */
			for (int i = 5; i < rdlen; i++) {
				ss << ", " << raw[now];
				now++;
			}
		}break;
	}
	ss << "\n";
	*startptr = now; /* 位置ポインタの更新 */
	return ss.str();
}

/* getDomainName()関数：ドメイン名を取得する関数 */
string DNSPacket::getDomainName (const u_char *raw, u_int16_t *startptr) {
	stringstream ss;
	ss.str("");
	u_int16_t now = *startptr;
	u_int16_t temp = 0;
	/* 名前の取得 */
	do {
		u_int16_t len = (u_int16_t)raw[now]; /* ラベルの長さ */
		if (len > 64) { /* 先頭2ビットが1のとき，すなわち，メッセージを圧縮している場合 */
			if (!temp) temp = now + 2; /* 次の位置を保存 */
			u_int16_t offset = ((u_int16_t)raw[now] - 192) * 256;
			offset += (u_int16_t)raw[now + 1];
			now = offset; /* DNSメッセージの先頭からの位置 */
			len = raw[now]; /* ラベルの長さ */
		}
		u_int16_t target = now + len;
		while (now < target) {
			now++; /* 現在のポインタ位置をずらす */
			ss << raw[now]; /* ラベル名 */
		}
		now++;
		if (raw[now]) ss << ".";
	/* \0が来てなければ続ける（ドメイン名の最後まで続ける） */
	}while (raw[now]);
	if (temp) now = temp;
	else now++;
	*startptr = now;
	this->pdetail = this->pdetail + ss.str() + " "; /* パケット備考を更新する */
	return ss.str();
}
