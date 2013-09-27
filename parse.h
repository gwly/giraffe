#ifndef MONITOR_PACKETPARSE_H_
#define MONITOR_PACKETPARSE_H_

#ifdef __linux
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#else
#include "winsock2.h"
#endif

#include <pthread.h>
#include <deque>
#include <iostream>

#ifdef __linux
    #include <luajit-2.0/lua.hpp>
#else
	#include <lua.hpp>
#endif

#include "data.h"
#include "structs.h"
#include "datacollect.h"
#include "flags.h"
#include "utils.h"
#include "basethread.h"
#include "extract_dc.h"
#include "dzh_time_t.h"
#include "xml_class_set.h"
#include <csignal>


using namespace std;

#define EXTRACT_BUF (2*1024*2014)
#define DID_TEMPLATE_BUF (2*1024*1024)

class Parse:public BaseThread
{
public:
    Parse(zmq::context_t * context, XML_ListeningItem &listening_item):listening_item_(listening_item)
    {
        context_ = context;
		sock_recv_ = NULL;
		sock_send_to_lua_routine_ = NULL;
		sock_send_to_log_ = NULL;
        nHqTotal = 0;
        extractbuf = NULL;
        pStkStatic = NULL;
        pStkDyna = NULL;
        pStkMMPEx = NULL;
		count_rc_ = 0;
		last_pack_len_ = 0;
		last_temp_len_ = 0;
		long_pack_tag_ = 0;
		case2_tag_  = false;
		dc_header_last_inner_len_ = 0;
		last_tcp_seq_ = 0;
    };
    virtual ~Parse()
    {
		if(NULL != sock_recv_)
		{
			delete sock_recv_;
			sock_recv_ = NULL;
		}
		if(NULL != sock_send_to_log_)
		{
			delete sock_send_to_log_;
			sock_send_to_log_ = NULL;
		}
		if(NULL != sock_send_to_lua_routine_)
		{
			delete sock_send_to_lua_routine_;
			sock_send_to_lua_routine_ = NULL;
		}
		if(extractbuf != NULL)
		{
			delete[] extractbuf;
			extractbuf = NULL;
		}
		if(pStkStatic != NULL)
		{
			delete[] pStkStatic;
			pStkStatic = NULL;
		}
		if(pStkDyna != NULL)
		{
			delete[] pStkDyna;
			pStkDyna = NULL;
		}
		if(pStkMMPEx != NULL)
		{
			delete[] pStkMMPEx;
			pStkMMPEx = NULL;
		}
	};
	void *RunThreadFunc();
	void Init();
    BOOL DecryptDataPack(DC_HEAD * pData);
    BOOL ExtractDataPack(const DC_HEAD* pOrgHead,DC_HEAD* pHeadBuf,int nBufSize,WORD* pwMarketBuf=NULL);
	bool IsDCType(int dc_type);
	bool IsDCHeader(unsigned char * dc_header);
	void CombineDCPacket(unsigned char * pdch, int dc_len);
	void HandlePacket(struct timeval timestamp, unsigned char *pkt_data, int port_tag);
	//void DispatchToLogThread(bufelement &info);
	//void DispatchToLuaRoutineThread(void * data, int size);
	void DispatchData(zmq::socket_t * sock, void *data, size_t size);
	void DownloadData(unsigned char * data, size_t len);
	int nHqTotal;
    char*  extractbuf;
    STK_STATIC *pStkStatic;
    STK_DYNA *pStkDyna;
    SH_L2_MMPEX *pStkMMPEx;
protected:
private:
	void InitZMQ();
	
	zmq::context_t *context_;
	zmq::socket_t *sock_recv_;
	zmq::socket_t *sock_send_to_lua_routine_;
	zmq::socket_t *sock_send_to_log_;
	XML_ListeningItem listening_item_;
	ExtractDC extractDC_;
	deque<CombinedPacketItem> combined_packet_deque_;
	int count_rc_;
	int long_pack_tag_;
	int last_pack_len_;
	int last_temp_len_;
	bool case2_tag_;
	CombinedPacketItem combined_packet_item_;
	unsigned char dc_header_[10];//10 is the length of dc_header
	int dc_header_last_inner_len_;
	unsigned long last_tcp_seq_;
	unsigned char recombined_header_buf_[PCAPTOPARSE_BUF_SIZE];
};


//class Parse
//{
//public:
//    Parse()
//    {
//    };
//    ~Parse(){};
//    void Parsing(int, struct pcap_pkthdr *, unsigned char *,zmq::socket_t *);
//    void StartThread(zmq::context_t *, int);
//    void JoinThread();
//
//    static void *ThreadFunc(void * args);
//protected:
//private:
//    pthread_t tid_;
//    ParseParam pp_;
//};

#endif // MONITOR_PACKETPARSE_H_
