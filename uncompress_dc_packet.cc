#include "uncompress_dc_packet.h"
#include "extract_dc.h"
//#include "DidUncompress.h"
//#include "public.h"
#include "xml_class_set.h"
#include "DIDTemplateToLuaStruct.h"
#ifdef __linux
    #include <luajit-2.0/lua.hpp>
#else
    #include <lua.hpp>
#endif
#include <sys/types.h>
#include <assert.h>
#include <zlib.h>
#include <set>
#include "flags.h"

using namespace log4cxx;

LoggerPtr UncompressDCPacket::logger_(Logger::getLogger("uncompress_dc_pack"));

BOOL UncompressDCPacket::ExtractDataPack(const DC_HEAD* pOrgHead,DC_HEAD* pHeadBuf,int nBufSize,WORD* pwMarketBuf)
{
	assert(NULL != pOrgHead && NULL != pHeadBuf);
    const BYTE *pOrgData = (BYTE *)(pOrgHead + 1);
    BYTE *pDataBuf = (BYTE *)(pHeadBuf + 1);
 	int nRet = 0;
	if(pOrgHead->m_cTag==DC_TAG && !(pOrgHead->m_wAttrib&(DC_ENCY_MASK)) && (pOrgHead->m_wAttrib&DC_CPS_MASK)==DC_STD_CPS)
	{
		if(!pOrgData)
			pOrgData = (BYTE*)(pOrgHead+1);
		if(!pDataBuf)
			pDataBuf = (BYTE*)(pHeadBuf+1);

		*pHeadBuf = *pOrgHead;
		pHeadBuf->m_wAttrib &= ~DC_STD_CPS;

		extractDC_.set_stk_total(nHqTotal);
		nRet = extractDC_.ExtractData(pOrgHead->m_cType,pOrgData,(DWORD)pOrgHead->m_nLen.Get(),pDataBuf,nBufSize-sizeof(DC_HEAD),pStkDyna,pStkStatic);
		if(nRet>0)
		{
			pHeadBuf->m_nLen = nRet;
			nRet += sizeof(DC_HEAD);
		}
	}
	else if (DC_TAG == pOrgHead->m_cTag && DC_ZLIB_CPS == (pOrgHead->m_wAttrib & DC_CPS_MASK))
	{
		char * extract_buf = (char*)pHeadBuf;
		ZLIB_HEAD *zlib_head = (ZLIB_HEAD*)(pOrgHead + 1);
		int dst_len = nBufSize;
		int zlib_len = zlib_head->m_dwSourceLen + sizeof(DC_HEAD);
		if(zlib_len > nBufSize)
		{
			if(NULL != extract_buf)
			{
				delete [] extract_buf;
				extract_buf = NULL;
			}
			extract_buf = new char[zlib_len];
			pHeadBuf = (DC_HEAD*)extract_buf;
			dst_len = zlib_len; 
		}
		memcpy(extract_buf,pOrgHead,sizeof(DC_HEAD));
		dst_len -= sizeof(DC_HEAD);
		char *zlib_data = (char*)(zlib_head + 1);
		uncompress((Bytef*)(extract_buf+sizeof(DC_HEAD)),(uLongf*)&dst_len,(Bytef*)zlib_data,pOrgHead->m_nLen.Get()-sizeof(ZLIB_HEAD));
		if(dst_len != zlib_head->m_dwSourceLen)
		{
			nRet = -1;
		}
		nRet = pHeadBuf->m_nLen.Get() + sizeof(DC_HEAD);
	}
	return nRet;
}

void UncompressDCPacket::Uncompress(struct timeval timestamp, unsigned char *pkt_data, int pre_dch_offset)
{
	int extract_ret = 0;
	static int did_sync_module_tag = 0;
	int did_template_id = 0;
	short stknum = 0;
	int struct_size = 0;
	DC_HEAD * pdch = NULL;
	unsigned char * pdcdata = NULL;
	int dc_general_intype = 0;
	
	char *iproto = NULL;
	unsigned short sport,dport;
	char *netflags = NULL;
	ip_head *ih = NULL;
	int iph_len = 0;
	int tcph_len = 0;
	udp_head *udph = NULL;
	tcp_head *tcph = NULL;
	struct tm *ltime = NULL;
	time_t local_tv_sec;
	bufelement info;


	//DataBuffer data_buf;
	pdch = (DC_HEAD *)(pkt_data+pre_dch_offset);
	//cout<<"dctype:"<<(int)(pdch->m_cType)<<endl;
	//LOG4CXX_INFO(logger_, "dctype:" << (int)(pdch->m_cType));
	if( DC_STD_CPS == (pdch->m_wAttrib & DC_CPS_MASK) || DC_ZLIB_CPS == (pdch->m_wAttrib &DC_CPS_MASK))
	{
		if(NULL == extractbuf)
		{
			extractbuf = new char[EXTRACT_BUF];
		}
		memset(extractbuf,0,EXTRACT_BUF);
		if((extract_ret = (ExtractDataPack(pdch,(DC_HEAD *)extractbuf,EXTRACT_BUF)))>0)
		{
			pdch = (DC_HEAD *)extractbuf;
		}
		else if(extract_ret == 0)
		{
			//cout<<"the packet isn't extracted!"<<endl;
			//LOG4CXX_INFO(logger_, "the packet isn't extracted!");
		}
		else
		{
			//cout<<"extract error! error num is :"<<extract_ret<<endl;
			LOG4CXX_ERROR(logger_, "extract error! error num is :" << extract_ret);
			return ;
		}
	}
	
    if(DCT_STKSTATIC == pdch->m_cType)
    {
		//initial the did related files
		extractDC_.set_static_before_dyna_tag(true);	
        DC_STKSTATIC_MY* p = (DC_STKSTATIC_MY*)(pdch+1);
        if (0 == pdch->m_wAttrib)
        {
			stknum = p->m_nNum;
			struct_size = sizeof(STK_STATIC);
            pdcdata = (unsigned char  *)(p+1);
            nHqTotal = p->m_nNum;
            if(NULL == pStkDyna)
            {
                pStkDyna = new STK_DYNA[nHqTotal];
            }
            else
            {
                delete [] pStkDyna;
                pStkDyna = new STK_DYNA[nHqTotal];
            }
            if(NULL == pStkMMPEx)
            {
                pStkMMPEx = new SH_L2_MMPEX[nHqTotal];
            }
            else
            {
                delete [] pStkMMPEx;
                pStkMMPEx = new SH_L2_MMPEX[nHqTotal];
            }

            if(NULL == pStkStatic)
            {
                pStkStatic = new STK_STATIC[nHqTotal];
            }
            else
            {
                delete [] pStkStatic;
                pStkStatic = new STK_STATIC[nHqTotal];
            }

            if(pStkStatic && pStkDyna && pStkMMPEx)
            {
                STK_STATIC *pdata = (STK_STATIC *)(p+1);
                memmove(pStkStatic, pdata, nHqTotal*sizeof(STK_STATIC));
                memset(pStkDyna,   0, sizeof(STK_DYNA)*nHqTotal);
                memset(pStkMMPEx,  0, sizeof(SH_L2_MMPEX)*nHqTotal);
            }
       	}
       	else if (DC_DATA_CHANGED == pdch->m_wAttrib)
       	{
			STK_STATIC* pSta = (STK_STATIC*)(p+1);
            STK_STATIC* pData = NULL;
            for (int kk=0;kk<p->m_nNum;kk++)
            {
                if ((pSta+kk)->m_wStkID < nHqTotal)
                {
                    pData = pStkStatic + (pSta+kk)->m_wStkID;
                    memmove(pData, pSta+kk, sizeof(STK_STATIC));
                }
            }
       	}
    }
    else if(DCT_STKDYNA == pdch->m_cType)
    {
        DC_STKDYNA_MY* p = (DC_STKDYNA_MY*)(pdch+1);
		stknum = p->m_nNum;
		struct_size = sizeof(STK_DYNA);
        pdcdata = (unsigned char  *)(p+1);
        STK_DYNA* pDyna = (STK_DYNA*)(p+1);
        STK_DYNA* pData = NULL;
        for (int i=0; i<p->m_nNum; i++)
        {
            if (((pDyna+i)->m_wStkID < nHqTotal))
            {
                pData = pStkDyna + (pDyna+i)->m_wStkID;
                memmove(pData, pDyna+i, sizeof(STK_DYNA));
            }
        }
    }
    else if(DCT_SHL2_MMPEx == pdch->m_cType)
    {
		DC_SHL2_MMPEx_MY *p= (DC_SHL2_MMPEx_MY *)(pdch+1);
		pdcdata = (unsigned char *)(p+1);
		stknum = p->m_nNum;
		struct_size = sizeof(SH_L2_MMPEX);
		SH_L2_MMPEX* pMMPEx = (SH_L2_MMPEX*)(p+1);
		SH_L2_MMPEX* pData = NULL;
		for (int i=0; i<p->m_nNum; i++)
		{
			if (((pMMPEx+i)->m_wStkID < nHqTotal) )
			{
				pData = pStkMMPEx + (pMMPEx+i)->m_wStkID;
				memmove(pData, pMMPEx+i, struct_size);
			}
		}
    }
	else if(DCT_SHL2_QUEUE == pdch->m_cType)
	{
//         DC_SHL2_QUEUE* p = (DC_SHL2_QUEUE*)(pdch+1);
//         BOOL bLog = FALSE;
		//int nOrderCount = 0;
		//int strLen = 0;
		//std::string strOut;
		//char buffer[10];
		//memset(buffer, 0, 10);
//         for (int i=0; i< pdch->m_nLen/4;i++)
//         {
//             if ((p->m_dwQueue[i] & QUEUE_TYPE_MASK) == QUEUE_STOCKID)
//             {
//                 nOrderCount = 0;
//                 strLen = strOut.length();
//                 strOut.erase(0, strLen);
//                 if ((p->m_dwQueue[i]&(~QUEUE_STOCKID)) == 1) 
//                 {
//                     bLog = TRUE;
//                 }else
//                 {
//                     bLog = FALSE;
//                 }
//             }else if ((p->m_dwQueue[i] & QUEUE_TYPE_MASK) == QUEUE_BUYPRICE)
//             {
//                 nOrderCount = 0;
//                 strLen = strOut.length();
//                 strOut.erase(0, strLen);
//                 
//                 i++;
//                 
//             }else if ((p->m_dwQueue[i] & QUEUE_TYPE_MASK) == QUEUE_SELLPRICE)
//             {
//                 nOrderCount = 0;
//                 strLen = strOut.length();
//                 strOut.erase(0, strLen);
//                 i++;
//             }else
//             {
//                 nOrderCount++;
//                 memset(buffer, 0, 10);
//                 sprintf(buffer, " %d", p->m_dwQueue[i]);
//                 strOut += std::string(buffer);
//             }
//         }
	}
	else if (DCT_SZL2_ORDER_STAT == pdch->m_cType)
	{
		DC_SZL2_ORDER_STAT_MY * p = (DC_SZL2_ORDER_STAT_MY *)(pdch + 1);
		pdcdata = (unsigned char *)(p+1);
		stknum = p->m_nNum;
		struct_size = sizeof(SZ_L2_ORDER_STAT);
	}
	else if (DCT_SZL2_ORDER_FIVE == pdch->m_cType)
	{
		SZL2_ORDER_FIVE * p = (SZL2_ORDER_FIVE *)(pdch + 1);
		pdcdata = (unsigned char *)p;
		struct_size = sizeof(SZL2_ORDER_FIVE);
		stknum = pdch->m_nLen / struct_size; 
	}
	else if (DCT_SZL2_TRADE_FIVE == pdch->m_cType)
	{
		SZL2_TRADE_FIVE * p = (SZL2_TRADE_FIVE*)(pdch + 1);
		pdcdata = (unsigned char *)p;
		struct_size = sizeof(SZL2_TRADE_FIVE);
		stknum = pdch->m_nLen / struct_size;
	}
	else if(DCT_GENERAL == pdch->m_cType)
	{
		DC_GENERAL_MY * p = (DC_GENERAL_MY *)(pdch + 1);
		pdcdata = (unsigned char *)(p+1);
		stknum = p->m_wNum;
		struct_size = p->m_nDataSize;
		int total_len = p->m_nTotalLen;
		int num = p->m_wNum;
		int data_size = p->m_nDataSize;
		if(GENERAL_STRUCT_FIX == p->m_dwDataAttr)
		{
			int len = sizeof(DC_GENERAL_MY ) + num * sizeof(WORD) + 	num * data_size;
			if(GE_STATIC_EX == p->m_wDataID)
			{
				dc_general_intype = GE_STATIC_EX;

			}
			else if(GE_HKDYNA == p->m_wDataID)
			{
				dc_general_intype = GE_HKDYNA;
			}
		}
		else if(GENERAL_FLOAT_FIX == p->m_dwDataAttr)
		{
			if(GE_IOPV == p->m_wDataID)
			{
				dc_general_intype = GE_IOPV;
			}
			else if(GE_MATU_YLD == p->m_wDataID)
			{
				dc_general_intype = GE_MATU_YLD;
			}
		}
		else if(GENERAL_STRING_FIX == p->m_dwDataAttr)
		{

		}
		else if(GENERAL_STRING_VAR == p->m_dwDataAttr)
		{
			if(GE_HKREL_TXT == p->m_wDataID)
			{
				dc_general_intype = GE_HKREL_TXT;
			}	
		}
		else if(GENERAL_FLOAT_VAR == p->m_dwDataAttr)
		{

		}
		else if(GENERAL_VAR == p->m_dwDataAttr)
		{
			if(GE_BLK_STK == p->m_wDataID)
			{
				dc_general_intype = GE_BLK_STK;
			}
		}
	}
	else if(DCT_DSDATA == pdch->m_cType)
	{
		//write template files
		//DC_DSDATA * dsdata = (DC_DSDATA *)(pdch+1);
		//int data_len = pdch->m_nLen.Get() + sizeof(DC_DSLEN) - sizeof(DC_DSDATA) - dsdata->m_dwDidNum*sizeof(DC_DSLEN);
		//if(dsdata->m_dwDidNum > 0)
		//{
		//	DC_DSLEN *ds_len = dsdata->mDsLen;
		//	char *did_template_data = (char *)ds_len +dsdata->m_dwDidNum * sizeof(DC_DSLEN);
		//	for(int i=0;i<dsdata->m_dwDidNum;i++)//write template files
		//	{
		//		char file_name[64] = {0};
		//		sprintf(file_name ,"%lu.xml",ds_len->m_dwDid);

		//		//wirte to file
		//		FILE * fp = fopen(file_name, "wb");
		//		assert(NULL != fp);
		//		fwrite(did_template_data,ds_len->m_dwLen,1,fp);
		//		fclose(fp);
		//		fp = NULL;

		//		data_len -= ds_len->m_dwLen;
		//		did_template_data += ds_len->m_dwLen;
		//		ds_len++;
		//	}
		//}
	}
	else if(DCT_DID == pdch->m_cType)
	{
		//int port = listening_item_.get_port();
		//char temp_file[64];
		//sprintf(temp_file,"%d_did_config.xml",port);
		//std::string did_config_file(temp_file);
		//DidUncompress diducp(did_config_file);
		//diducp.ReadConfig();
		//diducp.Initialize();
		//if(diducp.DisassemblePack(pdch,data_buf))
		//{
		//	cout<<"uncompress success!"<<endl;
		//	DC_DIDHead *did_head = (DC_DIDHead *)(pdch+1);
		//	did_template_id = did_head->GetDid();
		//	stknum = did_head->GetRecNum();
		//	struct_size = data_buf.GetLen()/stknum;
		//	pdcdata = (unsigned char *)(data_buf.GetData());
		//}
		//else
		//{
		//	cout<<"uncompress fail"<<endl;
		//}
	}
	else if(DCT_DIDSTATIC == pdch->m_cType)
	{

	}
	else
	{
		pdcdata = NULL;
	}
	
	if(NULL != pdcdata)
	{
		Lua_ZMQ_MSG_Item msg_item;
		msg_item.dc_type = pdch->m_cType;
		msg_item.pack_len = pdch->m_nLen+10; 
		msg_item.dc_general_intype = dc_general_intype;
		msg_item.stk_num = stknum;
		msg_item.struct_size = struct_size;
		msg_item.did_template_id = did_template_id;
		msg_item.stk_static = pStkStatic;
		int data_size = stknum * struct_size;
		unsigned char *buf = (unsigned char *)malloc(data_size);
		assert(NULL != buf);
		memcpy(buf,pdcdata,data_size);
		msg_item.pdcdata = buf;
		DispatchData(sock_send_to_lua_routine_, &msg_item, sizeof(msg_item));
	}

	ih = (ip_head *)(pkt_data + 14); //14 bytes is the length of ethernet header
    iph_len = (ih->ver_ihl & 0xf) * 4;//20bytes

	tcph = (tcp_head *) ((unsigned char*)ih + iph_len);
	tcph_len = 4*((tcph->dataoffset)>>4&0x0f);
    iproto = "TCP";
   		/* convert from network byte order to host byte order */
	sport = ntohs( tcph->source );
	dport = ntohs( tcph->dest );
	netflags = Utils::tcp_flag_to_str(tcph->flags);
	/* convert the timestamp to readable format */
	local_tv_sec = timestamp.tv_sec;
	ltime=localtime(&local_tv_sec);
	memset(info.timestamp, 0, sizeof(info.timestamp));
	strftime(info.timestamp, sizeof(info.timestamp), "%H:%M:%S", ltime);

	info.port_tag = listening_item_.get_port();
	info.len = ntohs(ih->tlen) - iph_len - tcph_len;
	info.iproto = iproto;
	info.saddrbyte1 = ih->saddr.byte1;
	info.saddrbyte2 = ih->saddr.byte2;
	info.saddrbyte3 = ih->saddr.byte3;
	info.saddrbyte4 = ih->saddr.byte4;
	info.sport = sport;
	info.daddrbyte1 = ih->daddr.byte1;
	info.daddrbyte2 = ih->daddr.byte2;
	info.daddrbyte3 = ih->daddr.byte3;
	info.daddrbyte4 = ih->daddr.byte4;
	info.dport = dport;
	info.flags = netflags;
    
	if(DCT_GENERAL == pdch->m_cType)
	{
		if(0 != dc_general_intype)
		{
			info.dctype = Utils::DCGeneral_IntypeToString(dc_general_intype);
		}
		else
		{
			info.dctype = Utils::DCTypeToString(pdch->m_cType);
		}
	}
	else
	{
		info.dctype = Utils::DCTypeToString(pdch->m_cType);
	}
	info.seqtag = pdch->m_nSeq.Get();
	DispatchData(sock_send_to_log_, &info, sizeof(info));
                        
	pdch = NULL;
	pdcdata = NULL;	
//                                try
//                                {
//                                    zmq::message_t msg_workinglua_send(sizeof(pdch));
//                                    memcpy((void *)msg_workinglua_send.data(),(char *)&stock_dyna,sizeof(stock_dyna));
//                                    socket_workinglua_send.send(msg_workinglua_send,ZMQ_NOBLOCK);
//                                }
//                                catch(zmq::error_t error)
//                                {
//                                    cout<<"zmq send error"<<error.what()<<endl;
//									  LOG4CXX_ERROR(logger_, "zmq send error" << error.what()):
//                                }

            //test UncompressDCPacket flow rate
//                          if(countnum_UncompressDCPacket == 0)
//                          {
//                              timebase_UncompressDCPacket = time((time_t)NULL);
//                              cout<<"UncompressDCPacket begin counting"<<endl;
//                          }
//                          long int timelive_UncompressDCPacket = time((time_t *)NULL);
//                          if(timelive_UncompressDCPacket - timebase_UncompressDCPacket > 120 && timetag_UncompressDCPacket== 0)
//                          {
//                              timetag_UncompressDCPacket = 1;
//                              //cout<<"UncompressDCPacket filtertag:"<<filtertag<<" !!! 2min packet quantity:"<<countnum_UncompressDCPacket<<endl<<flush;
//                          }
//                          else
//                          {
//                              countnum_UncompressDCPacket = countnum_UncompressDCPacket + 1;
				///*cout<<countnum_UncompressDCPacket<<endl;*/
//                          }
    
}


void UncompressDCPacket::DispatchData(zmq::socket_t * sock, void *data, size_t size)
{
	assert(NULL != data && NULL != sock);
	try
	{
		zmq::message_t msg(size);
		memcpy(msg.data(), data, size);
		sock->send(msg);
	}
	catch(zmq::error_t error)
	{
		//cout<<"zmq send error! error content:"<<error.what()<<endl;
		LOG4CXX_ERROR(logger_, "zmq send error! error content:" << error.what());
		assert(0);
	}
}


void UncompressDCPacket::Init()
{
	InitZMQ();
}

void UncompressDCPacket::InitZMQ()
{
    sock_recv_ = new zmq::socket_t (*context_, this->zmqitems_[0].zmqpattern);

    //sock_recv_->setsockopt(ZMQ_RCVHWM,&ZMQ_RCVHWM_SIZE,sizeof(ZMQ_RCVHWM_SIZE));
    if("bind" == this->zmqitems_[0].zmqsocketaction)
    {
        sock_recv_->bind(this->zmqitems_[0].zmqsocketaddr.c_str());
    }
    else if("connect" == this->zmqitems_[0].zmqsocketaction)
    {
        sock_recv_->connect(this->zmqitems_[0].zmqsocketaddr.c_str());
    }
    //sock_recv_->setsockopt(ZMQ_SUBSCRIBE,"",0);

    sock_send_to_lua_routine_ = new zmq::socket_t (*context_, this->zmqitems_[1].zmqpattern);
    //sock_send_to_lua_routine_->setsockopt(ZMQ_SNDHWM,&ZMQ_SNDHWM_SIZE,sizeof(ZMQ_SNDHWM_SIZE));
    if("bind" == this->zmqitems_[1].zmqsocketaction)
    {
        sock_send_to_lua_routine_->bind(this->zmqitems_[1].zmqsocketaddr.c_str());
    }
    else if("connect" == this->zmqitems_[1].zmqsocketaction)
    {
        sock_send_to_lua_routine_->connect(this->zmqitems_[1].zmqsocketaddr.c_str());
    }

    sock_send_to_log_ = new zmq::socket_t (*context_, this->zmqitems_[2].zmqpattern);
    if("bind" == this->zmqitems_[1].zmqsocketaction)
    {
        sock_send_to_log_->bind(this->zmqitems_[2].zmqsocketaddr.c_str());
    }
    else if("connect" == this->zmqitems_[2].zmqsocketaction)
    {
        sock_send_to_log_->connect(this->zmqitems_[2].zmqsocketaddr.c_str());
    }
}

void UncompressDCPacket::DownloadData(unsigned char * data, size_t len)
{
	size_t length = len + sizeof(size_t);
	char *buf = (char *)malloc(length);
	memcpy(buf, &len, sizeof(size_t));
	memcpy(buf + 4, data, len);
	Utils::WriteIntoFile("test_data.txt", "ab+", buf, length);	
	free(buf);
	buf = NULL;
}

void *UncompressDCPacket::RunThreadFunc()
{
	pthread_detach(pthread_self());

	//signal(SIGALRM, PrintCountInfo);
	//tick.it_value.tv_sec = 10;
	//tick.it_value.tv_usec = 0;

	//tick.it_interval.tv_sec = 60;
	//tick.it_interval.tv_usec = 0;

	//setitimer(ITIMER_REAL,&tick,NULL);

    //zmq::pollitem_t items[] = {socket_UncompressDCPacket_rcv, 0, ZMQ_POLLIN, 0};
 
    PacketItem *packet_item_ptr;
	
	//long long seqtag;
	struct pcap_pkthdr *header = NULL;
	unsigned char *pkt_data = NULL;
    unsigned int countnum = 0;
    long int timebase = 0;
    int timetag = 0;
	DC_HEAD *pdch;
	
    unsigned int countnum_UncompressDCPacket = 0;
    long int timebase_UncompressDCPacket = 0;
    long int timelive_UncompressDCPacket = 0;
    int timetag_UncompressDCPacket = 0;

	char * pfinalpacket = NULL;
	int dc_len = 0;
	ip_head *ih = NULL;
	tcp_head *tcph = NULL;
	int pre_dch_offset = 0;
	int iph_len = 0;
	int tcph_len = 0;
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
                //socket_UncompressDCPacket_rcv.recv(&msg_rcv,ZMQ_NOBLOCK);
				bool ret = sock_recv_->recv(&msg_rcv);
				assert(true == ret);
                packet_item_ptr = static_cast<PacketItem *>(msg_rcv.data());

				header = &(packet_item_ptr->header);
				pkt_data = packet_item_ptr->data;
				if(NULL == pkt_data) break;	
				ih = (ip_head *)(pkt_data + 14); //14 bytes is the length of ethernet header	
				iph_len = (ih->ver_ihl & 0xf) * 4;//20bytes
				tcph = (tcp_head *) ((unsigned char*)ih + iph_len);
				tcph_len = 4*((tcph->dataoffset)>>4&0x0f);
				pre_dch_offset = 14 + iph_len + tcph_len;

				Uncompress(header->ts, pkt_data, pre_dch_offset);

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

