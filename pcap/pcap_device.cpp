#include "pcap_device.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

Device::Device(pcap_if_t *interfaces)
{
	bpf_u_int32 netp, maskp;
       struct in_addr addr;
	char errbuf[PCAP_ERRBUF_SIZE];
	name = std::string("");
	detail = std::string("");
	netaddr = std::string("");
	netmask = std::string("");
	next = NULL;

	if (interfaces) {
		name = interfaces->name;	// デバイス名
		if (interfaces->description)	// デバイス詳細
			detail = interfaces->description;
		else
			detail = "";
		if (!pcap_lookupnet(interfaces->name, &netp, &maskp, errbuf)) {
			addr.s_addr = netp;
			netaddr = inet_ntoa(addr);	// ネットワークアドレス
			addr.s_addr = maskp;
			netmask = inet_ntoa(addr);	// サブネットマスク
		}
		if (interfaces->next)
			next = new Device(interfaces->next); // 次デバイス
	}
}

Device::~Device()
{
	if (next) deleteDeviceList(next);
}

void Device::deleteDeviceList(Device *dev)
{
	if (dev) delete dev;
}

std::string Device::deviceData()
{
	std::stringstream ss;
	ss.str("");
	ss << 6 << "|"<< name << "|";	// デバイス名
	ss << detail << "|";	// デバイス詳細
	ss << netaddr << "|"; // ネットワークアドレス
	ss << netmask << "\n"; // ネットマスク
	if (next)
		return ss.str() + next->deviceData();
	else return ss.str();
}
