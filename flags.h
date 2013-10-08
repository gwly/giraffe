#ifndef MONITOR_FLAGS_H_
#define MONITOR_FLAGS_H_

#include <time.h>
#include <iostream>
#include <log4cxx/rollingfileappender.h>

static const char CONFIG_FILE[10] = "conf.xml";
static const unsigned long DELTA_DISCONNECT_TIME_USEC = 10000000;//10s 
static const int BUFFERSIZE = 1024;
static const int DC_HEAD_LEN = 10;
static const int CAP_PACK_BUF_SIZE = 4096;
static const int ZMQ_RCVHWM_SIZE = 1024*1024;
static const int ZMQ_SNDHWM_SIZE = 1024*1024;

typedef long long               int64;
typedef int                       BOOL;
typedef unsigned short       WORD;
typedef float                     FLOAT;
typedef int                        INT;
typedef unsigned int           UINT;

typedef const char * LPCSTR;
typedef unsigned long ULONG_PTR, *PULONG_PTR;
typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;

#ifndef INT64
#if defined(WIN32)
typedef unsigned __int64 UINT64;
typedef __int64 INT64;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef unsigned int  UINT32;
#else
typedef unsigned long long UINT64;
typedef long long INT64;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef unsigned int  UINT32;

#endif
#endif

//市场定义

#define		MKT_SH			0x4853	//'HS'		// 上海证券交易所
#define		MKT_SZ			0x5A53	//'ZS'		// 深圳证券交易所
#define		MKT_HK			0x4B48	//'KH'		// 香港联交所
#define		MKT_TW			0x5754	//'WT'		// 台湾证券交易所
#define		MKT_NY			0x594E	//'YN'		// 纽约证券交易所
#define		MKT_NSQ			0x534E	//'SN'		// Nasdaq证券交易所

#define		MKT_FE			0x4546	//'EF'		// 外汇

#define		MKT_SC			0x4353	//'CS'		// 上海期交所
#define		MKT_ZC			0x435A	//'CZ'		// 郑州期交所
#define		MKT_DC			0x4344	//'CD'		// 大连期交所
#define		MKT_CC			0x4343	//'CC'		// 芝加哥期货
#define		MKT_SF			0x4653	//'FS'		//上海金融期货交易所
#define		MKT_SG			0x4753	//'GS'		//上海黄金现货交易所
#define		MKT_BI			0x2442	//'$B'		// 板块指数
#define		MKT_UI			0x2424	//'$$'		// 自定义指数
#define		MKT_FI			0x4946	//'IF'		// 上交所固定收益平台
#define		MKT_IX			0x5849	//'XI'		// 全球主要市场指数
#define		MKT_ZI			0x495A	//'IZ'		// 中证指数
#define		MKT_NW			0x574E	//'WN'		// 新闻市场
#define		MKT_HS			0x5348	//'SH'		// H股市场，上交所提供的H股股价市场
#define		MKT_BO			0x4F42	//'OB'		// 渤海商品交易所
#define		MKT_DSM			0x2323	//'##'		//监控客户端
#define		MKT_SS			0x5353	//'SS'		//上海航运市场
#define		MKT_Z$			0x245A	//'$Z'		//深圳全队列市场
#define		MKT_TE			0x4554	//'ET'		// 台湾柜买中心

enum THREAD_TAG
{
    NORMAL = 0,
    RESET = 1
};

enum STK_SUBTYPE
{
    NILTYPE = 0,
    ASHARE	= 'A',			//A股,仅对STOCK,WARRANT有效
    BSHARE	= 'B',			//B股,仅对STOCK,WARRANT有效
    GOV_BOND = 'G',			//国债,仅对BOND有效
    ENT_BOND = 'O',			//企业债,仅对BOND有效
    FIN_BOND = 'F',			//金融债,仅对BOND有效
};
enum STK_SP//股票属性
{
    NULLSP = 0,
    NSP	= 'N',//正常
    SSP	= 'S',//ST股
    PSP = 'P',//PT股
    TSP = 'T',//代办转让证券
    LSP = 'L',//上市开放型基金（LOF）
    OSP = 'O',//仅揭示净值的开放式基金
    FSP = 'F',//非交易型开放式基金
    ESP = 'E',//ETF
    ZSP = 'Z',//处于退市整理期的证券
};


enum Lv4protocol
{
    TCP = 6,
    UDP = 17
};

enum IpFlags
{
    FIN = 0x01,
    SYN = 0x02,
    RST = 0x04,
    PSH = 0x08,
    ACK = 0x10,
    FINACK = 0x11,
    SYNACK = 0x12,
    RSTACK = 0x14,
    PSHACK = 0x18,
    FINPSHACK = 0x19,
    URG = 0x20
};


#endif // MONITOR_FLAGS_H_
