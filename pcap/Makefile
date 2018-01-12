CC	= g++
LIBS	= -lpcap
OBJSP	= pcap_main.o pcap_pcap.o pcap_packet.o pcap_ieee802_3.o pcap_ip.o pcap_arp.o pcap_tcp.o pcap_udp.o pcap_icmp.o pcap_dns.o pcap_http.o pcap_statistics.o
OBJSD	= pcap_device.o device_main.o
TARGETP	= pcap-capture
TARGETD	= pcap-device

all:	$(TARGETP) $(TARGETD)

pcap-device:	$(OBJSD)
	$(CC) $(OBJSD) $(LIBS) -o $(TARGETD)
device_main.o:	device_main.cpp
	$(CC) -c device_main.cpp
pcap_device.o:	pcap_device.cpp
	$(CC) -c pcap_device.cpp
pcap-capture: 	$(OBJSP)
	$(CC) $(OBJSP) $(LIBS) -o $(TARGETP)
pcap_main.o: 	pcap_main.cpp
	$(CC) -c pcap_main.cpp
pcap_pcap.o:	pcap_pcap.cpp
	$(CC) -c pcap_pcap.cpp
pcap_packet.o:	pcap_packet.cpp
	$(CC) -c pcap_packet.cpp
pcap_ieee802_3.o:	pcap_ieee802_3.cpp
	$(CC) -c pcap_ieee802_3.cpp
pcap_ip.o:	pcap_ip.cpp
	$(CC) -c pcap_ip.cpp
pcap_arp.o:	pcap_arp.cpp
	$(CC) -c pcap_arp.cpp
pcap_tcp.o:	pcap_tcp.cpp
	$(CC) -c pcap_tcp.cpp
pcap_udp.o:	pcap_udp.cpp
	$(CC) -c pcap_udp.cpp
pcap_icmp.o:	pcap_icmp.cpp
	$(CC) -c pcap_icmp.cpp
pcap_dns.o:	pcap_dns.cpp
	$(CC) -c pcap_dns.cpp
pcap_http.o:	pcap_http.cpp
	$(CC) -c pcap_http.cpp
pcap_statistics.o:	pcap_statistics.cpp
	$(CC) -c pcap_statistics.cpp

clean:
	rm -f *.o
