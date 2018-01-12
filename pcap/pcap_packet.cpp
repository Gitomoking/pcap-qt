#include <iostream>
#include <string.h>
#include <stdio.h>
#include "pcap_packets.h"
using namespace std;

extern PacketCaptureHandler *pcap; // pcap_main.cpp

/* コンストラクタ
 * 生パケットからデータを読み取る */
Packet::Packet (int *id, const struct pcap_pkthdr *pkthdr, const u_char *raw, long int starttime) {
       /* メンバを初期化 */
       this->pid = *id;            /* パケット番号 */
       this->noterr = true;
       for (int i = 0; i < 4; i++)
              this->pro[i] = 0;    /* プロトコル識別番号 */
       this->captime = pkthdr->ts.tv_sec - starttime; /* 取得時間 */
       this->len = pkthdr->len;    /* パケット長 */
       this->dlen = pkthdr->len;   /* データ長 */
       this->saddr = "";           /* 送信元アドレス */
       this->daddr = "";           /* 宛先アドレス */
       this->protocol = "";        /* プロトコル名 */
       this->detail = "";          /* 備考 */
       this->datalink = "";        /* データリンク層詳細 */
       this->network = "";         /* ネットワーク層詳細 */
       this->transport = "";       /* トランスポート層詳細 */
       this->application = "";     /* アプリケーション層詳細 */
       /* データリンクによって場合分け */
       switch (pcap->getDatalinkType()) {
              case DLT_EN10MB :
                     /* IEEE802.3（Ethernet II） */
                     this->pro[0] = DATALINK_IEEE802_3;
                     break;
       }
       this->getDatalink (raw); /* 各層の情報を取得 */
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

/* Datalink()関数
 * 概要 :データリンク層に関する情報をパケットから取得する関数
 * raw :パケットの生データ
 * 備考 :内部でNetwork()関数を呼び出す */
void Packet::getDatalink (const u_char *raw) {
       u_int16_t len = 0; /* データリンクのヘッダ長 */
       switch (this->pro[0]) {
              /* IEEE802.3の場合 */
              case DATALINK_IEEE802_3 : {
                     IEEE802_3 ether(raw); /* etherにデータを格納 */
                     len = 14; /* ヘッダ長は14 */
                     /* イーサタイプからネットワーク層のプロトコルを識別 */
                     switch (ether.getEtherType()) {
                            case ETHER_IP :
                                   /* IP */
                                   this->pro[1] = NETWORK_IP;
                                   break;
                            case ETHER_ARP :
                                   /* ARP */
                                   this->pro[1] = NETWORK_ARP;
                                   break;
                     }
                     this->dlen -= len; /* データ長を更新する */
                     /* データリンク層詳細を取得 */
                     this->datalink = ether.getDatalinkDetail();
              }
                     break;
       }
       /* ネットワーク層の情報を取得 */
       this->getNetwork (raw + len);
}

/* Network()関数
 * 概要 :ネットワーク層の情報を取得する関数
 * raw :パケットの生データで，ポインタはネットワークプロトコルヘッダの先頭を示す
 * 備考 :内部でTransport()関数を呼び出す*/
void Packet::getNetwork (const u_char *raw) {
       switch (this->pro[1]) {
              case NETWORK_IP : {
                     /* IP */
                     IPPacket ip(raw);   /* ipにデータ格納 */
                     u_int32_t len = ip.getIPHeaderLen(); /* IPヘッダ長を取得 */
                     this->dlen -= len; /* データ長 */
                     this->saddr = ip.getSAddr(); /* 送信元IPアドレスを取得 */
                     this->daddr = ip.getDAddr(); /* 宛先IPアドレスを取得*/
                     this->network = ip.getNetworkDetail(); /* ネットワーク層詳細を取得*/
                     struct pseudohdr *pseudohdr; /* 疑似ヘッダの宣言 */
                     pseudohdr = ip.getPseudoHeader(); /* 疑似ヘッダの取得 */
                     /* トランスポート層のプロトコルを識別 */
                     switch (ip.getProtocol()) {
                            case IP_TCP :
                                   /* TCP */
                                   this->pro[2] = TRANSPORT_TCP;
                                   pseudohdr->proto = IP_TCP; /* 疑似ヘッダのプロトコル */
                                   this->protocol = "TCP"; /* 表示用プロトコル */
                                   break;
                            case IP_UDP :
                                   /* UDP */
                                   this->pro[2] = TRANSPORT_UDP;
                                   pseudohdr->proto = IP_UDP; /* 疑似ヘッダのプロトコル */
                                   this->protocol = "UDP"; /* 表示用プロトコル */
                                   break;
                            case IP_ICMP :
                                   /* ICMP */
                                   this->pro[2] = TRANSPORT_ICMP;
                                   this->protocol = "ICMP";
                                   break;
                     }
                     /* トランスポート層の情報を取得 */
                     this->getTransport ((raw + len), pseudohdr);
              }break;
              case NETWORK_ARP : {
                     /* ARP */
                     ARPPacket arp(raw); /* arpにデータ格納 */
                     this->saddr = arp.getSAddr(); /* 送信元アドレスを取得 */
                     this->daddr = arp.getDAddr(); /* 宛先アドレスを取得 */
                     this->protocol = "ARP"; /* 表示用プロトコル */
                     this->detail = arp.getPacketDetail(); /* 備考を取得 */
                     this->network = arp.getNetworkDetail(); /* ネットワーク層詳細を取得 */
              }break;
       }
}

/* Transport()関数
 * 概要 :トランスポート層の情報を取得する関数
 * raw :パケットの生データ，ポインタはトランスポートプロトコルヘッダの先頭を示す
 * pseudohdr :TCP, UDP用の疑似ヘッダ，チェックサム計算に用いる
 * 備考 :内部でApplication()関数を呼び出す */
void Packet::getTransport (const u_char *raw, struct pseudohdr *pseudohdr) {
       switch (this->pro[2]) {
              case TRANSPORT_TCP : {
                     /* TCP */
                     TCPPacket tcp(raw, pseudohdr); /* tcpにデータ格納 */
                     u_int16_t len = tcp.getHeaderLen(); /* セグメント長を取得 */
                     u_int16_t sport = tcp.getSPort(); /* 送信元ポート番号を取得 */
                     u_int16_t dport = tcp.getDPort(); /* 宛先ポート番号を取得 */
                     this->dlen -= len; /* データ長 */
                     if (dport == TCP_FTP_DATA || sport == TCP_FTP_DATA) {
                            /* FTPdata */
                            this->pro[3] = APPLICATION_FTPDATA;
                            this->protocol = "FTP"; /* プロトコル表示用 */
                     }else if (dport == TCP_FTP || sport == TCP_FTP) {
                            /* FTP */
                            this->pro[3] = APPLICATION_FTP;
                            this->protocol = "FTP"; /* プロトコル表示用 */
                     }else if (dport == TCP_TELNET || sport == TCP_TELNET) {
                            /* Telnet */
                            this->pro[3] = APPLICATION_TELNET;
                            this->protocol = "Telnet"; /* プロトコル表示用 */
                     }else if (dport == TCP_SSH || sport == TCP_SSH) {
                            // SSH
                            this->pro[3] = APPLICATION_SSH;
                            this->protocol = "SSH";
                     }else if (dport == TCP_DNS || sport == TCP_DNS) {
                            /* DNS */
                            this->pro[3] = APPLICATION_DNS;
                            this->protocol = "DNS"; /* プロトコル表示用 */
                     }else if (dport == TCP_HTTP || sport == TCP_HTTP) {
                            /* HTTP */
                            this->pro[3] = APPLICATION_HTTP;
                            this->protocol = "HTTP"; /* プロトコル表示用 */
                     }else if (dport == TCP_HTTPS || sport == TCP_HTTPS){
                            /* HTTPS */
                            this->pro[3] = APPLICATION_HTTPS;
                            this->protocol = "HTTPS";
                     }else {
                            /* それ以外のメッセージ */
                            this->pro[3] = APPLICATION_DEFAULT;
                     }
                     this->noterr = tcp.checkSum(); /* エラーパケットかどうか判断 */
                     this->detail = tcp.getPacketDetail ();
                     /* トランスポート層詳細を取得 */
                     this->transport = tcp.getTransportDetail();
                     if (this->dlen > 0 && this->noterr) {
                            /* アプリケーションメッセージが存在するか，パケットが壊れてない場合 */
                            /* アプリケーション層の情報を取得 */
                            this->getApplication (raw + len);
                     }
              }break;
              case TRANSPORT_UDP : {
                     /* UDP */
                     UDPPacket udp(raw, pseudohdr); /* udpにデータを格納 */
                     u_int16_t len = 8;
                     u_int16_t sport = udp.getSPort(); /* 送信元ポート番号を取得 */
                     u_int16_t dport = udp.getDPort(); /* 宛先ポート番号を取得 */
                     this->dlen -= 8; /* データ長 */
                     if (sport == UDP_DNS || dport == UDP_DNS) {
                            /* DNS */
                            this->pro[3] = APPLICATION_DNS;
                            this->protocol = "DNS"; /* プロトコル表示用 */
                     }else if (sport == UDP_DHCP_SERVER || dport == UDP_DHCP_SERVER) {
                            /* DHCP_SERVER*/
                            this->pro[3] = APPLICATION_DHCP_S;
                            this->protocol = "DHCP"; /* プロトコル表示用 */
                     }else if (sport == UDP_DHCP_CLIENT || dport == UDP_DHCP_CLIENT) {
                            /* DHCP_CLIENT */
                            this->pro[3] = APPLICATION_DHCP_C;
                            this->protocol = "DHCP"; /* プロトコル表示用 */
                     }else if (sport == UDP_NTP || dport == UDP_NTP) {
                            /* NTP */
                            this->pro[3] = APPLICATION_NTP;
                            this->protocol = "NTP"; /* プロトコル表示用 */
                     }else if (sport == UDP_HTTPS || dport == UDP_HTTPS) {
                            /* HTTPS */
                            this->pro[3] = APPLICATION_HTTPS;
                            this->protocol = "HTTPS";
                     }else {
                            /* それ以外のメッセージ */
                            this->pro[3] = APPLICATION_DEFAULT;
                     }
                     this->noterr = udp.checkSum();
                     this->detail = udp.getPacketDetail();
                     /* トランスポート層詳細を取得 */
                     this->transport = udp.getTransportDetail();
                     if (this->dlen > 0) { /* アプリケーションメッセージが存在する場合 */
                            /* アプリケーション層の情報を取得 */
                            this->getApplication (raw + len);
                     }
              }break;
              case TRANSPORT_ICMP : {
                     /* ICMP */
                     ICMPPacket icmp(raw);
                     this->detail = icmp.getPacketDetail(raw + 4); /* 備考を取得 */
                     /* ネットワーク層詳細を取得 */
                     this->transport = icmp.getTransportDetail(raw + 4);
              }break;
       }
       if (pseudohdr) delete pseudohdr;
}

/* Application()関数
 * 概要 :アプリケーション層の情報を取得する関数
 * raw :パケットの生データ，ポインタはアプリケーションメッセージの先頭を示す */
void Packet::getApplication (const u_char *raw) {
       switch (this->pro[3]) {
              case APPLICATION_DNS : {
                     /* DNS */
                     DNSPacket dns(raw); /* dnsにデータ格納 */
                     this->detail = dns.getPacketDetail(); /* 備考を取得 */
                     /* アプリケーション層詳細を取得 */
                     this->application = dns.getApplicationDetail();
              }break;
              case APPLICATION_HTTP : {
                     /* HTTP */
/*                     HTTPPacket http(raw); // httpにデータ格納
                     this->detail = http.getPacketDetail(); // 備考を取得
                     // アプリケーション層詳細を取得
                     this->application = http.getApplicationDetail();*/
              }break;
              case APPLICATION_DHCP_S : {
                     /* DHCP_SERVER */
              }break;
              case APPLICATION_DHCP_C : {
                     /* DHCP_CLIENT */
              }break;
              case APPLICATION_NTP : {
                     /* NTP */
              }break;
              case APPLICATION_DEFAULT : {
                     /* デフォルト */
              }break;
       }
}

string Packet::rawData (const u_char *raw) {
       stringstream ss;
       ss.str("");
       ss << hex;
       ss << "[データ（16進数）]";
       for (int i = 0; i < this->len; i++) {
              if (!(i % 16)) {
                     ss << "\n";
                     ss << setfill('0') << setw(4) << hex << i;
              }
              (!(i % 8))? ss << "  " : ss << " ";
              ss << setfill('0') << hex << setw(2) << (int)raw[i];
       }
       return ss.str();
}

/* writeData()関数
 * ファイル書き込み用の文字列を出力する関数 */
string Packet::packetData (string rawdata) {
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
//       if (!datalink.empty())
              ss << this->datalink << "|";        /* データリンク層詳細 */
//       else ss << "___" << "|";
//       if (!network.empty())
              ss << this->network << "|";         /* ネットワーク層詳細 */
//       else ss << "___" << "|";
//       if (!transport.empty())
              ss << this->transport << "|";       /* トランスポート層詳細 */
//       else ss << "___" << "|";
//       if (!application.empty())
              ss << this->application << "|\n";     /* アプリケーション層詳細 */
//       else ss << "___" << "|\n";
       ss << rawdata; /* 生データ */
       sdata = ss.str(); /* データをstringに変換 */
       ss.str("");
       ss << WRITE_MAINPACKET << "|" << sdata.size() << "\n";
       sdata = ss.str() + sdata;
       return sdata;
}
