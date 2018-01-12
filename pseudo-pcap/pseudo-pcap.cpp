
#include "pseudo-pcap.h"
#include <stdlib.h>
#include <stdio.h>

using namespace std;

Packet::Packet (int id, double catime) {
       /* メンバを初期化 */
       this->pid = id;            /* パケット番号 */

	this->captime = catime;

	// パケット長
	this->len = 67 + rand() % 1000;

	pro[0] = DATALINK_IEEE802_3;

	for (int i = 1; i < 4; i++) {
		pro[i] = 0;
	}
	// エラーの有無
	int err = rand() % 10;
	if (err < 9) this->noterr = true;
	else this->noterr = false;

	// プロトコルの決定
	int randp = rand() % 100;
	if (randp < 10) {
		this->protocol = "ARP";
		pro[1] = NETWORK_ARP;
	} else {
		pro[1] = NETWORK_IP;
		if (randp < 50) {
			this->protocol = "TCP";
			pro[2] = TRANSPORT_TCP;
			pro[3] = APPLICATION_DEFAULT;
		} else if (randp < 60) {
			this->protocol = "ICMP";
			pro[2] = TRANSPORT_ICMP;
		} else if (randp < 70) {
			this->protocol = "DNS";
			pro[2] = TRANSPORT_UDP;
			pro[3] = APPLICATION_DNS;
		} else if (randp < 80) {
			this->protocol = "HTTP";
			pro[2] = TRANSPORT_UDP;
			pro[3] = APPLICATION_HTTP;
		} else if (randp < 85) {
			this->protocol = "HTTPS";
			pro[2] = TRANSPORT_UDP;
			pro[3] = APPLICATION_HTTPS;
		} else if (randp < 86) {
			this->protocol = "FTP";
			pro[2] = TRANSPORT_TCP;
			pro[3] = APPLICATION_FTP;
		} else if (randp < 88) {
			this->protocol = "DHCP";
			pro[2] = TRANSPORT_UDP;
			pro[3] = APPLICATION_DHCP_C;
		} else if (randp < 92) {
			this->protocol = "NTP";
			pro[2] = TRANSPORT_UDP;
			pro[3] = APPLICATION_NTP;
		} else if (randp < 95) {
			this->protocol = "Telnet";
			pro[2] = TRANSPORT_TCP;
			pro[3] = APPLICATION_TELNET;
		} else {
			this->protocol = "SSH";
			pro[2] = TRANSPORT_TCP;
			pro[3] = APPLICATION_SSH;
		}
	}

	// アドレス，備考の決定
	if (this->protocol == "ARP") {
		if (rand() % 2) {
			this->saddr = "00:00:00:00:00:00";
			this->daddr = "AA:BB:CC:DD:EE:FF";
			this->detail = "Who has X?";
		} else {
			this->saddr = "AA:BB:CC:DD:EE:FF";
			this->daddr = "00:00:00:00:00:00";
			this->detail = "X is Y";
		}
	} else {
		switch (rand() % 7) {
			case 0 :
				this->saddr = "192.168.2.119";
				switch (rand() % 6) {
					case 1 : this->daddr = "192.168.2.118"; break;
					case 2 : this->daddr = "192.168.2.120"; break;
					case 3 : this->daddr = "202.152.1.10"; break;
					case 4 : this->daddr = "202.152.1.9"; break;
					case 5 : this->daddr = "74.125.235.120"; break;
					case 0 : this->daddr = "183.79.71.173"; break;
				} break;
			case 1 :
				this->saddr = "192.168.2.118";
				switch (rand() % 6) {
					case 1 : this->daddr = "192.168.2.119"; break;
					case 2 : this->daddr = "192.168.2.120"; break;
					case 3 : this->daddr = "202.152.1.10"; break;
					case 4 : this->daddr = "202.152.1.9"; break;
					case 5 : this->daddr = "74.125.235.120"; break;
					case 0 : this->daddr = "183.79.71.173"; break;
				} break;
			case 2 : this->saddr = "192.168.2.120";
				switch (rand() % 6) {
					case 1 : this->daddr = "192.168.2.119"; break;
					case 2 : this->daddr = "192.168.2.118"; break;
					case 3 : this->daddr = "202.152.1.10"; break;
					case 4 : this->daddr = "202.152.1.9"; break;
					case 5 : this->daddr = "74.125.235.120"; break;
					case 0 : this->daddr = "183.79.71.173"; break;
				} break;
			case 3 : this->saddr = "202.152.1.10";
			switch (rand() % 6) {
				case 1 : this->daddr = "192.168.2.119"; break;
				case 2 : this->daddr = "192.168.2.118"; break;
				case 3 : this->daddr = "192.168.2.120"; break;
				case 4 : this->daddr = "202.152.1.9"; break;
				case 5 : this->daddr = "74.125.235.120"; break;
				case 0 : this->daddr = "183.79.71.173"; break;
			} break;
			case 4 : this->saddr = "202.152.1.9";
			switch (rand() % 6) {
				case 1 : this->daddr = "192.168.2.119"; break;
				case 2 : this->daddr = "192.168.2.118"; break;
				case 3 : this->daddr = "202.152.1.10"; break;
				case 4 : this->daddr = "192.168.2.120"; break;
				case 5 : this->daddr = "74.125.235.120"; break;
				case 0 : this->daddr = "183.79.71.173"; break;
			} break;
			case 5 : this->saddr = "74.125.235.120";
			switch (rand() % 6) {
				case 1 : this->daddr = "192.168.2.119"; break;
				case 2 : this->daddr = "192.168.2.118"; break;
				case 3 : this->daddr = "202.152.1.10"; break;
				case 4 : this->daddr = "202.152.1.9"; break;
				case 5 : this->daddr = "192.168.2.120"; break;
				case 0 : this->daddr = "183.79.71.173"; break;
			} break;
			case 6 : this->saddr = "183.79.71.173";
			switch (rand() % 6) {
				case 1 : this->daddr = "192.168.2.119"; break;
				case 2 : this->daddr = "192.168.2.118"; break;
				case 3 : this->daddr = "202.152.1.10"; break;
				case 4 : this->daddr = "202.152.1.9"; break;
				case 5 : this->daddr = "74.125.235.120"; break;
				case 0 : this->daddr = "192.168.2.120"; break;
			} break;
		}
		if (protocol == "ICMP") {
			// ICMPの備考
			this->detail = "ICMPメッセージ";
		} else if (protocol == "DNS") {
			this->detail = "google.comなど．DNSメッセージ";
		} else {
			this->detail = "[source PORT] -> [destination PORT]";
		}
	}

	// 各層詳細
	this->datalink = "データリンク層の詳細\n";
	this->network = "ネットワーク層の詳細\n";
	this->transport = "トランスポート層の詳細\n";
	this->application = "アプリケーション層の詳細\n";

}

string Packet::getSAddress (void) {
       return this->saddr;
}

string Packet::getDAddress (void) {
       return this->daddr;
}

u_int32_t Packet::getBytes (void) {
       return this->len;
}

int *Packet::getProtocolType (void) {
       return this->pro;
}

string Packet::packetData (void) {
       string sdata;
       stringstream ss;
       ss.str(""); /* 文字列加工用変数の初期化 */
       ss << dec << this->pid << "|";     /* パケットID */
       if (this->noterr)
              ss << 1 << "|";             /* エラーなし */
       else
              ss << 0 << "|";             /* エラーあり */
       ss << this->captime << "|";        /* 取得時刻 */
       ss << this->saddr << "|";          /* 送信元IPアドレス */
       ss << this->daddr << "|";          /* 宛先IPアドレス */
       ss << this->len << "|";            /* パケット長 */
       ss << this->protocol << "|";       /* プロトコル */
       ss << this->detail << "|";         /* 備考 */
       ss << this->datalink << "|";        /* データリンク層詳細 */
	ss << this->network << "|";         /* ネットワーク層詳細 */
       ss << this->transport << "|";       /* トランスポート層詳細 */
       ss << this->application << "|";     /* アプリケーション層詳細 */
       ss << "パケットの生データ" << "\n"; /* 生データ */
       sdata = ss.str(); /* データをstringに変換 */
       ss.str("");
       ss << WRITE_MAINPACKET << "|" << sdata.size() << "\n";
       sdata = ss.str() + sdata;
       return sdata;
}
