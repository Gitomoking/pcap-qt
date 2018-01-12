#include "pcap_statistics.h"

class Packet {
       int pid;                    /* パケットID */
       bool noterr;                /* true: エラーなし, false: エラー */
       int pro[4];                 /* 層ごとのプロトコル識別 */
       double captime;           /* 取得時刻 */
       std::string saddr;          /* 送信元IPアドレス */
       std::string daddr;          /* 宛先IPアドレス */
       std::string protocol;       /* プロトコル */
       u_int32_t len;              /* パケット長 */
       std::string detail;         /* 備考 */
       std::string datalink;       /* データリンク層詳細 */
       std::string network;        /* ネットワーク層詳細 */
       std::string transport;      /* トランスポート層詳細 */
       std::string application;    /* アプリケーション層詳細 */
public:
       Packet (int id, double catime);  /* コンストラクタ */
       std::string getSAddress (void);    /* 送信元アドレスを返す */
       std::string getDAddress (void);    /* 宛先アドレスを返す */
       u_int32_t getBytes (void);         /* パケット長を返す */
       int *getProtocolType (void);       /* プロトコル識別子を返す */
       std::string packetData (void); /* パケット表示用データ作成 */
};
