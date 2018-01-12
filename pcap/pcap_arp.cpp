#include "pcap_network.h"
#include <netinet/if_ether.h>
using namespace std;

/* コンストラクタ :メンバ変数にデータを格納する */
ARPPacket::ARPPacket (const u_char *raw) {
	/* 変数宣言
	 * arp :ARPヘッダ "pcap_network.h"
	 * ss :文字列加工用 */
	struct arp *arphdr;
	stringstream ss;
	ss.str(""); /* 初期化 */
	arphdr = (struct arp *)raw; /* 型変換 */
	this->htype = ntohs (arphdr->htype); /* ハードウェアタイプ */
	this->ptype = ntohs (arphdr->ptype); /* プロトコルタイプ */
	this->opec = ntohs (arphdr->opec); /* オペコード */
	/* 送信元MACアドレス */
	ss.str("");
	for (int i = 0; i < 5; i++) {
              /* 16進数で，空きの場所には0を詰める */
              ss << setw(2) << setfill('0') << hex
              << (int)arphdr->sha[i] << ":";
       }
       ss << setw(2) << setfill('0') << hex
       << (int)arphdr->sha[5];
	this->sha = ss.str(); /* 送信元MACアドレスを取得 */
	/* 送信元IPアドレス */
	ss.str("");
	for (int i = 0; i < 3; i++) {
              ss << dec << (int)arphdr->spa[i] << ".";
        }
       ss << dec << (int)arphdr->spa[3];
	this->spa = ss.str(); /* 送信元IPアドレスを取得 */
	/* 宛先MACアドレス */
	ss.str("");
	for (int i = 0; i < 5; i++) {
              /* 16進数で，空きの場所には0を詰める */
              ss << setw(2) << setfill('0') << hex
              << (int)arphdr->dha[i] << ":";
       }
       ss << setw(2) << setfill('0') << hex << (int)arphdr->dha[5];
	this->dha = ss.str(); /* 宛先MACアドレスを取得 */
	/* 宛先IPアドレス */
	ss.str("");
	for (int i = 0; i < 3; i++) {
              ss << dec << (int)arphdr->dpa[i] << ".";
       }
       ss << dec << (int)arphdr->dpa[3];
	this->dpa = ss.str(); /* 宛先IPアドレスを取得 */
}

/* getSAddr()関数 :送信元アドレスを返す関数 */
string ARPPacket::getSAddr (void) {
	return this->sha;
}

/* getDAddr()関数 :宛先アドレスを返す関数 */
string ARPPacket::getDAddr (void) {
	return this->dha;
}

/* getPacketDetail()関数 :パケットの備考を返す関数 */
string ARPPacket::getPacketDetail (void) {
	stringstream ss;
	ss.str("");
	/* オペレーションタイプ */
	switch (this->opec) {
		case ARP_REQUEST :
			ss << this->dpa << "のMACアドレスは?(Who has)  ";
			ss << this->spa << "より(Tell)";
			break;
		case ARP_REPLY :
			ss << this->spa << "のMACアドレスは";
			ss << this->sha;
			break;
	}
	return ss.str();
}

/* getNetworkDetail()関数 :ネットワーク層詳細を返す関数 */
string ARPPacket::getNetworkDetail (void) {
	stringstream ss; /* 文字列格納用 */
	ss.str(""); /* 初期化 */
	ss << "[ARP] " << this->sha << " -> " << this->dha
	<< " (";
	/* ハードウェアタイプ */
	switch (this->htype) {
		case ARP_ETHER :
			ss << "Ethernet"; break;
		case ARP_IEEE802_ :
			ss << "IEEE802.x"; break;
	}
	ss << ", ";
	/* プロトコルタイプ */
	switch (this->ptype) {
		case ETHER_IP :
			ss << "IP"; break;
	}
	ss << ") ";
	/* オペレーションタイプ */
	switch (this->opec) {
		case ARP_REQUEST :
			ss << "ARP要求\n"; break;
		case ARP_REPLY :
			ss << "ARP応答\n"; break;
	}
	/* プロトコルアドレス */
	ss << " > 送信元プロトコルアドレス :" << this->spa << "\n";
	ss << " > 宛先プロトコルアドレス　 :" << this->dpa << "\n";
	return ss.str(); /* ネットワーク層詳細を返す */
}
