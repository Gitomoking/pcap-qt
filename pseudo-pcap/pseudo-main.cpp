#include "pseudo-pcap.h"
#include <time.h>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <signal.h>

using namespace std;

int id;

fd_set fdset;

Protocol protoData;
Traffic trafficData;

void abortCapture(int sig_num){
	protoData.sendProtocolData();
	trafficData.sendNodeData();
	exit(0);
}

void packetCapture(int sig_num)
{
	double t = (double)id - 1;
	Packet packet(id, t);

	int *prodata = packet.getProtocolType();
	string src = packet.getSAddress();
	string dst = packet.getDAddress();
	int len = packet.getBytes();

	protoData.setProtocolData(prodata);
	trafficData.setData(src, dst, len);
	string sdata = packet.packetData();
	write(1, sdata.c_str(), sdata.size());
	id++;
	if (id%10 == 0)
		trafficData.sendTrafficData();

	if (signal(SIGALRM, packetCapture) == SIG_ERR) {
		exit(1);
	}
	alarm(1);
}


int main()
{
	id = 1;
	FD_ZERO(&fdset);
	FD_SET(1, &fdset);
	if (signal(SIGINT, abortCapture) == SIG_ERR) {
		exit(1);
	}
	if (signal(SIGTERM, abortCapture) == SIG_ERR) {
		exit(1);
	}
	if (signal(SIGALRM, packetCapture) == SIG_ERR) {
		exit(1);
	}
	alarm (1);
	while(1){

	}
}
