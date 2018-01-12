#include "pcap_application.h"
using namespace std;

/* コンストラクタ：メンバ変数にデータ格納 */
HTTPPacket::HTTPPacket (const u_char *raw) {
	stringstream ss;
	ss.str("");
	this->pdetail = ""; /* パケット備考を初期化する */
	this->adetail = "[HTTP]\n"; /* アプリケーション層詳細を初期化する */
	int now = 0;
	/* \r\n が2回連続で取得されるまで，すなわちHTTPメッセージの終了まで繰り返す */
	while (!((raw[now] == 0x0d) && (raw[now + 1] == 0x0a))) {
		/* 1行取得する */
		while (!((raw[now] == 0x0d) && (raw[now + 1] == 0x0a))) {
			ss << raw[now];
			now++;
		}
		/* 最初の行ならば，パケットの備考に格納する */
		if (this->pdetail.empty())
			this->pdetail = ss.str();
		ss << "\n";
		now += 2; /* \n分(1) と次ポインタ */
	}
	/* HTTPメッセージをアプリケーション層詳細に格納する */
	this->adetail += ss.str();
}

/* getPacketDetail()関数：パケットの備考を返す関数 */
string HTTPPacket::getPacketDetail (void) {
	return this->pdetail;
}

/* getApplicationDetail()関数：アプリケーション層詳細を返す関数 */
string HTTPPacket::getApplicationDetail (void) {
	return this->adetail;
}
