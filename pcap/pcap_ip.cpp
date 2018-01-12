#include "pcap_network.h"
#include <netinet/ip.h>

using namespace std;

/* コンストラクタ :メンバ変数にデータを格納する */
IPPacket::IPPacket (const u_char *raw) {
	/* 変数宣言
	 * iphdr :IPヘッダ <netinet/ip.h> */
	struct ip *iphdr = (struct ip *)raw;
	this->hlen = 4 * iphdr->ip_hl; /* ヘッダ長を取得 */
	this->ver = iphdr->ip_v; /* バージョンを取得 */
	this->tos = iphdr->ip_tos; /* TOS（Type of Service）ぽ取得 */
	this->len = ntohs (iphdr->ip_len); /* IPパケット長を取得 */
	this->id = ntohs (iphdr->ip_id); /* パケットIDを取得 */
	this->off = ntohs (iphdr->ip_off); /* フラグメントオフセットを取得 */
	this->ttl = iphdr->ip_ttl; /* TTLを取得 */
	this->proto = iphdr->ip_p; /* プロトコルを取得 */
	this->sum = ntohs (iphdr->ip_sum); /* チェックサムを取得 */
	this->saddr = iphdr->ip_src; /* 送信元IPアドレスを取得 */
	this->daddr = iphdr->ip_dst; /* 宛先IPアドレスを取得 */
	this->checkhdr = (u_int16_t *)raw; /* チェックサムを計算 */
	/* 疑似ヘッダのアドレス部分とパケット長を取得 */
	this->pseudohdr = new struct pseudohdr;
	if (__BYTE_ORDER == __LITTLE_ENDIAN) {
		/* ビッグエンディアンの場合 */
		for (int i = 0; i < 8; i++) {
			if (i < 4) {
				/* 疑似ヘッダの送信元アドレスフィールド */
				this->pseudohdr->saddr[i] = raw[12 + i];
			}else {
				/* 疑似ヘッダの宛先アドレスフィールド */
				this->pseudohdr->daddr[i - 4] = raw[12 + i];
			}
		}
	}else if (__BYTE_ORDER == __BIG_ENDIAN) {
		/* リトルエンディアンの場合 */
		/* 疑似ヘッダの送信元アドレスフィールド */
		this->pseudohdr->saddr[0] = raw[13];
		this->pseudohdr->saddr[1] = raw[12];
		this->pseudohdr->saddr[2] = raw[15];
		this->pseudohdr->saddr[3] = raw[14];
		/* 疑似ヘッダの宛先アドレスフィールド */
		this->pseudohdr->daddr[0] = raw[17];
		this->pseudohdr->daddr[1] = raw[16];
		this->pseudohdr->daddr[2] = raw[19];
		this->pseudohdr->daddr[3] = raw[18];
	}
	/* 疑似ヘッダのプロトコルフィールド初期化 */
	this->pseudohdr->proto = 0;
	/* 疑似ヘッダのパケット長フィールド */
	this->pseudohdr->len = 0;
	/* IPオプション */
	u_int8_t len = this->hlen - 20;
	while (len > 0) {
		len--;
	}
}

/* getSAddr()関数 :送信元IPアドレスを返す関数 */
string IPPacket::getSAddr (void) {
	/* char *inet_ntoa(struct in_addr)関数
	 * IPアドレスをドット付き表記に変換する関数
	 * <sys/socket.h> <netinet/in.h> <arpa/inet.h> */
	string addr = inet_ntoa (this->saddr);
	return addr;
}

/* getDAddr()関数 :宛先IPアドレスを返す関数 */
string IPPacket::getDAddr (void) {
	string addr = inet_ntoa (this->daddr);
	return addr;
}

/* getIPHeaderLen()関数 :ヘッダ長を返す関数 */
u_int32_t IPPacket::getIPHeaderLen (void) {
	return (u_int32_t)this->hlen;
}

/* getProtocol()関数 :プロトコル番号を返す関数 */
u_int8_t IPPacket::getProtocol (void) {
	return this->proto;
}

/* getNetworkDetail()関数
 * 概要 :ネットワーク層詳細を返す関数 */
string IPPacket::getNetworkDetail (void) {
	stringstream ss; /* 文字列格納 */
	ss.str(); /* 初期化 */
	ss << "[IPv4] " << inet_ntoa (this->saddr) << " -> ";
	ss << inet_ntoa (this->daddr) << " ("; /* アドレス */
	switch (this->proto) { /* プロトコル */
		case IP_TCP :
			ss << "TCP"; break;
		case IP_UDP :
			ss << "UDP"; break;
		case IP_ICMP :
			ss << "ICMP"; break;
		default :
			ss << (int)this->proto; break;
	}
	ss << ")\n";
	/* パケット長とヘッダ長 */
	ss << " > IPパケット長 :" << dec << this->len << "(ヘッダ :"
	<< (int)this->hlen << ")" << "バイト\n";
	/* 識別子とフラグメントオフセット */
	ss << " > 識別子 + フラグメントオフセット :" << dec << this->id
	<< " + " << this->off << " (0x" << hex << this->id
	<< " + 0x" << this->off <<")\n";
	/* サービスタイプ */
	ss << " > サービスタイプ :0x" << hex << (int)this->tos << "\n";
	/* 生存時間 */
	ss << " > 生存時間(TTL) :" << dec << (int)this->ttl << "\n";
	/* ヘッダチェックサム */
	ss << " > ヘッダチェックサム :0x" << hex << this->sum;
	/* チェックサムを計算 */
	if (this->checkSum()) ss << " (エラーなし)\n";
	else ss << " (エラーあり)\n";
	ss << " > 送信元IPアドレス :" << inet_ntoa(this->saddr) << "\n";
	ss << " > 宛先IPアドレス :" << inet_ntoa(this->daddr) << "\n";
	return ss.str();
}

/* checkSum()関数
 * ヘッダチェックサムを計算する関数
 * 返り値 :エラーなし -> true, エラーあり -> false */
bool IPPacket::checkSum (void) {
	u_int32_t csum = 0; /* 合計 */
	u_int16_t headlen = (u_int16_t)this->hlen / 2; /* 2バイト単位のヘッダ長 */
	for (int i = 0; i < headlen; i++) { /* ヘッダ部分 */
		csum += ntohs (this->checkhdr[i]); /* 総和 */
	}
	csum = csum + csum / 0x10000; /* 1の補数和の桁上がり分 */
	csum = csum - (csum / 0x10000) * 0x10000;
	/* ヘッダをすべて足したものの下位4ビットが0x0000となっているときtrue そうでなればfalse */
	if (csum % 0xffff) return false;
	else return true;
}

/* getPseudoHeader()関数 :疑似ヘッダを返す関数 */
struct pseudohdr *IPPacket::getPseudoHeader (void) {
	return this->pseudohdr;
}
