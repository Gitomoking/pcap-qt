#ifndef INCLUDED_PCAP_DEVICE
#define INCLUDED_PCAP_DEVICE

#include <pcap.h>
#include <string>
#include <sstream>

class Device
{
public:
       Device(pcap_if_t *interfaces);
       ~Device();
       std::string deviceData(); // 標準出力に出力
private:
       void deleteDeviceList(Device *dev); // リストの削除
       std::string name;    // デバイス名
       std::string detail;  // デバイスの説明
       std::string netaddr; // ネットワークアドレス
       std::string netmask; // サブネットマスク
       Device *next; // 次デバイス
};

#endif
