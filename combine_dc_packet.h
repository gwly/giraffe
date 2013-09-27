#ifndef MONITOR_COMBINE_DC_PACKET_H
#define MONITOR_COMBINE_DC_PACKET_H 
#include <log4cxx/logger.h>
#include "basethread.h"
#include "zmq.hpp"

class CombineDCPacket:public BaseThread
{
public:
	CombineDCPacket(zmq::context_t *context):context_(context)
	{
		sock_recv_ = NULL;
		sock_send_ = NULL;
		packet_item_.data = NULL;
		last_pack_len_ = 0;
		last_temp_len_ = 0;
		long_pack_tag_ = 0;
		case2_tag_  = false;
		dc_header_last_inner_len_ = 0;
		last_tcp_seq_ = 0;
		dc_header_ = new unsigned char [DC_HEAD_LEN];
		recombined_header_buf_ = new unsigned char [PCAPTOPARSE_BUF_SIZE];
	};
	~CombineDCPacket()
	{
		if(NULL != sock_recv_)
		{
			delete sock_recv_;
			sock_recv_ = NULL;
		}
		if(NULL != sock_send_)
		{
			delete sock_send_;
			sock_send_ = NULL;
		}
		if (NULL != dc_header_)
		{
			delete [] dc_header_;
			dc_header_ = NULL;
		}
		if(NULL != recombined_header_buf_)
		{
			delete [] recombined_header_buf_;
			recombined_header_buf_ = NULL;
		}
	};
	void *RunThreadFunc();
	void Init();
	bool IsDCType(int dc_type);
	bool IsDCHeader(unsigned char * dc_header);
	void DispatchData(zmq::socket_t * sock, void * data, int size);
	void Combine(struct pcap_pkthdr header, unsigned char * pkt_base_addr, int pre_dc_offset, int dc_len);//dc is dc_header+dc_data
private:
	zmq::context_t * context_;
	zmq::socket_t * sock_recv_;
	zmq::socket_t * sock_send_;
    PacketItem packet_item_;
    int dc_header_last_inner_len_;
    int long_pack_tag_;
    int last_pack_len_;
    int last_temp_len_;
    bool case2_tag_;
    unsigned long last_tcp_seq_;
    unsigned char *dc_header_;
    unsigned char *recombined_header_buf_;
	static log4cxx::LoggerPtr logger_;
};




#endif
