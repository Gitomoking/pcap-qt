#ifndef INCLUDED_PCAP_STATISTICS
#define INCLUDED_PCAP_STATISTICS
#include <iomanip>
#include <string>
#include <sstream>
#include <cctype>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <map>
#include <vector>

/* 通信識別用 */
#define WRITE_ERROR         -1
#define WRITE_MAINPACKET    1
#define WRITE_PROTOCOL      2
#define WRITE_TRAFFIC       3
#define WRITE_NODES         4
#define WRITE_END           5

/* 定数定義 */
#define DATALINK_IEEE802_3  1
#define NETWORK_IP          2
#define NETWORK_ARP         3
#define TRANSPORT_TCP       4
#define TRANSPORT_UDP       5
#define TRANSPORT_ICMP      6
#define APPLICATION_FTPDATA 7
#define APPLICATION_FTP     8
#define APPLICATION_TELNET  9
#define APPLICATION_DNS     10
#define APPLICATION_HTTP    11
#define APPLICATION_DEFAULT 12
#define APPLICATION_DHCP_S  13
#define APPLICATION_DHCP_C  14
#define APPLICATION_NTP     15
#define APPLICATION_HTTPS   16
#define APPLICATION_SSH     17

const int interval = 10;    /* トラフィック量を取得する時間の間隔 */

struct protoRate {
	/* ネットワーク／トランスポートプロトコル全体に対する割合 */
	double arp;
	double tcp;
	double udp;
	double icmp;
	/* アプリケーションプロトコル全体に対する割合 */
	double dns;
	double http;
	double https;
	double ftp;
	double ntp;
	double dhcp;
	double telnet;
	double ssh;
	double others;
};

class Protocol {
public:
	Protocol(); /* 初期化 */
	void setProtocolData (int data[]); /* プロトコルデータ更新 */
	void sendProtocolData (void); /* プロトコルデータ出力 */
private:
	unsigned int arp;	/* ARP */
	unsigned int tcp; 	/* TCP */
	unsigned int udp; 	/* UDP */
	unsigned int icmp; 	/* ICMP */
	unsigned int dns;	/* DNS */
	unsigned int http;	/* HTTP */
	unsigned int https;	/* HTTPS */
	unsigned int ftp;	/* FTP */
	unsigned int ntp;	/* NTP */
	unsigned int dhcp;	/* DHCP */
	unsigned int telnet;	/* Telnet */
	unsigned int ssh;	/* SSH */
	unsigned int others;	/* その他 */
};

class CoupleOfNodes {
public:
	CoupleOfNodes(std::string src, std::string dst, unsigned int len, int now);
	void setData(unsigned int len, int now); /* トラフィック量データ更新 */
	std::string trafficData(int now); /* 出力用トラフィック量データ */
	std::string nodeData(int now); /* 出力用ノードの組みデータ */
	std::string sname;		/* 送信元ノード名 */
	std::string dname;		/* 宛先ノード名 */
private:
	std::vector<double> traffic;	/* トラフィック量 */
	unsigned long int bytes;		/* 通信量 */
	int start;				/* 開始時刻 */
};

class Traffic {
public:
	Traffic();
	void setData(std::string src, std::string dst, unsigned int len); /* データ更新 */
	void sendTrafficData(void); 	/* トラフィック量出力 */
	void sendNodeData(void);		/* ノードの組み出力 */
private:
	int number(std::string src, std::string dst);	/* src,dstのインデックスを返す */
	int now;	/* 現在のデータ番号 */
	std::vector<CoupleOfNodes> nodev;	/* ノード組みデータ */
};

#endif
