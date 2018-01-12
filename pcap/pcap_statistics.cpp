#include "pcap_statistics.h"
#include <sys/file.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>
#include <stdio.h>
using namespace std;

extern fd_set fdset;

/* Protocol::Protocol()
 * コンストラクタ：メンバ変数の初期化 */
Protocol::Protocol() {
	arp = 0;
	tcp = 0;
	udp = 0;
	icmp = 0;
	dns = 0;
	http = 0;
	https = 0;
	ftp = 0;
	ntp = 0;
	dhcp = 0;
	telnet = 0;
	ssh = 0;
	others = 0;
}

/* Protocol::setProtocolData()
 * プロトコル出現数を加える関数（データの計算） */
void Protocol::setProtocolData (int data[]) {
	if (data[1] == NETWORK_ARP)
		this->arp++;
	switch (data[2]) {
		case TRANSPORT_TCP : this->tcp++; break;
		case TRANSPORT_UDP : this->udp++; break;
		case TRANSPORT_ICMP : this->icmp++; break;
	}
	switch (data[3]) {
		case APPLICATION_DNS : this->dns++; break;
		case APPLICATION_HTTP : this->http++; break;
		case APPLICATION_HTTPS : this->https++; break;
		case APPLICATION_FTP :
		case APPLICATION_FTPDATA :
			this->ftp++; break;
		case APPLICATION_NTP : this->ntp++; break;
		case APPLICATION_DHCP_C :
		case APPLICATION_DHCP_S :
			this->dhcp++; break;
		case APPLICATION_TELNET : this->telnet++; break;
		case APPLICATION_SSH : this->ssh++; break;
		case APPLICATION_DEFAULT : this->others++; break;
	}
}

/* Protocol::saveProtocolData()
 * プロトコルデータを標準出力に出力する関数 */
void Protocol::sendProtocolData (void) {
	unsigned int sumt = 0, suma = 0;
	const char *data;
	int n;
	string sdata;
	stringstream ss;
	ss.str("");
	sumt = arp + tcp + udp + icmp;
	suma = dns + http + https + ftp + ntp + dhcp + telnet + ssh + others;
	struct protoRate rate;
	if (sumt) {
		rate.arp = ((double)arp/sumt)*100;
		rate.tcp = ((double)tcp/sumt)*100;
		rate.udp = ((double)udp/sumt)*100;
		rate.icmp = ((double)icmp/sumt)*100;
	} else {
		rate.arp = 0.0;
		rate.tcp = 0.0;
		rate.udp = 0.0;
		rate.icmp = 0.0;
	}
	if (suma) {
		rate.dns = ((double)dns/suma)*100;
		rate.http = ((double)http/suma)*100;
		rate.https = ((double)https/suma)*100;
		rate.ftp = ((double)ftp/suma)*100;
		rate.ntp = ((double)ntp/suma)*100;
		rate.dhcp = ((double)dhcp/suma)*100;
		rate.telnet = ((double)telnet/suma)*100;
		rate.ssh = ((double)ssh/suma)*100;
		rate.others = ((double)others/suma)*100;
	} else {
		rate.dns = 0.0;
		rate.http = 0.0;
		rate.https = 0.0;
		rate.ftp = 0.0;
		rate.ntp = 0.0;
		rate.dhcp = 0.0;
		rate.telnet = 0.0;
		rate.ssh = 0.0;
		rate.others = 0.0;
	}
	/* 送信用データの作成 */
	ss << WRITE_PROTOCOL << "|ARP|" << arp << "|" << rate.arp << "\n";
	ss << WRITE_PROTOCOL << "|TCP|" << tcp << "|" << rate.tcp << "\n";
	ss << WRITE_PROTOCOL << "|UDP|" << udp << "|" << rate.udp << "\n";
	ss << WRITE_PROTOCOL << "|ICMP|" << icmp << "|" << rate.icmp << "\n";
	ss << WRITE_PROTOCOL << "|DNS|" << dns << "|" << rate.dns << "\n";
	ss << WRITE_PROTOCOL << "|HTTP|" << http << "|" << rate.http << "\n";
	ss << WRITE_PROTOCOL << "|HTTPS|" << https << "|" << rate.https << "\n";
	ss << WRITE_PROTOCOL << "|FTP|" << ftp << "|" << rate.ftp << "\n";
	ss << WRITE_PROTOCOL << "|NTP|" << ntp << "|" << rate.ntp << "\n";
	ss << WRITE_PROTOCOL << "|DHCP|" << dhcp << "|" << rate.dhcp << "\n";
	ss << WRITE_PROTOCOL << "|Telnet|" << telnet << "|" << rate.telnet << "\n";
	ss << WRITE_PROTOCOL << "|SSH|" << ssh << "|" << rate.ssh << "\n";
	ss << WRITE_PROTOCOL << "|その他|" << others << "|" << rate.others << "\n";
	sdata = ss.str();		/* stringに変換 */
	data = sdata.c_str();	/* const char *型に変換 */
	n = strlen (data);		/* 書き込む長さ */
	if (flock (1, LOCK_EX) == -1) {
		perror ("flock");
		exit (1);
	}
	if (FD_ISSET (1, &fdset)) {
		if (write (1, data, n) == -1) {	/* 書き込み */
			perror ("write");
			exit (1);
		}
	}
	/* ファイル記述子をアンロックする */
	if (flock (1, LOCK_UN) == -1) {
		perror ("flock");
		exit (1);
	}
}

/* CoupleOfNodes::CoupleOfNodes()
 * コンストラクタ：メンバ変数の初期化 */
CoupleOfNodes::CoupleOfNodes(string src, string dst, unsigned int len, int now) {
	sname = src;	/* 送信元ノード名 */
	dname = dst;	/* 宛先ノード名 */
	traffic.push_back((double)len);	/* トラフィック量 */
	bytes = (unsigned long int)len;	/* 通信量 */
	start = now;	/* 開始時刻 */
}

/* CoupleOfNodes::setData()
 * バイト数を加える関数 */
void CoupleOfNodes::setData(unsigned int len, int now) {
	int index = now - start;
	if (traffic.size() > index)
		traffic[index] += (double)len;	/* トラフィック量 */
	else /* interval秒のうち最初のデータ */
		traffic.push_back((double)len);
	bytes += len;	/* 通信量 */
}

/* CoupleOfNodes::trafficData()
 * 出力用のトラフィック量データを返す関数 */
string CoupleOfNodes::trafficData(int now) {
	stringstream ss;
	ss.str("");
	int index = now - 1 - start; /* 書き込むトラフィック量の識別子 */
	if (index > -1) {
		/* エラー回避用
		 * 参照するときは必ず，データがあるようにする */
		if (traffic.size() <= index)
			traffic.push_back((double)0);

		traffic[index] /= interval; /* byte/sec に変換 */
		/* 書き込み用データの作成
		 * データ識別子,送信元名,宛先名,データ番号,トラフィック量\n */
		ss << WRITE_TRAFFIC << "|" << sname << "|" << dname
		   << "|" << now - 1 << "|" << traffic[index] << "\n";
		return ss.str();
	} else {
		return "";
	}
}

/* CoupleOfNodes::nodeData()
 * 出力用のノード組みデータを返す関数 */
string CoupleOfNodes::nodeData(int now) {
	stringstream ss;
	ss.str("");
	/* 平均のトラフィック量の計算 */
	double traf = (double)bytes/((now + 1)*interval);
	ss << WRITE_NODES << "|" << sname << "|"
	   << dname << "|" << traf << "\n";
	return ss.str();
}

/* Traffic::Traffic()
 * コンストラクタ：メンバ変数を初期化 */
Traffic::Traffic() {
	CoupleOfNodes alltraf("全体", "全体", 0, 0);
	nodev.push_back(alltraf);
	now = 0;
}

/* Traffic::setData()
 * データを加える関数
 * ノードの組みが新規ならばCoupleOfNodesオブジェクト新規作成
 * 新規でないなら対応するCoupleOfNodesオブジェクトのデータを更新 */
void Traffic::setData(string src, string dst, unsigned int len) {
	nodev[0].setData(len, now);	// 全体に書き込み
	int num = number(src, dst); /* どのデータかを識別 */
	if (num == -1) {
		/* 新規ノードの組みの場合
		 * 新しくデータを作成する */
		CoupleOfNodes newnodes(src, dst, len, now);
		nodev.push_back(newnodes);
	} else {
		/* ノードの組みが登録済みの場合
		 * データを書き込む */
		nodev[num].setData(len, now);
	}
}

/* Traffic::sendTrafficDa()
 * トラフィック量データを標準出力に出力する関数 */
void Traffic::sendTrafficData(void) {
	now++;
	string sdata("");
	const char *data;
	int n;
	int size = (int)nodev.size();
	for (int i = 0; i < size; i++) { /* すべてのノード組み */
		/* それぞれのデータを取得する */
		sdata += nodev[i].trafficData(now);
	}
	if (!sdata.empty()) { /* sdataが空でないかどうか調べる */
		data = sdata.c_str();	/* const char *型に変換 */
		n = strlen (data);		/* 書き込む長さ */
		if (flock (1, LOCK_EX) == -1) {
			perror ("flock");
			exit (1);
		}
		if (FD_ISSET (1, &fdset)) {
			if (write (1, data, n) == -1) { /* 書き込み */
				perror ("write");
				exit (1);
			}
		}
		/* ファイル記述子をアンロックする */
		if (flock (1, LOCK_UN) == -1) {
			perror ("flock");
			exit (1);
		}
	}
}

/* Traffic::sendNodeData()
 * ノードの組みデータを標準出力に出力する関数 */
void Traffic::sendNodeData(void) {
	string sdata("");
	const char *data;
	int n;
	int size = (int)nodev.size();
	for (int i = 0; i < size; i++) { /* すべてのノード組み */
		/* それぞれのデータを取得 */
		sdata += nodev.at(i).nodeData(now);
	}
	stringstream ss;
	ss.str("");
	ss << WRITE_END << "\n";
	sdata += ss.str();
	if (!sdata.empty()) { /* sdataが空でないかどうか調べる */
		data = sdata.c_str();	/* const char *型に変換 */
		n = strlen (data);		/* 書き込む長さ */
		if (flock (1, LOCK_EX) == -1) {
			perror ("flock");
			exit (1);
		}
		if (FD_ISSET (1, &fdset)) {
			if (write (1, data, n) == -1) { /* 書き込み */
				perror ("write");
				exit (1);
			}
		}
		/* ファイル記述子をアンロックする */
		if (flock (1, LOCK_UN) == -1) {
			perror ("flock");
			exit (1);
		}
	}
}

/* Traffic::number()
 * src, dst をもつCoupleOfNodesクラスの添字を返す
 * 送信元ノード名とsrc，宛先ノード名とdstが等しければその添字を返し
 * 対応するデータが存在しなければ-1を返す */
int Traffic::number(string src, string dst) {
	for (int i = 1; i < (int)nodev.size(); i++) {
		if (src == nodev.at(i).sname) { /* 送信元名比較 */
			if (dst == nodev.at(i).dname) { /* 宛先名比較 */
				return i;
			}
		}
	}
	return -1; /* データが存在しない場合 */
}
