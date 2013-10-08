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

//�г�����

#define		MKT_SH			0x4853	//'HS'		// �Ϻ�֤ȯ������
#define		MKT_SZ			0x5A53	//'ZS'		// ����֤ȯ������
#define		MKT_HK			0x4B48	//'KH'		// ���������
#define		MKT_TW			0x5754	//'WT'		// ̨��֤ȯ������
#define		MKT_NY			0x594E	//'YN'		// ŦԼ֤ȯ������
#define		MKT_NSQ			0x534E	//'SN'		// Nasdaq֤ȯ������

#define		MKT_FE			0x4546	//'EF'		// ���

#define		MKT_SC			0x4353	//'CS'		// �Ϻ��ڽ���
#define		MKT_ZC			0x435A	//'CZ'		// ֣���ڽ���
#define		MKT_DC			0x4344	//'CD'		// �����ڽ���
#define		MKT_CC			0x4343	//'CC'		// ֥�Ӹ��ڻ�
#define		MKT_SF			0x4653	//'FS'		//�Ϻ������ڻ�������
#define		MKT_SG			0x4753	//'GS'		//�Ϻ��ƽ��ֻ�������
#define		MKT_BI			0x2442	//'$B'		// ���ָ��
#define		MKT_UI			0x2424	//'$$'		// �Զ���ָ��
#define		MKT_FI			0x4946	//'IF'		// �Ͻ����̶�����ƽ̨
#define		MKT_IX			0x5849	//'XI'		// ȫ����Ҫ�г�ָ��
#define		MKT_ZI			0x495A	//'IZ'		// ��ָ֤��
#define		MKT_NW			0x574E	//'WN'		// �����г�
#define		MKT_HS			0x5348	//'SH'		// H���г����Ͻ����ṩ��H�ɹɼ��г�
#define		MKT_BO			0x4F42	//'OB'		// ������Ʒ������
#define		MKT_DSM			0x2323	//'##'		//��ؿͻ���
#define		MKT_SS			0x5353	//'SS'		//�Ϻ������г�
#define		MKT_Z$			0x245A	//'$Z'		//����ȫ�����г�
#define		MKT_TE			0x4554	//'ET'		// ̨���������

enum THREAD_TAG
{
    NORMAL = 0,
    RESET = 1
};

enum STK_SUBTYPE
{
    NILTYPE = 0,
    ASHARE	= 'A',			//A��,����STOCK,WARRANT��Ч
    BSHARE	= 'B',			//B��,����STOCK,WARRANT��Ч
    GOV_BOND = 'G',			//��ծ,����BOND��Ч
    ENT_BOND = 'O',			//��ҵծ,����BOND��Ч
    FIN_BOND = 'F',			//����ծ,����BOND��Ч
};
enum STK_SP//��Ʊ����
{
    NULLSP = 0,
    NSP	= 'N',//����
    SSP	= 'S',//ST��
    PSP = 'P',//PT��
    TSP = 'T',//����ת��֤ȯ
    LSP = 'L',//���п����ͻ���LOF��
    OSP = 'O',//����ʾ��ֵ�Ŀ���ʽ����
    FSP = 'F',//�ǽ����Ϳ���ʽ����
    ESP = 'E',//ETF
    ZSP = 'Z',//�������������ڵ�֤ȯ
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
