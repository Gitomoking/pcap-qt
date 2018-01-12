#include "pcap_device.h"
#include <unistd.h>	// write

int main()
{
	pcap_if_t *interfaces;
	char errbuf[PCAP_ERRBUF_SIZE];
	std::string sdata;	// 送信データ

	// デバイスリストを取得
	pcap_findalldevs(&interfaces, errbuf);
	if (interfaces) {
		// デバイスクラスを生成
		Device *dev = new Device(interfaces);
		if (dev) {
			sdata = dev->deviceData(); // 送信データ
			if (sdata.size()) {
				// デバイスデータを送信
				write(1, sdata.c_str(), sdata.size());	// 標準出力に書き込み
			}
			delete(dev);	// デバイスリストを削除
			pcap_freealldevs(interfaces);
			return 0;
		} else	// デバイスリストを削除
			pcap_freealldevs(interfaces);
	}
	std::string err("デバイスリストを取得できませんでした\n");
	write(1, err.c_str(), err.size());	// エラーメッセージ出力
	return 1;
}
