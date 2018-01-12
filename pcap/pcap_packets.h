#ifndef INCLUDED_PCAP_PACKETS
#define INCLUDED_PCAP_PACKETS

#include "pcap_datalink.h"
#include "pcap_network.h"
#include "pcap_transport.h"
#include "pcap_application.h"
#include "pcap_statistics.h"
#include <sys/time.h>
#include <pcap.h>

class PacketCaptureHandler {
public:
       PacketCaptureHandler();
       bool openHandler(std::string filterexp);
       void closeHandler();
       void startPacketCapture();
       void breakPacketCapture();
       void packetDissect(u_char *count,
               const struct pcap_pkthdr *pkthdr,
                const u_char *rawpacket);
       void setStatisticsData(int data[], std::string src,
                            std::string dst, unsigned int len);
       void sendTrafficData();
       void sendAllStatisticsDatas();

       void setDevice(std::string name);
       int getDatalinkType();
private:
       std::string devname; // デバイス名
       int datalink;
       pcap_t *handler;     // キャプチャハンドラ
       Protocol protoData;  // プロトコル統計データ
       Traffic trafficData; // トラフィック量統計データ
};

class Packet {
       int pid;                    /* パケットID */
       bool noterr;                /* true: エラー, false: エラーなし */
       int pro[4];                 /* 層ごとのプロトコル識別 */
       long int captime;           /* 取得時刻 */
       std::string saddr;          /* 送信元IPアドレス */
       std::string daddr;          /* 宛先IPアドレス */
       std::string protocol;       /* プロトコル */
       u_int32_t len;              /* パケット長 */
       u_int32_t dlen;             /* データ長 */
       std::string detail;         /* 備考 */
       std::string datalink;       /* データリンク層詳細 */
       std::string network;        /* ネットワーク層詳細 */
       std::string transport;      /* トランスポート層詳細 */
       std::string application;    /* アプリケーション層詳細 */
public:
       Packet (int *id, const struct pcap_pkthdr *pkthdr, const u_char *raw, long int starttime);  /* コンストラクタ */
       std::string getSAddress (void);    /* 送信元アドレスを返す */
       std::string getDAddress (void);    /* 宛先アドレスを返す */
       u_int32_t getBytes (void);         /* パケット長を返す */
       int *getProtocolType (void);       /* プロトコル識別子を返す */
       void getDatalink (const u_char *raw);     /* データリンク層解析 */
       void getNetwork (const u_char *raw);      /* ネットワーク層解析 */
       void getTransport (const u_char *raw, struct pseudohdr *pseudohdr);   /* トランスポート層解析 */
       void getApplication (const u_char *raw);  /* アプリケーション層解析 */
       std::string rawData (const u_char *raw);  /* 生パケットデータ作成 */
       std::string packetData (std::string raw); /* パケット表示用データ作成 */
};

void abortCapture (int sig_num);   /* シグナル捕獲関数：パケットキャプチャ終了指示 */
void saveTrafficData(int sig_num); /* シグナル捕獲関数：30秒タイマー */
#endif
