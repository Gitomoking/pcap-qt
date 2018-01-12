#include "pcap_packets.h"
#include <cstdlib>
#include <cstring>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/unistd.h>
#include <signal.h>
#include <iostream>

PacketCaptureHandler *pcap;
fd_set fdset;

void usage()
{
       std::cout << "引数にネットワークデバイス名を入力してください" << std::endl;
       std::cout << "形式：デバイス名 フィルタ式 フィルタ式 フィルタ式 ..." << std::endl;
}

int main (int argc, char *argv[]) {
       std::string devname;
       std::string filterexp("");

       if (argc > 1) {
              devname = std::string(argv[1]);
       } else {
              usage();
              exit(1);
       }

       for (int i = 2; i < argc; i++) {
              filterexp += std::string(argv[i]);  /* フィルタ式 */
              filterexp += " ";
       }

       // 監視用ファイル記述子の集合設定
       FD_ZERO(&fdset);
       FD_SET(1, &fdset);

       if (signal (SIGINT, abortCapture) == SIG_ERR) {
		/* 親プロセスがSIGINTを送ったら，
		 * abortCapture関数を呼び出すように設定 */
		 /* エラー処理 */
		 perror ("signal");
		 exit (1);
	}

       if (signal (SIGTERM, abortCapture) == SIG_ERR) {
		/* 親プロセスがSIGKILLを送ったら，
		 * abortCapture関数を呼び出すように設定 */
		 perror ("signal");
		 exit (1);
	}

       pcap = new PacketCaptureHandler;
       pcap->setDevice(devname);
       if (pcap->openHandler(filterexp)) {
              pcap->startPacketCapture();
              pcap->closeHandler();
              delete pcap;
              exit(0);
       } else {
              delete pcap;
              // ハンドラを開けない場合
              std::string err("パケットキャプチャハンドラを開けませんでした : ");
              err += devname;
              write(1, err.c_str(), err.size()); // エラーメッセージを出力
              exit(1);
       }
}
