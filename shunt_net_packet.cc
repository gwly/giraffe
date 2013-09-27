#include "shunt_net_packet.h"
#include <map>
#include "flags.h"
#include "utils.h"
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <csignal>

using namespace log4cxx;

LoggerPtr ShuntNetPacket::logger_(Logger::getLogger("shunt_net_pack"));

pthread_key_t pthread_key_shunt;
pthread_once_t pthread_once_shunt = PTHREAD_ONCE_INIT;

//int count_pack = 0;
//struct itimerval tick;
//
//void PrintCountInfo(int signo)
//{
//  switch(signo)
//  {
//      case SIGALRM:
//          cout<<"combine pack count:"<<count_pack<<endl;
//          count_pack = 0;
//          break;
//      default:
//          break;
//  }
//  return ;
//}
//

void CreateThreadKey()
{
	pthread_key_create(&pthread_key_shunt, NULL);
}

void ShuntNetPacket::Init()
{
	sock_ = new zmq::socket_t (*context_, this->zmqitems_[0].zmqpattern);
	//sock_->setsockopt(ZMQ_RCVHWM, &ZMQ_RCVHWM_SIZE, sizeof(ZMQ_RCVHWM_SIZE));
	if("bind" == this->zmqitems_[0].zmqsocketaction)
	{
		sock_->bind(this->zmqitems_[0].zmqsocketaddr.c_str());
	}
	else if("connect" == this->zmqitems_[0].zmqsocketaction)
	{
		sock_->connect(this->zmqitems_[0].zmqsocketaddr.c_str());
	}
	IncreasePool(kPoolSize);
	//cout<<"complete the initialization!"<<endl;
	LOG4CXX_INFO(logger_, "complete the initialization of shunting part!");
}

void ShuntNetPacket::InitZMQEx(int index)
{
	//init ShuntNetPacket zmq property from config file
	deque<XML_ZMQ> *zmq_deque = listening_item_.get_shunt_net_packet()->get_zmqdeque();
	deque<XML_ZMQ>::iterator iter=zmq_deque->begin();
	for(++iter;iter!=zmq_deque->end();iter++)
	{
		ZMQItem zmq_item;
		zmq_item.zmqpattern = (*iter).get_zmqpattern();
		zmq_item.zmqsocketaction = (*iter).get_zmqsocketaction();
		char buf[16];
		memset(buf,0,sizeof(buf));
		sprintf(buf,"%d",index);
		zmq_item.zmqsocketaddr = (*iter).get_zmqsocketaddr() + buf;
		AddZMQItemEx(zmq_item);
		break;
	}
}

void ShuntNetPacket::AddToZMQDequeEx(int index)
{
	//init cap socket
	zmq::socket_t *sock = new zmq::socket_t (*context_,this->zmqitems_ex_[index].zmqpattern);
	if("bind" == this->zmqitems_ex_[index].zmqsocketaction)
	{
		sock->bind(this->zmqitems_ex_[index].zmqsocketaddr.c_str());
	}
	else if("connect" == this->zmqitems_ex_[index].zmqsocketaction)
	{
		sock->connect(this->zmqitems_ex_[index].zmqsocketaddr.c_str());
	}
	sock_ex_deque_.push_back(sock);
}

void ShuntNetPacket::RunLuaRoutineThread(int index)
{
	//init lua routine zmq property from config file
	LuaRoutine* lua_routine = new LuaRoutine(context_,listening_item_);
	deque<XML_ZMQ>* lua_routine_zmq_deque = listening_item_.get_lua_routine()->get_zmqdeque();
	for(deque<XML_ZMQ>::iterator iter = lua_routine_zmq_deque->begin();iter!=lua_routine_zmq_deque->end();iter++)
	{
		ZMQItem lua_routine_zmq_item;
		lua_routine_zmq_item.zmqpattern = (*iter).get_zmqpattern();
		lua_routine_zmq_item.zmqsocketaction = (*iter).get_zmqsocketaction();
		//TO FIX 33333
		if("tcp://localhost:33333" == (*iter).get_zmqsocketaddr())
		{
			lua_routine_zmq_item.zmqsocketaddr = (*iter).get_zmqsocketaddr();
		}
		else
		{
			char buf[16];
			memset(buf,0,sizeof(buf));
			sprintf(buf,"%d",index);
			lua_routine_zmq_item.zmqsocketaddr = (*iter).get_zmqsocketaddr() + buf;
		}
		lua_routine->AddZMQItem(lua_routine_zmq_item);
	}
	lua_routine->Init();
	lua_routine->Start();
	lua_routine_deque_.push_back(lua_routine);
}

void ShuntNetPacket::RunCombineDCPacketThread(int index)
{
	CombineDCPacket* combine_dc_packet= new CombineDCPacket(context_);
	deque<XML_ZMQ>* combine_dc_zmq_deque = listening_item_.get_combine_dc_packet()->get_zmqdeque();
	for(deque<XML_ZMQ>::iterator iter = combine_dc_zmq_deque->begin();iter!=combine_dc_zmq_deque->end();iter++)
	{
		ZMQItem combine_dc_zmq_item;
		combine_dc_zmq_item.zmqpattern = (*iter).get_zmqpattern();
		combine_dc_zmq_item.zmqsocketaction = (*iter).get_zmqsocketaction();
		char buf[16];
		memset(buf,0,sizeof(buf));
		sprintf(buf,"%d",index);
		combine_dc_zmq_item.zmqsocketaddr = (*iter).get_zmqsocketaddr() + buf;
		combine_dc_packet->AddZMQItem(combine_dc_zmq_item);
	}
	combine_dc_packet->Init();
	combine_dc_packet->Start();
	combine_dc_deque_.push_back(combine_dc_packet);
}

void ShuntNetPacket::RunUncompressDCPacketThread(int index)
{
	UncompressDCPacket* uncompress_dc_packet= new UncompressDCPacket(context_ , listening_item_);
	deque<XML_ZMQ>* uncompress_dc_zmq_deque = listening_item_.get_uncompress_dc_packet()->get_zmqdeque();
	for(deque<XML_ZMQ>::iterator iter = uncompress_dc_zmq_deque->begin();iter!=uncompress_dc_zmq_deque->end();iter++)
	{
		ZMQItem uncompress_dc_zmq_item;
		uncompress_dc_zmq_item.zmqpattern =(*iter).get_zmqpattern();
		uncompress_dc_zmq_item.zmqsocketaction = (*iter).get_zmqsocketaction();
		if("inproc://log" == (*iter).get_zmqsocketaddr())
		{
			uncompress_dc_zmq_item.zmqsocketaddr = (*iter).get_zmqsocketaddr();
		}
		else
		{
			char buf[16];
			memset(buf,0,sizeof(buf));
			sprintf(buf,"%d",index);
			uncompress_dc_zmq_item.zmqsocketaddr = (*iter).get_zmqsocketaddr() + buf;
		}
		uncompress_dc_packet->AddZMQItem(uncompress_dc_zmq_item);
	}
	uncompress_dc_packet->Init();
	uncompress_dc_packet->Start();
	uncompress_dc_deque_.push_back(uncompress_dc_packet);
}

// void ShuntNetPacket::RunParseThread(int index)
// {
// 	//init parse zmq property from config file
// 	Parse* parse = new Parse(context_ , listening_item_);
// 	deque<XML_ZMQ>* parse_zmq_deque = listening_item_.get_parse()->get_zmqdeque();
// 	for(deque<XML_ZMQ>::iterator iter = parse_zmq_deque->begin();iter!=parse_zmq_deque->end();iter++)
// 	{
// 		ZMQItem parse_zmq_item;
// 		parse_zmq_item.zmqpattern =(*iter).get_zmqpattern();
// 		parse_zmq_item.zmqsocketaction = (*iter).get_zmqsocketaction();
// 		if("inproc://log" == (*iter).get_zmqsocketaddr())
// 		{
// 			parse_zmq_item.zmqsocketaddr = (*iter).get_zmqsocketaddr();
// 		}
// 		else
// 		{
// 			char buf[16];
// 			memset(buf,0,sizeof(buf));
// 			sprintf(buf,"%d",index);
// 			parse_zmq_item.zmqsocketaddr = (*iter).get_zmqsocketaddr() + buf;
// 		}
// 		parse->AddZMQItem(parse_zmq_item);
// 	}
// 	parse->Init();
// 	parse->Start();
// 	parse_deque_.push_back(parse);
// }

void set_inner_thread_params(const void * value)
{
	pthread_once(&pthread_once_shunt, CreateThreadKey);
	pthread_setspecific(pthread_key_shunt, value);
}

void * get_inner_thread_params()
{
	return pthread_getspecific(pthread_key_shunt);
}

//void ShuntNetPacket::ListenTcpConnection()
//{
//
//}
//
//void ShuntNetPacket::ListenTcpDisconnction()
//{
//
//}

bool ShuntNetPacket::IncreasePool(int pool_size)
{
	for(int i=curent_pool_size_;i<curent_pool_size_ + pool_size;i++)
	{
		InitZMQEx(i);
		AddToZMQDequeEx(i);
#ifndef __linux
		Sleep(100);
#else
		Utils::SleepUsec(10000);
#endif
		RunLuaRoutineThread(i);
		//RunParseThread(i);
		RunCombineDCPacketThread(i);
#ifndef __linux
		Sleep(100);
#else
		Utils::SleepUsec(10000);
#endif
		RunUncompressDCPacketThread(i);
	}
	curent_pool_size_ += pool_size;
	return true;
}

void ShuntNetPacket::CreateDidConfContent(vector<DidStruct> & did_structs, char * out_str)
{
	assert(NULL != out_str);
	char did_content[256] = {0};
	strcat(out_str, "<?xml version=\"1.0\" encoding=\"gb2312\" ?>\n <DidStruct>\n");
	for(vector<DidStruct>::iterator iter = did_structs.begin();iter != did_structs.end();iter++)
	{
		memset(did_content, 0,sizeof(did_content));
		sprintf(did_content, "\t <did id=\"%d\" file=\"%s\" whole=\"%u\" compress=\"%u\" /> \n", iter->id ,iter->file_path.c_str(),iter->whole_tag,iter->compress_tag);
		strcat(out_str, did_content);
	}
	strcat(out_str, "</DidStruct>\n");
}

//void WriteIntoFile(const char *file_name, const char *mode, const void* data , size_t length)
//{
//	FILE * fp = fopen(file_name, mode);
//	if(NULL != fp)
//	{
//		fwrite(data, 1, length, fp);
//		fclose(fp);
//	}
//	else
//	{
//		cout<<"open file error!"<<endl;
//		assert(0);
//	}
//}

void ShuntNetPacket::WriteDidConfFile(const char * file_name, vector<DidStruct> &did_structs)
{
	char file_content[2048] = {0};
	CreateDidConfContent(did_structs, file_content);
	Utils::WriteIntoFile(file_name, "wb", file_content, strlen(file_content));
}

bool ShuntNetPacket::IsTcpConnection(unsigned char flags, int &tcpconntag, int &tcpconnstatus)
{
	if(SYN == flags)
	{
		tcpconntag = 1;
		tcpconnstatus = 0;
	}
	if(1 == tcpconntag)
	{
		if(SYN == flags)
		{
			tcpconnstatus |= 0x1;
		}
		else if(SYNACK == flags)
		{
			tcpconnstatus |= 0x2;
		}
		else if(ACK == flags)
		{
			tcpconnstatus |= 0x4;
			tcpconntag = 0;
			if(7 == tcpconnstatus)
			{
				tcpconnstatus = 0;
				return true;
			}
			else
			{
				tcpconnstatus = 0;
			}
        }
		else
		{
			tcpconntag = 0;
			tcpconnstatus = 0;
		}
	}
	return false;
}

bool ShuntNetPacket::IsTcpDisConnection(unsigned char flags)
{
	if(FINACK == flags)
	{
		return true;
	}
	return false;
}

void ShuntNetPacket::DispatchData(zmq::socket_t * sock, void * data, int size)
{
	assert(NULL != sock && NULL != data);
	try
	{
		zmq::message_t msg(size);
		memcpy((void*)(msg.data()), data, size);
		//sock->send(msg,ZMQ_NOBLOCK);
		int ret = sock->send(msg);
	}
	catch(zmq::error_t error)
	{
		//cout<<"cap: zmq send error! error content:"<<error.what()<<endl;
		LOG4CXX_ERROR(logger_, "cap: zmq send error! error content:" << error.what());
		assert(0);
	}
}

void ShuntNetPacket::PreHandleADisconnection()
{
	if(sock_delta_time_map_.empty())
	{
		return ;
	}
	else
	{
		map<zmq::socket_t *, unsigned long>::iterator it;
		for(it=sock_delta_time_map_.begin();it!=sock_delta_time_map_.end();++it)
		{
			struct timeval tv;
			gettimeofday(&tv,NULL);
			if(0 == it->second)
			{
				it->second = tv.tv_sec*1000000 + tv.tv_usec;	
			}
			else
			{
				if(tv.tv_sec*1000000+tv.tv_usec-it->second > 30000000) //30s
				{
					//has a disconnection, send reset tag
					PacketItem item;
                    item.thread_tag = RESET;
                    item.port_tag = 0;
                    item.data = NULL;
                    DispatchData(it->first, &item, sizeof(item));
					sock_ex_deque_.push_back(it->first);
					map<std::string, zmq::socket_t *>::iterator iter_map;
					for(iter_map=sock_ex_map_.begin();iter_map!=sock_ex_map_.end();)
					{
						if(iter_map->second == it->first) sock_ex_map_.erase(iter_map++);
					}
					//cout<<"handle a abnormal disconnection!"<<endl;
					LOG4CXX_INFO(logger_, "handled a abnormal disconnection!");
				}	
			}
		}
	}
}

void * ShuntNetPacket::RunThreadFunc()
{
	struct pcap_pkthdr *header = NULL;
	unsigned char * pkt_data = NULL;
	ip_head *ih;
	tcp_head *tcph;
	int iph_len = 0;
	int tcph_len = 0;
	int head_len = 0;
	unsigned long tcp_current_seq = 0;
	unsigned long tcp_data_len = 0;
	char key_ip_src[32];
	char key_ip_dst[32];
	XML_ListeningItem *listening_item = &(this->listening_item_);
	int port = listening_item->get_port();
	deque<zmq::socket_t *> *sock_deque = &(this->sock_ex_deque_);
	map<std::string,zmq::socket_t*>* sock_ex_map_ = &(this->sock_ex_map_);

    //signal(SIGALRM, PrintCountInfo);
    //tick.it_value.tv_sec = 10;
    //tick.it_value.tv_usec = 0;

    //tick.it_interval.tv_sec = 60;
    //tick.it_interval.tv_usec = 0;

    //setitimer(ITIMER_REAL,&tick,NULL);


	//unsigned long pack_seq = 0;

	int acktag = 0;
	int fintag = 0;
	int tcpconnstatus = 0;
	int tcpconntag = 0;
	int tcpdisconnstatus = 0;
	int tcpdisconntag = 0;

	zmq::message_t msg_rcv(sizeof(CapNetPacketItem));
	while(true)
	{
		PreHandleADisconnection();

		msg_rcv.rebuild();
		sock_->recv(&msg_rcv);
		CapNetPacketItem *msg_item = (CapNetPacketItem*)(msg_rcv.data());
		header = &(msg_item->header);
		pkt_data = msg_item->data;

		ih = (ip_head *)(pkt_data + 14);
		iph_len = (ih->ver_ihl & 0xf) * 4;
		tcph = (tcp_head *)((char *)ih + iph_len);
		tcph_len = 4*((tcph->dataoffset)>>4&0x0f);
		head_len = iph_len + tcph_len;

		//pack_seq = ntohl(tcph->seq);

		memset(key_ip_src,0,sizeof(key_ip_src));
		memset(key_ip_dst,0,sizeof(key_ip_dst));
		sprintf(key_ip_src,"%d.%d.%d.%d:%d",ih->saddr.byte1,ih->saddr.byte2,ih->saddr.byte3,ih->saddr.byte4,ntohs(tcph->source));
		sprintf(key_ip_dst,"%d.%d.%d.%d:%d",ih->daddr.byte1,ih->daddr.byte2,ih->daddr.byte3,ih->daddr.byte4,ntohs(tcph->dest));

		if(TCP == ih->protocol)
		{//1.ÅÐ¶ÏtcpÁ¬½Ó×´Ì¬ºÍ¶Ï¿ª×´Ì¬
			if(IsTcpConnection(tcph->flags, tcpconntag, tcpconnstatus))
			{
				//cout<<"A new connection was been built! The ip and port is:"<<key_ip_src<<endl;
				LOG4CXX_INFO(logger_, "A new connection was been built! The ip and port is:" << key_ip_src);
				tcpconnstatus = 0;
				if(sock_ex_map_->end() == sock_ex_map_->find(key_ip_src))
				{
					if(!sock_deque->empty())
					{
						zmq::socket_t * sock = sock_deque->front();
						sock_deque->pop_front();
						sock_ex_map_->insert(pair<std::string,zmq::socket_t *>(key_ip_src,sock));
						sock_delta_time_map_.insert(make_pair(sock,0));
						//cout<<"connection:key_ip:"<<key_ip_src<<endl;
						LOG4CXX_INFO(logger_, "connection:key_ip:" << key_ip_src);
					}
					else
					{
						if(this->IncreasePool(kPoolSize))
						{
							zmq::socket_t *sock = sock_deque->front();
							sock_deque->pop_front();
							sock_ex_map_->insert(pair<std::string,zmq::socket_t *>(key_ip_src,sock));
							sock_delta_time_map_.insert(make_pair(sock,0));
							//cout<<"connection:key_ip:"<<key_ip_src<<endl;
							LOG4CXX_INFO(logger_, "connection:key_ip:" << key_ip_src);
						}
						else
						{
							return (void*)0;
						}
					}
				}
			}
			if(IsTcpDisConnection(tcph->flags))
			{
				map<std::string,zmq::socket_t*>::iterator iter_map;
				if (port == ntohs(tcph->source))
				{
					if((iter_map=sock_ex_map_->find(key_ip_dst)) != sock_ex_map_->end())
                    {
                        zmq::socket_t * sock =iter_map->second;
                        PacketItem item;
                        item.thread_tag = RESET;
                        item.port_tag = 0;
                        item.data = NULL;
                        DispatchData(sock, &item, sizeof(item));
                        sock_deque->push_back(sock);
						map<zmq::socket_t *, unsigned long>::iterator it_time = sock_delta_time_map_.find(sock);
						if(it_time != sock_delta_time_map_.end()) sock_delta_time_map_.erase(it_time);
                        sock_ex_map_->erase(iter_map);
                        //cout<<key_ip_dst<<" was disconnected!dst"<<endl;
						LOG4CXX_INFO(logger_, key_ip_dst << " was disconnected!dst");
                    }
				}
				else if (port == ntohs(tcph->dest))
				{
					if((iter_map=sock_ex_map_->find(key_ip_src)) != sock_ex_map_->end())
					{
						zmq::socket_t * sock =iter_map->second;
						PacketItem item;
						item.thread_tag = RESET;
						item.port_tag = 0;
						item.data = NULL;
						DispatchData(sock, &item, sizeof(item));
						sock_deque->push_back(sock);
						map<zmq::socket_t *, unsigned long>::iterator it_time = sock_delta_time_map_.find(sock);
						if(it_time != sock_delta_time_map_.end()) sock_delta_time_map_.erase(it_time);
						sock_ex_map_->erase(iter_map);
						//cout<<key_ip_src<<" was disconnected!src"<<endl;
						LOG4CXX_INFO(logger_, key_ip_src << " was disconnected!src");
					}
				}
				else
				{
					assert(0);
				}

			}

			map<std::string,zmq::socket_t*>::iterator iter;
			tcp_data_len = ntohs(ih->tlen) - head_len;//must use ih->tlen, because sometime it will have supplement package.
			tcp_current_seq = ntohl(tcph->seq);
			//cout<<"cap:current_seq:"<<tcp_current_seq<<" data_len:"<<tcp_data_len<<endl;
			//caishu  --> zhongzhuan
			if((iter=sock_ex_map_->find(key_ip_dst)) != sock_ex_map_->end())
			{
				//count_pack += 1;
				/*cout<<"key_ip_dst:"<<key_ip_dst<<endl;*/
				PacketItem item;
				item.thread_tag = NORMAL;
				item.port_tag = port;
				item.header = *header;
				//unsigned char * data_buf = (unsigned char*)malloc(sizeof(CAP_PACK_BUF_SIZE));
				unsigned char *data_buf = new unsigned char[CAP_PACK_BUF_SIZE];
				assert(NULL != data_buf);
				memset(data_buf,0,CAP_PACK_BUF_SIZE);
				memcpy(data_buf, pkt_data, header->caplen);
				item.data = data_buf;
				zmq::socket_t * psock = iter->second;
				map<zmq::socket_t *, unsigned long>::iterator it_time = sock_delta_time_map_.find(psock); 
				if(it_time != sock_delta_time_map_.end()) 
				{
					it_time->second = 0;
				}
				DispatchData(psock, &item, sizeof(item));
			}
			else //zhongzhuan  --> caishu
			{
				DC_HEAD * pdch = (DC_HEAD *)(pkt_data + 54);//	54= 14+20+20 ethernet head:14bytes, ip head:20bytes, tcp head:20bytes
				if(DC_TAG == pdch->m_cTag && DCT_DSDID == pdch->m_cType)
				{
					DC_DSDID *pdsdid =  (DC_DSDID *)(pdch + 1);
					int port = listening_item->get_port();
					vector<DidStruct> did_structs;
					for(int i=0;i<pdch->m_nLen/sizeof(DC_DSDID);i++)
					{
						DidStruct did_struct;
						map<int, std::string> & did_filepath_map = listening_item->get_did_filepath_map();
						map<int, std::string>::iterator iter = did_filepath_map.find(pdsdid->m_dwDid);
						if(iter != did_filepath_map.end())
						{
							did_struct.id = pdsdid->m_dwDid;
							did_struct.whole_tag = pdsdid->m_bFull;
							did_struct.compress_tag = pdsdid->m_bNoCompress ? 0 : 1;
							did_struct.file_path = iter->second;
							did_structs.push_back(did_struct);
							pdsdid += 1;
						}
					}

					char did_conf_file[64] = {0};
					sprintf(did_conf_file, "%d_did_config.xml", port);
					WriteDidConfFile(did_conf_file, did_structs);
				}
			}
		}
		if(NULL != pkt_data)
		{
			delete[] pkt_data;
			pkt_data = NULL;
		}

	}
	return ((void *)0);
}
//
//void ShuntNetPacket::PacketHandler(unsigned char *param, const struct pcap_pkthdr *header, const unsigned char *pkt_data)
//{
//	pcap_dump(param,header,pkt_data);
//}

