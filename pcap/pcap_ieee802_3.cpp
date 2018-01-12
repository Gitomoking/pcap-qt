#include "pcap_datalink.h"
#include <net/ethernet.h>

using namespace std;

/* コンストラクタ
 * IEEE802_3クラスのメンバ変数を初期化し，格納する */
IEEE802_3::IEEE802_3 (const u_char *raw) {
	/* 変数宣言
	 * ss :文字列加工用 <sstream>
	 * addrp :MACアドレス（数のみ）の格納用
	 * hdr :IEEE802.3ヘッダ <net/ethernet.h> */
	stringstream ss;
	u_char *addrp;
	struct ether_header *hdr = (struct ether_header *)raw;
	/* メンバ変数初期化 */
	this->shost = ""; /* 送信元MACアドレス*/
	this->dhost = ""; /* 宛先MACアドレス */
	this->type = 0; /* イーサタイプ */
	/* 送信元MACアドレス */
	ss.str(); /* 初期化 */
	addrp = hdr->ether_shost;
	for (int i = 0; i < 5; i++) {
	       /* 16進数で，空きの場所には0を詰める */
		/* setw()関数 :引数で指定した長さで表示を固定する関数
		 * setfill()関数 :setw()で指定した長さよりも短い場合に，
		 * 引数の文字で空いた場所を詰める関数
		 * hex :16進数で表示
		 * <iomanip> */
	       ss << setw(2) << setfill('0') << hex
	       << (int)addrp[i] << ":";
	}
	ss << setw(2) << setfill('0') << hex << (int)addrp[5];
	this->shost = ss.str(); /* 送信元MACアドレスを取得 */
	/* 宛先MACアドレス */
	ss.str(""); /* 初期化 */
	addrp = hdr->ether_dhost;
	for (int i = 0; i < 5; i++) {
	       ss << setw(2) << setfill('0') << hex
	       << (int)addrp[i] << ":";
	}
	ss << setw(2) << setfill('0') << hex << (int)addrp[5];
	this->dhost = ss.str(); /* 宛先MACアドレスを取得 */
	/* u_int16_t ntohs(u_int16_t)関数
	 * ネットワークバイトオーダからホストバイトオーダに変換する関数
	 * 16ビット以上のフィールドは必ずこの関数を通す
	 * <arpa/inet.h> <netinet/in.h> */
	this->type = ntohs (hdr->ether_type); /* イーサタイプを取得 */
}

/* getEtherType()関数
 * イーサタイプを返す関数 */
u_int16_t IEEE802_3::getEtherType (void) {
	return this->type;
}


/* getDatalinkDetail()関数
 * データリンク層詳細を返す関数 */
string IEEE802_3::getDatalinkDetail (void) {
	/* 変数宣言
	 * ss :文字列格納用 */
	stringstream ss;
	ss.str(); /* 初期化 */
	/* ssに格納 */
	ss << "[IEEE802.3] " << this->shost << " -> "
	<< this->dhost << " (";
	switch (this->type) {
		case ETHER_IP :
			ss << "IP"; break;
		case ETHER_ARP :
			ss << "ARP"; break;
	}
	ss << ")\n";
	return ss.str(); /* データリンク層詳細を返す */
}
