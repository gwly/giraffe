#ifndef MONITOR_CAPTURE_NET_PACKET_H
#define MONITOR_CAPTURE_NET_PACKET_H

#include "basethread.h"
#include <log4cxx/logger.h>


class CaptureNetPacket: public BaseThread
{
public:
	CaptureNetPacket(std::string adapter_id, std::string filter, zmq::context_t *context):adapter_id_(adapter_id),filter_(filter),context_(context)
	{
		sock_ = NULL;
	};
	~CaptureNetPacket()
	{
		if(NULL != sock_)
		{
			delete sock_;
			sock_ = NULL;
		}
	};
	void RunThreadFunc();
	void Init();
private:
	void InitZMQ();
	static void DispatchCapData(zmq::socket_t * sock, void * data, int size);
	static void PacketHandler(unsigned char*param, const struct pcap_pkthdr *header, const unsigned char *pkt_data);
	static log4cxx::LoggerPtr logger_;
	zmq::socket_t *sock_;
	std::string adapter_id_;
	std::string filter_;	
	zmq::context_t *context_;
};
#endif
