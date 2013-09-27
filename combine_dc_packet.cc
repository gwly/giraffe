#include "combine_dc_packet.h"
#include "datacollect.h"
#include "flags.h"

using namespace log4cxx;

LoggerPtr CombineDCPacket::logger_(Logger::getLogger("combine_net_pack"));

static const int MAX_PACKET_LEN = 409600;

void CombineDCPacket::Init()
{
	sock_recv_ = new zmq::socket_t (*context_, this->zmqitems_[0].zmqpattern);
	//sock_->setsockopt(ZMQ_RCVHWM, &ZMQ_RCVHWM_SIZE, sizeof(ZMQ_RCVHWM_SIZE));
	if("bind" == this->zmqitems_[0].zmqsocketaction)
	{
		sock_recv_->bind(this->zmqitems_[0].zmqsocketaddr.c_str());
	}
	else if("connect" == this->zmqitems_[0].zmqsocketaction)
	{
		sock_recv_->connect(this->zmqitems_[0].zmqsocketaddr.c_str());
	}
	sock_send_= new zmq::socket_t (*context_, this->zmqitems_[1].zmqpattern);
	//sock_->setsockopt(ZMQ_RCVHWM, &ZMQ_RCVHWM_SIZE, sizeof(ZMQ_RCVHWM_SIZE));
	if("bind" == this->zmqitems_[1].zmqsocketaction)
	{
		sock_send_->bind(this->zmqitems_[1].zmqsocketaddr.c_str());
	}
	else if("connect" == this->zmqitems_[1].zmqsocketaction)
	{
		sock_send_->connect(this->zmqitems_[1].zmqsocketaddr.c_str());
	}

}

bool CombineDCPacket::IsDCType(int dc_type)
{
	if(dc_type < 0 || (dc_type > 28 && dc_type <100) || dc_type > 101)
	{
		return false;
	}
	else
		return true;
}

bool CombineDCPacket::IsDCHeader(unsigned char * dc_header)
{
	DC_HEAD * dc_head = (DC_HEAD *)dc_header;
	if(DC_TAG == dc_head->m_cTag && IsDCType(dc_head->m_cType) && 0 != dc_head->m_nLen.Get())
		return true;
	else
		return false;
}

void CombineDCPacket::DispatchData(zmq::socket_t * sock, void * data, int size)
{
	assert(NULL != sock && NULL != data);
	try
	{
		zmq::message_t msg(size);
		memcpy((void*)(msg.data()), data, size);
		//sock->send(msg,ZMQ_NOBLOCK);
		sock->send(msg);
	}
	catch(zmq::error_t &error)
	{
		//cout<<"cap: zmq send error! error content:"<<error.what()<<endl;
		LOG4CXX_ERROR(logger_, "cap: zmq send error! error content:" << error.what());
		assert(0);
	}
}

void CombineDCPacket::Combine(struct pcap_pkthdr header, unsigned char *pkt_base_addr, int pre_dch_offset, int dc_len)
{
	//count_combine_pack += dc_len;
	int temp_len = dc_len;
	unsigned char *temp_pdch = (unsigned char *)(pkt_base_addr + pre_dch_offset);
	DC_HEAD *temp_dch_item = (DC_HEAD *)temp_pdch;
	int packet_len = 0;
	int recombined_header_bufsize = 0;
	while(temp_len > 0)
	{
		//combine dc_header and judge whether the combined dc_header is the real dc_header
		if(0 !=dc_header_last_inner_len_ )
		{
			memcpy(dc_header_ + dc_header_last_inner_len_ , temp_pdch, sizeof(DC_HEAD)-dc_header_last_inner_len_);
			if(IsDCHeader(dc_header_))
			{
				recombined_header_bufsize = PCAPTOPARSE_BUF_SIZE;
				if(temp_len < recombined_header_bufsize)
				{
					memset(recombined_header_buf_ , 0 , recombined_header_bufsize);
					memcpy(recombined_header_buf_, dc_header_,dc_header_last_inner_len_);
					memcpy(recombined_header_buf_+dc_header_last_inner_len_, temp_pdch, temp_len);
					temp_pdch = recombined_header_buf_;
					temp_dch_item = (DC_HEAD *) temp_pdch;
					temp_len += dc_header_last_inner_len_;
					dc_header_last_inner_len_= 0;
				}
				else
				{
					//cout<<"caplen is larger than recombined header buffer size"<<endl;
					LOG4CXX_ERROR(logger_, "caplen is larger than recombined header buffer size");
					return ;
				}
			}
			else
			{
				dc_header_last_inner_len_ = 0;
			}
		}

		if(last_pack_len_ != 0 && long_pack_tag_ == 1)
		{
			packet_len = last_pack_len_;
			if(packet_len > MAX_PACKET_LEN)
			{
				packet_len = 0;
			}
		}
		if(1 != long_pack_tag_ && DC_TAG == temp_dch_item->m_cTag && 0 != temp_dch_item->m_nLen.Get() && IsDCType(temp_dch_item->m_cType))
		{
			packet_len = sizeof(DC_HEAD) + temp_dch_item->m_nLen.Get();
			packet_item_.header = header;
			packet_item_.data = new unsigned char [pre_dch_offset + packet_len];
			memcpy(packet_item_.data, pkt_base_addr, pre_dch_offset);

			if(packet_len > MAX_PACKET_LEN)
			{
				packet_len = 0;
			}
		}
		else if( 1 != long_pack_tag_)
			packet_len = 0;

		//case 1
		if(!case2_tag_ && temp_len >= packet_len && 0 != packet_len)
		{
			long_pack_tag_ = 0;
			last_pack_len_ = 0;
			last_temp_len_ = 0;
			while(temp_len >= packet_len && 0 != packet_len)
			{
				//cout<<"case1:temp_len:"<<temp_len<<" packet_len:"<<packet_len<<endl;
				//LOG4CXX_INFO(logger_, "case1:temp_len:" << temp_len << " packet_len:" << packet_len);
				if( DC_TAG == temp_dch_item->m_cTag && IsDCType(temp_dch_item->m_cType))
				{
					if(temp_len == packet_len)
					{

						//memset(combined_packet_buf_,0,COMBINED_PACKET_BUF_SIZE);
						memcpy(packet_item_.data+pre_dch_offset,temp_pdch,packet_len);
						//combined_packet_deque_.push_back(combined_packet_item_);
						DispatchData(sock_send_, &packet_item_, sizeof(packet_item_));
						temp_len = 0;
						packet_len = 0;
						break;
					}
					else //(temp_len > packet_len)
					{
						//memset(combined_packet_buf_, 0, COMBINED_PACKET_BUF_SIZE);
						memcpy(packet_item_.data+pre_dch_offset,temp_pdch,packet_len);
						//combined_packet_deque_.push_back(combined_packet_item_);
						DispatchData(sock_send_, &packet_item_, sizeof(packet_item_));
						temp_len -= packet_len;
						temp_pdch += packet_len;
						temp_dch_item = (DC_HEAD *)temp_pdch;

						if(temp_len > 0 && temp_len < sizeof(DC_HEAD))
						{
							memset(dc_header_, 0 ,DC_HEAD_LEN);
							memcpy(dc_header_,temp_pdch,temp_len);
							dc_header_last_inner_len_ += temp_len;
							packet_len = 0;
							break;
						}

						if(DC_TAG == temp_dch_item->m_cTag && 0 != temp_dch_item->m_nLen.Get() && IsDCType(temp_dch_item->m_cType))
						{
							last_temp_len_ = 0;
							last_pack_len_ = 0;
							packet_len = sizeof(DC_HEAD) + temp_dch_item->m_nLen.Get();
							packet_item_.header = header;
							packet_item_.data = new unsigned char [pre_dch_offset+packet_len];
							memcpy(packet_item_.data, pkt_base_addr, pre_dch_offset);
						}
						else
						{
							packet_len = 0;
							temp_len = 0;
							break;
						}
					}
				}
				else
				{
					packet_len = 0;
					temp_len = 0;
					break;
				}
			}
			if(0 == packet_len)
				break;
		}
	    if(0 != packet_len)
		{
			//case 2
			//cout<<"case2 templen:"<<temp_len<<" packet_len:"<<packet_len<<endl<<flush;
			//LOG4CXX_INFO(logger_, "case2 templen:" << temp_len << " packet_len:" << packet_len);

			//鹿媒脗脣碌么脝麓掳眉禄鹿脙禄脥锚鲁脡拢卢碌芦脢脟脠麓脢脮碌陆脪禄脨漏脝盲脣没掳眉碌脛脟茅驴枚隆拢拢篓卤脠陆脧脡脵录没拢隆卤脠脠莽脭脷脝麓dc_static鹿媒鲁脤脰脨脠麓脢脮碌陆dc_dsdata掳眉拢漏
			/*if(0 != packet_len && last_pack_len_ > 0 && packet_len > temp_len && DC_TAG == temp_dch_item->m_cTag && IsDCType(temp_dch_item->m_cType))
			{
			temp_len = 0;
			break;
			}*/
			if(0 != packet_len && last_pack_len_ == 0 && packet_len > temp_len && DC_TAG == temp_dch_item->m_cTag && IsDCType(temp_dch_item->m_cType))
			{
				case2_tag_ = true;
				long_pack_tag_ = 1;
				//memset(combined_packet_buf_,0,COMBINED_PACKET_BUF_SIZE);
				memcpy(packet_item_.data+pre_dch_offset,temp_pdch,temp_len);
				last_pack_len_ = packet_len - temp_len;
				last_temp_len_ += temp_len;
				temp_len = 0;
				break;
			}
			if(long_pack_tag_ == 1 && last_pack_len_ > temp_len)
			{
				memcpy(packet_item_.data+pre_dch_offset+last_temp_len_,temp_pdch,temp_len);
				last_pack_len_ -= temp_len;
				last_temp_len_ += temp_len;
				temp_len = 0;
			}
			else if(long_pack_tag_ ==1 && last_pack_len_<= temp_len )
			{
				memcpy(packet_item_.data+pre_dch_offset + last_temp_len_,temp_pdch,last_pack_len_);
				//combined_packet_deque_.push_back(combined_packet_item_);
				DispatchData(sock_send_, &packet_item_, sizeof(packet_item_));
				temp_pdch += last_pack_len_;
				temp_len -= last_pack_len_;
				temp_dch_item = (DC_HEAD *)temp_pdch;

				if(temp_len > 0 && temp_len < sizeof(DC_HEAD))
				{
					memset(dc_header_, 0 ,sizeof(DC_HEAD_LEN));
					memcpy(dc_header_,temp_pdch,temp_len);
					dc_header_last_inner_len_ += temp_len;
					temp_len = 0;
					packet_len = 0;
					last_pack_len_ = 0;
					last_temp_len_ = 0;
					long_pack_tag_ = 0;
					case2_tag_ = false;
					break;
				}

				//if(DC_TAG == temp_dch_item->m_cTag && 0 != temp_dch_item->m_nLen.Get() && IsDCType(temp_dch_item->m_cType))
				//{
				//	packet_len = sizeof(DC_HEAD) + temp_dch_item->m_nLen.Get();
				//}
				//else
				//{
				//	packet_len = 0;
				//	temp_len = 0;
				//}
				last_pack_len_ = 0;
				last_temp_len_ = 0;
				long_pack_tag_ = 0;
				case2_tag_ = false;
			}
			else
			{
				//cout<<"-----------"<<endl;
				temp_len = 0;
				packet_len = 0;
				last_pack_len_ = 0;
				last_temp_len_ = 0;
				long_pack_tag_ = 0;
				case2_tag_ = false;
			}
		}
		else
		{
			temp_len = 0;
			break;
		}
	}
}

void * CombineDCPacket::RunThreadFunc()
{
	pthread_detach(pthread_self());

	//signal(SIGALRM, PrintCountInfo);
	//tick.it_value.tv_sec = 10;
	//tick.it_value.tv_usec = 0;

	//tick.it_interval.tv_sec = 60;
	//tick.it_interval.tv_usec = 0;

	//setitimer(ITIMER_REAL,&tick,NULL);

    //zmq::pollitem_t items[] = {socket_parse_rcv, 0, ZMQ_POLLIN, 0};

    PacketItem *pw_item_ptr;
    int thread_tag;
	int port_tag;
	//long long seqtag;
	struct pcap_pkthdr *header = NULL;
	unsigned char *pkt_data = NULL;
	unsigned char *temp_pkt_data = NULL;
	DC_HEAD *pdch = NULL;

	int count_disorder = 0;
	ip_head *ih = NULL;
	tcp_head *tcph = NULL;
	int head_len = 0;
	int iph_len = 0;
	int tcph_len = 0;
	int pre_dch_offset = 0;
	unsigned long tcp_expect_seq = 0;
	unsigned long tcp_current_seq = 0;
	unsigned long tcp_data_len = 0 ;
	int disorder_tag = 0;
	set<TcpDisorderSetItem> tcp_disorder_set;

	zmq::message_t msg_rcv(sizeof(PacketItem));

    while(true)
    {
  //      int rc = zmq_poll(items, 1, 1000);//timeout = 1s
		//if(rc > 0)
		//{
            //if(items[0].revents & ZMQ_POLLIN)
            //{
                //socket_parse_rcv.recv(&msg_rcv,ZMQ_NOBLOCK);
				bool ret = sock_recv_->recv(&msg_rcv);
				assert(true == ret);
                pw_item_ptr = static_cast<PacketItem *>(msg_rcv.data());

				// port_tag = pw_item_ptr->port_tag;
				//seqtag = pw_item_ptr->seqtag;
				thread_tag = pw_item_ptr->thread_tag;
				header = &(pw_item_ptr->header);
				pkt_data = pw_item_ptr->data;
				if (RESET == thread_tag)
				{
					header = NULL;
					pkt_data = NULL;
					temp_pkt_data = NULL;
					pdch = NULL;
					count_disorder = 0;
					ih = NULL;
					tcph = NULL;
					head_len = 0;
					iph_len = 0;
					tcph_len = 0;
					pre_dch_offset = 0;
					tcp_expect_seq = 0;
					tcp_current_seq = 0;
					tcp_data_len = 0 ;
					disorder_tag = 0;

					//reset unfinished dc package
					//if(NULL != packet_item_.data) {
					//	delete [] packet_item_.data;
					//	packet_item_.data = NULL;
					//}
					memset(&packet_item_, 0, sizeof(packet_item_));
				    dc_header_last_inner_len_ = 0;
				    long_pack_tag_ = 0;
				    last_pack_len_ = 0;
				    last_temp_len_ = 0;
				    case2_tag_ = 0;
				    last_tcp_seq_ = 0;
				    memset(dc_header_, 0, DC_HEAD_LEN);
				    memset(recombined_header_buf_, 0 ,PCAPTOPARSE_BUF_SIZE);
				}
				else if (NORMAL == thread_tag)
				{
					ih = (ip_head *)(pkt_data + 14); //14 bytes is the length of ethernet header
					iph_len = (ih->ver_ihl & 0xf) * 4;//20bytes
					tcph = (tcp_head *) ((unsigned char*)ih + iph_len);
					tcph_len = 4*((tcph->dataoffset)>>4&0x0f);
					head_len = iph_len + tcph_len;
					pre_dch_offset = 14 + head_len;
					switch(ih->protocol)
					{
					case TCP:
						pdch = (DC_HEAD*)((u_char*)pkt_data + pre_dch_offset);//tcph_len is
						tcp_data_len = ntohs(ih->tlen) - head_len;//must use ih->tlen, because sometime it will have supplement package.
						tcp_current_seq = ntohl(tcph->seq);
						if(tcp_data_len > 0)
						{
							//if(pdch->m_cType == DCT_STKSTATIC)
						 		//DownloadData((unsigned char *)pdch, tcp_data_len);
							if(!disorder_tag)
							{
								if(tcp_expect_seq == 0 || tcp_expect_seq == tcp_current_seq)
								{
									//cout<<"tcp seq:"<<tcp_current_seq<<" tcp_data_len:"<<tcp_data_len<<endl<<flush;
									//cout<<"tcp_data_len:"<<tcp_data_len<<endl;
								 	Combine(*header,pkt_data,pre_dch_offset,tcp_data_len);
								 	//HandlePacket(header->ts,pkt_data,port_tag);
									tcp_expect_seq = tcp_current_seq + tcp_data_len;
								}
								else if(tcp_expect_seq > tcp_current_seq)
								{
									//cout<<"tcp_expect_seq > tcp_current_seq"<<endl;
									break;
								}
								else
								{
									count_disorder++;
									//cout<<"disorder! NO:"<<count_disorder<<" current_seq:"<<tcp_current_seq<<endl;
									LOG4CXX_INFO(logger_, "disorder! NO:" << count_disorder\
												 << " current_seq:" << tcp_current_seq);
									disorder_tag = 1;
									unsigned char *pktdata = (unsigned char *)malloc(header->caplen);
									assert(NULL != pktdata);
									memcpy(pktdata, (unsigned char *)pkt_data, header->caplen);
									TcpDisorderSetItem item;
									item.tcp_seq = tcp_current_seq;
									item.pre_dch_offset = pre_dch_offset;
									item.tcp_data_len = tcp_data_len;
									item.timestamp = header->ts;
									item.pktdata = pktdata;
									tcp_disorder_set.insert(item);
								}
							}
							else//disorder
							{
								count_disorder++;
								//cout<<"disorder! NO:"<<count_disorder<<" current_seq:"<<tcp_current_seq<<endl;
								if(tcp_expect_seq == tcp_current_seq)
								{
									//cout<<"tcp seq:"<<tcp_current_seq<<" tcp_data_len:"<<tcp_data_len<<endl<<flush;
									//cout<<"tcp_data_len:"<<tcp_data_len<<endl;
								 	Combine(*header, pkt_data,pre_dch_offset,tcp_data_len);
								 	//HandlePacket(header->ts,pkt_data,port_tag);
									tcp_expect_seq = tcp_current_seq + tcp_data_len;
									set<TcpDisorderSetItem>::iterator iter;
									for(iter=tcp_disorder_set.begin();iter != tcp_disorder_set.end();)
									{
										tcp_current_seq = iter->tcp_seq;
										tcp_data_len = iter->tcp_data_len;
										pre_dch_offset = iter->pre_dch_offset;
										temp_pkt_data = iter->pktdata;
										struct timeval timestamp = iter->timestamp;
										if(tcp_expect_seq == tcp_current_seq)
										{
											//cout<<"tcp seq:"<<tcp_current_seq<<" tcp_data_len:"<<tcp_data_len<<endl<<flush;
										 	Combine(*header, temp_pkt_data,pre_dch_offset,tcp_data_len);
										 	//HandlePacket(timestamp,pkt_data,port_tag);
											tcp_expect_seq = tcp_current_seq + tcp_data_len;
											free(temp_pkt_data);
											temp_pkt_data = NULL;
											tcp_disorder_set.erase(iter++);
										}
										else
										{
											//cout<<"the special case!"<<endl;
											break;
										}
									}
									if(tcp_disorder_set.empty())
									{
										disorder_tag = 0;
										count_disorder = 0;
										//cout<<"fixed disorder!"<<endl;
									}
								}
								else if(tcp_expect_seq > tcp_current_seq)
								{
									break;
								}
								else
								{
									unsigned char *pktdata = (unsigned char *)malloc(header->caplen);
									assert(NULL != pktdata);
									memcpy(pktdata, (unsigned char *)pkt_data, header->caplen);
									TcpDisorderSetItem item;
									item.tcp_seq = tcp_current_seq;
									item.pre_dch_offset = pre_dch_offset;
									item.tcp_data_len = tcp_data_len;
									item.timestamp = header->ts;
									item.pktdata = pktdata;
									tcp_disorder_set.insert(item);
								}
							}

							//old code
							 //if(last_tcp_seq_ < current_tcp_seq)//filter the same tcp seq
							 //{
							 //	cout<<"tcp seq:"<<current_tcp_seq<<" dc_len:"<<dc_len<<endl<<flush;
							 //	last_tcp_seq_ = current_tcp_seq;
							 //	DownloadData((unsigned char *)pdch, dc_len);
							 //	CombineDCPacket((unsigned char *)pdch,dc_len);
							 //	HandlePacket(header,pkt_data,port_tag);
							 //}
							 //else
							 //{
							 //	cout<<"warning: last tcp seq("<<last_tcp_seq_<<") >= current tcp seq("<<current_tcp_seq<<")!"<<endl;
							 //}
						}
						break;
					case UDP:
						break;
					default:
						break;
					}
				}
				else
				{
					assert(0);
				}

			//}
  //      }
  //      else if (rc ==0 )//timeout
  //      {
  //          continue;
  //      }
  //      else
  //      {
  //          cout<<"zmq poll fail"<<endl;
		//	throw zmq_errno();
		//}
		if (NULL != pkt_data)
		{
			delete [] pkt_data;
			pkt_data = NULL;
		}
    }
}
