#include "capture_net_packet.h"
#include "structs.h"
#include "flags.h"

using namespace log4cxx;

LoggerPtr CaptureNetPacket::logger_(Logger::getLogger("cap_net_pack"));

void CaptureNetPacket::DispatchCapData(zmq::socket_t * sock, void * data, int size)
{
	assert(NULL != sock && NULL != data);
	try
	{
		zmq::message_t msg(size);
		memcpy((void*)(msg.data()), data, size); 
		//sock->send(msg,ZMQ_NOBLOCK);
		sock->send(msg);
	}
	catch(zmq::error_t error)
	{
		//cout<<"cap: zmq send error! error content:"<<error.what()<<endl;
		LOG4CXX_ERROR(logger_, "cap: zmq send error! error content:" << error.what());
		assert(0);
	}
}

void CaptureNetPacket::Init()
{
	InitZMQ();
	LOG4CXX_INFO(logger_, "cap inited");	
}

void CaptureNetPacket::InitZMQ()
{
	assert(-1 != this->zmqitems_[0].zmqpattern);
	sock_ = new zmq::socket_t(*context_,this->zmqitems_[0].zmqpattern);
    if("bind" == this->zmqitems_[0].zmqsocketaction)
    {
        sock_->bind(this->zmqitems_[0].zmqsocketaddr.c_str());
    }
    else if("connect" == this->zmqitems_[0].zmqsocketaction)
    {
        sock_->connect(this->zmqitems_[0].zmqsocketaddr.c_str());
    }
}

void CaptureNetPacket::RunThreadFunc()
{
    pcap_if_t * alldevs ;
    char *errbuf= new char[PCAP_ERRBUF_SIZE];
    int len=10240;
    int mode= 1;
    int timeout=1000;
    pcap_addr_t *a;
    struct bpf_program fcode;
    pcap_if_t *p_dev;
    pcap_t *adhandle;
    unsigned int netmask = 0;
	if(pcap_findalldevs(&alldevs,errbuf) == -1)
	{
		//cout<<"Error in pcap_findalldevs %s "<<errbuf<<endl;
		LOG4CXX_ERROR(logger_, "Error in pcap_findalldevs %s " << errbuf);
		return ; 
	}
	
	for(p_dev=alldevs; p_dev; p_dev=p_dev->next)
	{	
		std::string net_adapter(p_dev->name);
		if(net_adapter.compare(adapter_id_) == 0)
		{
			break;
		}
	}	
	assert(NULL != p_dev);
	if((adhandle = pcap_open_live(p_dev->name,len,mode,timeout,errbuf))==NULL)
	{
		//cout<<"Unable to open the adapter! "<<p_dev->name<<" is not supported by libpcap or winpcap!"<<endl;
		LOG4CXX_ERROR(logger_, "Unable to open the adapter! " << p_dev->name << " is not supported by libpcap or winpcap!");
		pcap_freealldevs(alldevs);
		return ; 
	}
	else
	{
		/* Check the link layer. We support only Ethernet for simplicity. */
		if(pcap_datalink(adhandle) != DLT_EN10MB)
		{
			//cout<<"This program works only on Ethernet networks!"<<endl;
			LOG4CXX_ERROR(logger_, "This program works only on Ethernet networks!");
			pcap_freealldevs(alldevs);
			return ;
		}

		for(a=p_dev->addresses; a; a=a->next)
		{
			if(a->addr->sa_family == AF_INET)
			{
				netmask = ((struct sockaddr_in *)a->netmask)->sin_addr.s_addr;
				break;
			}
		}
	}

	//compile the filter
	if(pcap_compile(adhandle,&fcode,filter_.c_str(),1,netmask) < 0)
	{
		//cout<<"Unable to compile the packet filter. Check the syntax."<<endl;
		LOG4CXX_ERROR(logger_, "Unable to compile the packet filter. Check the syntax.");
		pcap_freealldevs(alldevs);
		return ;
	}

	//set the filter
	if(pcap_setfilter(adhandle, &fcode)<0)
	{
		//cout<<"Error setting the filter!"<<endl;
		LOG4CXX_ERROR(logger_, "Error setting the filter!");

		pcap_freealldevs(alldevs);
		return ;
	}
	
	//dump file
	//pcap_dumper_t *dumper;
	//dumper = pcap_dump_open(adhandle,"dumper.txt");
	//if(dumper == NULL)
	//{
	//	cout<<"can't open dumper filer~!"<<endl;
	//}

	delete [] errbuf;
	errbuf = NULL;
	pcap_freealldevs(alldevs);

	if(pcap_loop(adhandle,0,PacketHandler,(unsigned char*)sock_) < 0)
	{
		//cout<<"pcap_loop:"<<pcap_geterr(adhandle)<<endl;
		LOG4CXX_ERROR(logger_, "pcap_loop:" << pcap_geterr(adhandle));
	}
	
	//pcap_loop(adhandle,0,PacketHandler,(u_char *)dumper);
	//pcap_dump_close(dumper);
}

void CaptureNetPacket::PacketHandler(unsigned char *param, const struct pcap_pkthdr *header, const unsigned char *pkt_data)
{
	zmq::socket_t *sock = (zmq::socket_t *)param;
	CapNetPacketItem item;
	item.header = *header;
	unsigned char *pdata;
	if(CAP_PACK_BUF_SIZE < header->caplen)
	{
		pdata = new unsigned char[header->caplen];
		memset(pdata, 0, header->caplen);
		LOG4CXX_INFO(logger_, "cap buffer len:" << header->caplen);
	}
	else
	{
		pdata = new unsigned char[CAP_PACK_BUF_SIZE];
		memset(pdata, 0, CAP_PACK_BUF_SIZE);
		//LOG4CXX_INFO(logger_, "cap buffer len:" << CAP_PACK_BUF_SIZE);
	}
	memcpy(pdata,pkt_data,header->caplen);
	item.data = pdata;
	DispatchCapData(sock, &item, sizeof(item));
}

