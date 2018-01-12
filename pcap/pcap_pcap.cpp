#include "pcap_packets.h"
#include "pcap_statistics.h"
#include <netinet/if_ether.h>
#include <net/ethernet.h>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/file.h>

using namespace std;

extern PacketCaptureHandler *pcap;
extern fd_set fdset;

long int start_sec;
long int start_usec;

/* abortCapture関数
 * シグナル : SIGINT, SIGTERM
* シグナル捕獲関数であり，パケットキャプチャを終了し，統計データを出力する */
void abortCapture (int sig_num) {
       pcap->sendAllStatisticsDatas();
       pcap->breakPacketCapture();
}

/* saveTrafficData関数
 * シグナル : SIGALRM
 * シグナル捕獲関数であり，トラフィック量データを標準出力に出力する */
void saveTrafficData (int sig_num) {
       pcap->sendTrafficData();
//       trafficData.sendTrafficData(); /* トラフィック量データの出力 */
       /* シグナルの再設定 */
       if (signal (SIGALRM, saveTrafficData) == SIG_ERR) {
              perror ("signal");
              exit (1);
       }
       alarm (interval); /* 30秒のタイマーを設定 */
}

/* pcap_capture()関数
 * pcap_loop(), pcap_dispatch()により呼び出されるコールバック関数
 * @count :パケット数のカウンタ
 * @pkthdr :パケットの取得時間，パケット長の構造体
 * @packet :パケットデータ
 * 返り値なし
 */
void pcap_capture(u_char *count,
       const struct pcap_pkthdr *pkthdr, const u_char *rawpacket)
{
       int *counter = (int *)count;       // パケット数
       (*counter)++; // カウンタ+1
       std::string sdata;   // 送信データ
       std::string rawdata; // 生データ
       struct timeval tv;   // selectの待ち時間
       tv.tv_sec = 1;
       tv.tv_usec = 0;
       if (*counter == 1) { // 最初のパケットの場合
              start_sec = pkthdr->ts.tv_sec;
              start_usec = pkthdr->ts.tv_usec;
              if (signal(SIGALRM, saveTrafficData) == SIG_ERR) {
                     /* SIGALRMを受け取ったら，
                     * saveTrafficData関数を呼び出すように設定 */
                     perror ("signal");
                     exit(1);
              }
              alarm (interval); // intervalのタイマー設定
       }
       /* パケットの新オブジェクト */
       Packet *packet = new Packet(counter, pkthdr, rawpacket, start_sec);
       /* 統計データの更新 */
       int *prodata = packet->getProtocolType();
       pcap->setStatisticsData(prodata, packet->getSAddress(),
                     packet->getDAddress(), packet->getBytes());
       /* パケット一覧用データを作成 */
       rawdata = packet->rawData (rawpacket);
       sdata = packet->packetData (rawdata);
       /* 書き込み用ファイル記述子の監視 */
       if (select(2, NULL, (fd_set *)&fdset, NULL, &tv) == -1) {
              perror("select");
              exit(1);
       }
       /* 書き込み可能かどうかをチェック */
       if (FD_ISSET(1, &fdset)) {
              /* ファイル記述子をロックする */
              if (flock(1, LOCK_EX) == -1) {
                     perror("flock");
                     exit(1);
              }
              if (FD_ISSET(1, &fdset)) {
                     /* 標準出力に出力する */
                     if (write (1, sdata.c_str(), sdata.size()) == -1) {
                            perror ("write");
                            exit (1);
                     }
              }
              /* ファイル記述子をアンロックする */
              if (flock(1, LOCK_UN) == -1) {
                     perror("flock");
                     exit(1);
              }
       }
       /* newで生成したら必ず解放する */
       if (packet) delete packet;
}

PacketCaptureHandler::PacketCaptureHandler()
{
}

void PacketCaptureHandler::setDevice(std::string name)
{
       devname = name;
}

int PacketCaptureHandler::getDatalinkType()
{
       return datalink;
}

bool PacketCaptureHandler::openHandler(std::string filterexp)
{
       char errbuf[PCAP_ERRBUF_SIZE];
       bpf_u_int32 netp, maskp;
       // ハンドラの生成
       handler = pcap_open_live (devname.c_str(), BUFSIZ, 1, 0, errbuf);
       if (handler) { // ハンドラ生成成功
              datalink = pcap_datalink(handler); // データリンクタイプを取得
              // サブネットマスクを取得してフィルタの設定
              if (!pcap_lookupnet(devname.c_str(), &netp, &maskp, errbuf)) {
                     struct bpf_program filter;  // フィルタプログラム
                     pcap_compile (handler, &filter, filterexp.c_str(), 1, maskp);
                     pcap_setfilter (handler, &filter); // フィルタの指定
                     return true;
              }
       }
       return false; // エラーがあった場合は false
}

void PacketCaptureHandler::closeHandler()
{
       if (handler)
              pcap_close (handler);       // ハンドラをクローズ
}

void PacketCaptureHandler::startPacketCapture()
{
       // パケットキャプチャを開始
       if (handler) {
              int count = 0;
              /* int pcap_loop(pcap_t *p, int cnt, pcap_handler callback, u_char *user)関数
              * 指定された数だけパケットをキャプチャする関数
              * @p : パケットハンドラ
              * @cnt : キャプチャするパケットの数
              * @callback : パケット取得時に呼び出されるコールバック関数
              * @user : パケットを格納するポインタ
              * 返り値 : cnt分パケットを取得した場合 -> 0
              *         エラーが生じた場合 -> -1
              *         pcap_breakloop()で終了した場合 -> -2 */
              int result = pcap_loop(handler, 0, pcap_capture, (u_char *)&count);
       }
}

void PacketCaptureHandler::breakPacketCapture()
{
       pcap_breakloop(handler);    // パケットキャプチャを終了
}

void PacketCaptureHandler::setStatisticsData(int data[],
        std::string src, std::string dst, unsigned int len)
{
       protoData.setProtocolData(data);
       trafficData.setData(src, dst, len);
}

void PacketCaptureHandler::sendTrafficData()
{
       trafficData.sendTrafficData();     // トラフィック量データの出力
}

void PacketCaptureHandler::sendAllStatisticsDatas()
{
       protoData.sendProtocolData(); // プロトコルデータの出力
       trafficData.sendNodeData(); // ノードの組みデータの出力
}
