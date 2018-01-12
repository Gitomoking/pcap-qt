CC	= g++
OBJS	= pcap_statistics.o pseudo-pcap.o pseudo-main.o
TARGET	= pseudo-capture

all:	$(TARGET)

pseudo-capture:	$(OBJS)
	$(CC) $(OBJS) -o $(TARGET)
pseudo-main.o:	pseudo-main.cpp
	$(CC) -c pseudo-main.cpp
pseudo-pcap.o:	pseudo-pcap.cpp
	$(CC) -c pseudo-pcap.cpp
pcap_statistics.o:	pcap_statistics.cpp
	$(CC) -c pcap_statistics.cpp

clean:
	rm -f *.o
