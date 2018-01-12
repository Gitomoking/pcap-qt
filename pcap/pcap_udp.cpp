#include "pcap_transport.h"
#include <netinet/udp.h>
#include <iostream>
using namespace std;

/* コンストラクタ :メンバ変数にデータ格納 */
UDPPacket::UDPPacket (const u_char *raw, struct pseudohdr *pseudohdr) {
	/* 変数宣言
	 * udphdr :UDPヘッダ <netinet/udp.h> */
	struct udphdr *udphdr = (struct udphdr *)raw;
	this->sport = ntohs (udphdr->source); /* 送信元ポート番号を取得*/
	this->dport = ntohs (udphdr->dest); /* 宛先ポート番号を取得 */
	this->len = ntohs (udphdr->len); /* データグラム長を取得*/
	this->check = ntohs (udphdr->check); /* チェックサムを取得 */
	this->pseudo = pseudohdr; /* 疑似ヘッダを取得 */
	this->pseudo->len = this->len;
	this->checkhdr = (u_int16_t *)raw; /* チェックサム用ヘッダを取得 */
}

/* getHeaderLen()関数 :データグラム長を返す関数 */
u_int16_t UDPPacket::getHeaderLen (void) {
	return this->len;
}

/* getSPort()関数 :送信元ポート番号を返す関数 */
u_int16_t UDPPacket::getSPort (void) {
	return this->sport;
}

/* getDPort()関数 :宛先ポート番号を返す関数 */
u_int16_t UDPPacket::getDPort (void) {
	return this->dport;
}

/* getPacketDetail()関数 :パケットの備考を返す関数 */
string UDPPacket::getPacketDetail (void) {
	stringstream ss; /* 文字列格納用 */
	ss.str("");
	ss << dec;
	ss << "[" << this->sport;
	switch (this->sport) { /* 送信元ポート番号 */
		case UDP_DNS :
			ss << "(DNS)"; break;
		case UDP_NTP :
			ss << "(NTP)"; break;
		case UDP_DHCP_SERVER :
			ss << "(DHCP:S)"; break;
		case UDP_DHCP_CLIENT :
			ss << "(DHCP:C)"; break;
		case UDP_HTTPS :
			ss << "(HTTPS)"; break;
	}
	ss << "] -> [" << this->dport;
	switch (this->dport) { /* 宛先ポート番号 */
		case UDP_DNS :
			ss << "(DNS)"; break;
		case UDP_NTP :
			ss << "(NTP)"; break;
		case UDP_DHCP_SERVER :
			ss << "(DHCP:S)"; break;
		case UDP_DHCP_CLIENT :
			ss << "(DHCP:C)"; break;
		case UDP_HTTPS :
			ss << "(HTTPS)"; break;
	}
	ss << "] データグラム長:" << this->len; /* データグラム長 */
	return ss.str();
}

/* getTransportDetail()関数 :トランスポート層詳細を返す関数 */
string UDPPacket::getTransportDetail (void) {
	stringstream ss;
	ss.str("");
	ss << "[UDP] " << this->getPacketDetail() << "\n";
	ss << " > 送信元ポート番号 :" << this->sport << "\n";
	ss << " > 宛先ポート番号 :" << this->dport << "\n";
	ss << " > チェックサム :0x" << hex << this->check;
	/* チェックサムを計算 */
	if (this->checkSum ()) {
		ss << " (エラーなし)\n";
	}else {
		ss << " (エラーあり)\n";
	}
	return ss.str(); /* トランスポート層詳細を返す */
}

/* CheckSum()関数
 * 概要 :チェックサムを計算する関数
 * 返り値 :エラーなし -> true, エラーあり -> false */
bool UDPPacket::checkSum (void) {
	u_int32_t sum = 0;
	/* チェックサムが0のときtrue */
	if (!this->check) return true;
	/* 疑似ヘッダの全データの総和をとる */
	for (int i = 0; i < 2; i++) {
              sum += (this->pseudo->saddr[2*i] * 256);
              sum += this->pseudo->saddr[2*i + 1];
              sum += (this->pseudo->daddr[2*i] * 256);
              sum += this->pseudo->daddr[2*i + 1];
       }
	sum += this->pseudo->proto;
	sum += this->pseudo->len;
	/* UDPヘッダの全データの総和をとる */
	for (int i = 0; 2*i < this->len; i++) {
		sum += ntohs (this->checkhdr[i]);
	}
	sum = sum + sum / 0x10000; /* 1の補数和の桁上がり分 */
	sum = sum - ((sum / 0x10000) * 0x10000); /* 5桁以上を0にする */
	if (sum % 0xffff) /* エラーありの場合 */
		return false;
	else /* エラーなしの場合 */
		return true;
}
