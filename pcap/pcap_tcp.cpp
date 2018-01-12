#include "pcap_transport.h"
#include <netinet/tcp.h>
#include <iostream>

using namespace std;

#define TCP_FTP_DATA 20
#define TCP_FTP 21
#define TCP_TELNET 23
#define TCP_SMTP 25
#define TCP_DNS 53
#define TCP_HTTP 80
#define TCP_NTP 123
#define TCP_HTTPS 443

/* コンストラクタ :メンバ変数にデータ格納 */
TCPPacket::TCPPacket (const u_char *raw, struct pseudohdr *pseudohdr) {
	/* 変数宣言
	 * tcphdr :TCPヘッダ <netinet/tcp.h>
	 * ss :文字列加工用
	 * checkhdr :チェックサム計算用 */
	struct tcphdr *tcphdr;
	stringstream ss;
	tcphdr = (struct tcphdr *)raw;
	this->sport = ntohs (tcphdr->source); /* 送信元ポート番号を取得 */
	this->dport = ntohs (tcphdr->dest); /* 宛先ポート番号を取得 */
	this->seq = ntohs (tcphdr->seq); /* シーケンス番号を取得 */
	this->ack_seq = ntohs (tcphdr->ack_seq); /* ACKのシーケンス番号を取得 */
	this->len = (4 * tcphdr->doff); /* セグメント長を取得 */
	this->urg = (tcphdr->urg) ? true : false; /* URGフラグ */
	this->ack = (tcphdr->ack) ? true : false; /* ACKフラグ */
	this->psh = (tcphdr->psh) ? true : false; /* PSHフラグ */
	this->rst = (tcphdr->rst) ? true : false; /* RSTフラグ */
	this->syn = (tcphdr->syn) ? true : false; /* SYNフラグ */
	this->fin = (tcphdr->fin) ? true : false; /* FINフラグ */
	this->window = ntohs (tcphdr->window); /* ウィンドウサイズを取得 */
	this->check = ntohs (tcphdr->check); /* チェックサムを取得 */
	this->urg_ptr = ntohs (tcphdr->urg_ptr); /* 緊急ポインタを取得 */
	this->pseudo = pseudohdr; /* 疑似ヘッダを取得 */
	this->pseudo->len = this->len; /* 疑似ヘッダのセグメント長フィールド */
	this->checkhdr = (u_int16_t *)raw; /* チェックサムヘッダを取得 */
	int doff = tcphdr->doff - 5;
	ss.str("");
	/* オプションありの場合 */
       while (doff > 0) {
              struct tcpopts *opts; /* TCPオプション */
              opts = (struct tcpopts *)(raw + sizeof (struct tcphdr));
              ss << " > オプション：" << (int)opts->type << "(";
              switch (opts->type) {
                     /* オプションタイプ */
                     case TCPOPT_EOL : ss << "End Of Option List"; break;
                     case TCPOPT_NOP : ss << "No Operation"; break;
                     case TCPOPT_MAXSEG :
                            ss << "最大セグメント長；";
                            ss << "MSS = " << dec << ntohs (opts->value);
                            break;
                     case TCPOPT_WINDOW : ss << "Window Scale"; break;
                     case TCPOPT_SACK_PERMITTED : ss << "Sack Permitted"; break;
                     case TCPOPT_SACK : ss << "Sack"; break;
                     case TCPOPT_TIMESTAMP : ss << "Time Stamp"; break;
              }
              ss << ")\n";
              doff--;
       }
	this->opts = ss.str();
}

/* getHeaderLen()関数 :セグメント長を返す関数 */
u_int16_t TCPPacket::getHeaderLen (void) {
	return this->len;
}

/* getSPort()関数 :送信元ポート番号を返す関数 */
u_int16_t TCPPacket::getSPort (void) {
	return this->sport;
}

/* getDPort()関数 :宛先ポート番号を返す関数 */
u_int16_t TCPPacket::getDPort (void) {
	return this->dport;
}

/* getPacketDetail()関数
 * 概要 :パケットの備考を返す関数
 * [送信元ポート番号] -> [宛先ポート番号] :シーケンス番号 (ACKの番号) フラグ */
string TCPPacket::getPacketDetail (void) {
	stringstream ss; /* 文字列格納用 */
	ss.str(""); /* 初期化 */
	ss << dec; /* 10進数で表示 */
	ss << "[" << this->sport;
	switch (this->sport) { /* 送信元ポート番号 */
		case TCP_FTP :
		ss << "(FTP)"; break;
		case TCP_FTP_DATA :
		ss << "(FTPdata)"; break;
		case TCP_DNS :
		ss << "(DNS)"; break;
		case TCP_HTTP :
		ss << "(HTTP)"; break;
		case TCP_TELNET :
		ss << "(Telnet)"; break;
		case TCP_SMTP :
		ss << "(SMTP)"; break;
		case TCP_NTP :
		ss << "(NTP)"; break;
		case TCP_HTTPS :
		ss << "(HTTPS)"; break;
	}
	ss << "] -> [" << this->dport;
	switch (this->dport) { /* 宛先ポート番号 */
		case TCP_FTP :
		ss << "(FTP)"; break;
		case TCP_FTP_DATA :
		ss << "(FTPdata)"; break;
		case TCP_DNS :
		ss << "(DNS)"; break;
		case TCP_HTTP :
		ss << "(HTTP)"; break;
		case TCP_TELNET :
		ss << "(Telnet)"; break;
		case TCP_SMTP :
		ss << "(SMTP)"; break;
		case TCP_NTP :
		ss << "(NTP)"; break;
		case TCP_HTTPS :
		ss << "(HTTPS)"; break;
	}
	ss << "] シーケンス番号:" << this->seq << "(ACK:" << this->ack_seq
	<< ") ";
	/* 各フラグ */
	if (this->syn) ss << "SYN=1 ";
	if (this->ack) ss << "ACK=1 ";
	if (this->fin) ss << "FIN=1 ";
	if (this->urg) ss << "URG=1(" << this->urg_ptr << ")";
	if (this->psh) ss << "PSH=1 ";
	if (this->rst) ss << "RST=1 ";
	return ss.str();
}

/* getTransportDetail()関数 :トランスポート層詳細を返す関数 */
string TCPPacket::getTransportDetail (void) {
	stringstream ss;
	ss.str("");
	ss << "[TCP] " << this->getPacketDetail() << "\n";
	ss << " > 送信元ポート番号 :" << this->sport << "\n";
	ss << " > 宛先ポート番号 :" << this->dport << "\n";
	ss << " > セグメント長 :" << dec << this->len << "バイト\n";
	ss << " > ウィンドウサイズ :" << dec << this->window << "\n";
	ss << " > チェックサム :0x" << hex << this->check;
	/* チェックサムを計算 */
	if (this->checkSum ()) ss << " (エラーなし)\n";
	else ss << " (エラーあり)\n";
	ss << this->opts; /* オプション */
	return ss.str(); /* トランスポート層詳細を返す */
}

/* checkSum()関数 :ヘッダチェックサムを計算する関数
 * 返り値 :エラーなし -> true, エラーあり -> false */
bool TCPPacket::checkSum (void) {
	u_int32_t sum = 0;
	/* 疑似ヘッダの全データの総和をとる */
	for (int i = 0; i < 2; i++) {
              sum += (this->pseudo->saddr[2*i] * 256);
              sum += this->pseudo->saddr[2*i + 1];
              sum += (this->pseudo->daddr[2*i] * 256);
              sum += this->pseudo->daddr[2*i + 1];
       }
	sum += this->pseudo->proto;
	sum += this->pseudo->len;
	/* TCPヘッダの全データの総和をとる */
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
