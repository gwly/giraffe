#ifndef MONITOR_PROTOCOL_H_
#define MONITOR_PROTOCOL_H_

#include <pthread.h>
#include <iostream>
#include <deque>
#include "zmq.hpp"
#include "pcap/pcap.h"
#include "log4cpp/Category.hh"
#include "flags.h"
#include "mword.h"

using namespace std;

const int PCAPTOPARSE_BUF_SIZE = 2048;
const int COMBINED_PACKET_ITEM_SIZE = 409600;
const int LUA_RECV_BUF_SIZE = 1048576;

#pragma pack(push,1)

typedef struct CombinedPacketItem
{
	char data[COMBINED_PACKET_ITEM_SIZE];
}CombinedPacketItem;

typedef struct TcpDisorderSetItem
{
	unsigned long tcp_seq;
	int pre_dch_offset;
	int tcp_data_len;
	struct timeval timestamp;
	unsigned char *pktdata;

	//overwrite the operator <
	bool operator < (const TcpDisorderSetItem &item) const
	{
		return tcp_seq < item.tcp_seq;
	}
}TcpDisorderSetItem;

/* 4 bytes IP address */
typedef struct ip_address
{
	u_char byte1;
	u_char byte2;
	u_char byte3;
	u_char byte4;
}ip_address;

typedef struct IpHead
{
    u_char	ver_ihl;		// Version (4 bits) + Internet header length (4 bits)
	u_char	tos;			// Type of service
	u_short tlen;			// Total length
	u_short identification; // Identification
	u_short flags_fo;		// Flags (3 bits) + Fragment offset (13 bits)
	u_char	ttl;			// Time to live
	u_char	protocol;			// Protocol
	u_short crc;			// Header checksum
	ip_address	saddr;		// Source address
	ip_address	daddr;		// Destination address
	u_int	op_pad;			// Option + Padding
}ip_head;

typedef struct TcpHead
{
    u_short source;  //source port
	u_short dest; //destination port
	u_long seq;  //sequence number
	u_long ack_seq;  //acknowledge number
	u_char dataoffset;  //data offset
	u_char flags;  // flags
	u_short window;  // window length
	u_short check;  //checksum
	u_short urg_ptr;  //urgent pointer
}tcp_head;

typedef struct UdpHead
{
    u_short sport;			// Source port
	u_short dport;			// Destination port
	u_short len;			// Datagram length
	u_short crc;			// Checksum
}udp_head;

struct CPS_STATIC_HEAD
{
	enum ATTR
	{
		SEQ_ID = 1,				//˳��ID
		FULLDATA = 2,			//����ѹ��/����ѹ��
	};
	WORD	m_wMarket;		//�г�
	WORD	m_wNum;			//��������
	BYTE	m_cAttrib;
};

struct CPS_DYNA_HEAD
{
	enum ATTR
	{
		SEQ_CPS =		1,			//���ѹ��
		HAS_TIME_BIAS = 2,			//ӵ��ʱ��ƫ��(������������ӵ����ͬʱ��)
		HAS_TICK_COUNT= 0x40,		//ӵ�гɽ�����
		HAS_AMOUNT =	0x80,		//�������޳ɽ���

		EQUITY =		0,			//��Ʊ����������Ʒ��
		FUTURE =		0x8,		//��Ʒ�ڻ����ڻ�Ʒ��
		FOREIGN_EXG	=	0x10,		//����ֻ�п��ߵ�������һ�۵�Ʒ��
		HKSEC =			0x18,		//�۹ɵ������̼۸���������
		FTR_IDX =		0x20,		//�ڻ�ָ������5�������̺ͳֲ���������۵�Ʒ��
		TYPEMASK =		0x38,
	};
	BYTE	m_cCRC;			//CRCУ���
	WORD	m_wMarket;		//�г�
	WORD	m_wNum;			//��������
	time_t	m_baseTime;		//��׼ʱ�䣬���е�ʱ��ƫ�������ǻ��ڸ�ʱ��
	BYTE	m_cAttrib;		//����
	BYTE	m_cVer;			//ѹ���汾
};


//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct bufelement
{
    int port_tag;
    char timestamp[16];
    int len;
    char * iproto;
    int saddrbyte1;
    int saddrbyte2;
    int saddrbyte3;
    int saddrbyte4;
    int sport;
    int daddrbyte1;
    int daddrbyte2;
    int daddrbyte3;
    int daddrbyte4;
    int dport;
    char *flags;
    char *dctype;
    unsigned long seqtag;
}bufelement;

typedef struct PacketItem
{
	int thread_tag;
    int port_tag;
    //long long seqtag;
    struct pcap_pkthdr header;
    unsigned char *data;
}PacketItem;


typedef struct CapNetPacketItem
{
	struct pcap_pkthdr header;
	//unsigned char data[PCAPTOPARSE_BUF_SIZE];
	unsigned char *data;
}CapNetPacketItem;

typedef struct Lua_ZMQ_MSG_Item
{
	int dc_type;
	int dc_general_intype;
	int stk_num;
	int struct_size;
	int did_template_id;
	unsigned long pack_len;
	struct STK_STATIC * stk_static;
	unsigned char *pdcdata;
}Lua_ZMQ_MSG_Item;

typedef struct ThreadFuncParams
{
    string filter;
    int threadtag;
//    pthread_mutex_t *mutex_cp;
//    deque<pcap_work_item> *dbuf;
    zmq::context_t *context;
//    zmq::socket_t * sock;
    void *args;
}ThreadFuncParams;

typedef struct CapInnerThreadParam
{
	int tcpconntag;
	int tcpconnstatus;
	int tcpdisconntag;
	int tcpdisconnstatus;
	int fintag;
	int acktag;
}CapInnerThreadParam;

typedef struct LogParam
{
    int logfilenum;
//    deque<pcap_work_item> *dbuf_cp;
//    deque<bufelement> *dbuf_pl;
    zmq::context_t *context;
}LogParam;

typedef struct ParseParam
{
    int threadtag;
    zmq::context_t *context;
}ParseParam;

typedef  struct ThreadFuncLogParam
{
    deque<bufelement> *dbuf_pl;
    log4cpp::Category *log;
}ThreadFuncLogParam;

typedef struct ZMQItem
{
	int zmqpattern;
	std::string zmqsocketaction;
	std::string zmqsocketaddr;
	ZMQItem():zmqpattern(-1),zmqsocketaction(""),zmqsocketaddr(""){};
	~ZMQItem(){};
	ZMQItem(const ZMQItem & zmqitem)
	{
		zmqpattern = zmqitem.zmqpattern;
		zmqsocketaction = zmqitem.zmqsocketaction;
		zmqsocketaddr = zmqitem.zmqsocketaddr;
	}

}ZMQItem;

typedef struct DidStruct
{
	int id;
	unsigned int whole_tag;
	unsigned int compress_tag;
	std::string file_path;
}DidStruct;

#pragma pack(pop)

#endif // MONITOR_PROTOCOL_H_

