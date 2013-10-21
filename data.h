#ifndef _DATA_H_
#define _DATA_H_

#include "datacollect.h"

#pragma pack(push,1)


#define LOG_MAXLINE 1024

#define MY_VERSION "wdc_gen_xun_v2.0.0_130123_b1_x32"

#define CFG_FILE "config.xml"
#define LOG_FILE "datacollect.log"
#define MY_NAME  "DaZhiHui DataCollector"

#define MAX_SENDBUF      (1024*16)
#define MAX_RECVBUF      (1024*1024)
#define EXPAND_BUF       (2*1024*1024)
#define MAX_WRITEFILEBUF (1024*1024)
#define MAX_MARKET_RECVBUF (2*1024*1024)

#define FILE_MODE 0666

struct TQ_Config
{
	char m_strTQServer[32];
	int  m_nTQPort;
	unsigned char m_byShow2003Flag;
    int           m_nMaxShow2003Size;
	unsigned char m_bySjshqFlag;
    int           m_nMaxSjshqSize;
	unsigned char m_bySjsxxFlag;
    int           m_nMaxSjsxxSize;
	unsigned char m_byStbhqFlag;
    int           m_nMaxStbhqSize;
	unsigned char m_byGzlxFlag;
    int           m_nMaxGzlxSize;
    unsigned char m_bySHQzxxFlag;   //2006-10-18 增加
    int           m_nMaxSHQzxxSize;
    unsigned char m_byHTIndexFlag;   //2006-10-18 增加
    int           m_nMaxHTIndexSize;
	unsigned char m_byHghqFlag;
	int           m_nMaxHghqSize;
	unsigned char m_byHgxxnFlag;
    int           m_nMaxHgxxnSize;
};


struct DS_Config
{
    char m_strServer[32];
    int  m_nPort;
    char m_strUser[16];
    char m_strPass[24];
	BOOL m_bCompress;
	int  m_nEncyType;
};

struct MDF_Config
{
    char m_strServer[32];
    int  m_nPort;
    char m_strUser[20];
    char m_strPass[8];
	char m_strPK1[8];
	char m_strPK2[8];
	char m_strPK3[8];
};

struct WF_Config
{
	int  m_nMaxFiles;            //最大文件数
	int  m_bCompress;            //是否压缩
	int  m_nDelay;               //延时(毫秒)
	int  m_nTimeout;             //写控制文件超时(毫秒)
	int  m_nFullImageDelay;      //完整行情时间间隔(秒)
	int  m_nEncyType;            //加密类型
	char m_strConFile[64];       //控制文件名
	char m_strWritePath[64];     //输出文件目录
};

struct Spip_Config
{
    int  m_nMaxFiles;            //最大文件数
    int  m_bCompress;            //是否压缩
    int  m_nDelay;               //采样时间间隔（毫秒）
    int  m_nFullImageDelay;      //完整行情采样间隔（秒）
    int  m_nEncyType;            //加密方式
    char m_strWritePath[64];     //文件落盘路径，空则不落盘
    char m_strSpipServer[32];    //SPIP服务器地址
    int  m_nSpipPort;            //SPIP端口
    char m_strSpipUser[16];      //SPIP用户名
    char m_strSpipPwd[16];       //SPIP用户密码
};

struct SM_Type
{
    WORD m_wType;
	char m_strTypeName[16];
    WORD m_wFxjType;      //分析家类型
    WORD m_wSubType;      //
    WORD m_wAmtUnit;      //行情库金额单位
    WORD m_wVolUnit;      //行情库数量单位
    WORD m_wPerHand;      //每手股数
    WORD m_wMMPAmtUnit;   //行情库买卖盘数量单位
    WORD m_wPriceDigit;   //价格小数位数
    char m_chStamp[7][10];
};

//Level2扩展静态数据
struct STATIC_Ex
{
	char 	m_strStkCode[10];
    STK_STATICEx m_StkStaticEx;
};

struct DC_STKSTATIC_MY
{
	DWORD	m_dwVersion;	//静态数据版本,每次静态数据发生变化则必须增加数值,每个行情源最好有一个相对独立的高位，这样不同行情源就不会有相同的版本
	WORD	m_wAttrib;		//版本属性
	UINT	m_nDay;			//静态库日期，YYYYMMDD
	short	m_nNum;			//m_data数量
};


struct DC_STKSTATIC_Ex_MY
{
	short	m_nNum;
};

struct DC_STKDYNA_MY
{
	WORD	m_wDynaSeq;		//动态行情序列号，用于客户端转发
	short   m_nNum;
};

struct DC_SHL2_MMPEx_MY
{
	short	m_nNum;
};

struct DC_SHL2_REPORT_MY
{
	short	m_nNum;			//数量
};

struct DC_SHL2_BIG_WD_MY
{
	char	m_cBidOrAsk;	//'B'表示买单，'A'表示卖单
	short	m_nNum;			//数量
};

struct DC_SHL2_ACCU_WD_MY
{
	char	m_cBidOrAsk;	//'B'表示买单，'A'表示卖单
	short	m_nNum;			//数量
};

struct DC_SZL2_ORDER_STAT_MY
{
	short m_nNum;
	time_t m_time;
};

struct DC_GENERAL_MY
{
	WORD	m_wDataID;			//数据ID
	WORD	m_wMarket;			//市场
	DWORD	m_dwDataAttr;		//数据属性
	short	m_nDataSize;		//每个数据的长度,对于变长数据,表示每个数据的最大值(0表示没有最大值，任意长度)
	UINT24	m_nTotalLen;		//包含本数据头在内的数据总长度
	WORD	m_wNum;				//数量
	DWORD	m_dwReserved;		//保留，置为0
};
struct L2_Order_Queue
{
    BOOL    m_bHasFull;
    DWORD   m_dwBuyLen;
    DWORD   m_dwBuyValue[249];
    DWORD   m_dwSellLen;
    DWORD   m_dwSellValue[249];
};

struct L2_Order
{
    DWORD m_dwPrice;
    DWORD m_dwOrders;
    DWORD m_dwOrder[400];
    void deleteOrder(int index)
    {
        m_dwOrder[index] = 0;
    }
    void addOrder(DWORD order)
    {
        for (int i=399;i>=0;i--)
        {
            if (m_dwOrder[i] > 0)
            {
                m_dwOrder[i+1] = order;
                return;
            }
        }
        m_dwOrder[0] = order;//如果旧委托全部删除，
    }
    void updateOrder(int index, DWORD order)
    {
        m_dwOrder[index] = order;
    }
};
struct L2_Order_Queue_Raw
{
    L2_Order m_sOrder[10];
    int updatePrice(DWORD dwPrice, DWORD dwOrders)
    {
        for (int i=0;i<10;i++)
        {
            if (m_sOrder[i].m_dwPrice == dwPrice)
            {
                m_sOrder[i].m_dwOrders = dwOrders;
                return i;
            }
        }
        return -1;
    }
    void deletePrice(DWORD dwPrice)
    {//删除一个价格档位，就把那个档位的委托数置0
        for(int i=0;i<10;i++)
        {
            if (m_sOrder[i].m_dwPrice == dwPrice)
            {
                m_sOrder[i].m_dwOrders = 0;
                return;
            }
        }
    }
    int addPrice(DWORD dwPrice, DWORD dwOrders, BOOL isBuy)
    {//增加一个价格档位,买：从高往低排序，卖：从低向高排序
        if (isBuy)
        {
            for (int i=0;i<10;i++)
            {
                if (m_sOrder[i].m_dwPrice < dwPrice)
                {
                    memmove(&m_sOrder[i+1], &m_sOrder[i], sizeof(L2_Order)*(9-i));
                    memset(&m_sOrder[i], 0, sizeof(L2_Order));
                    m_sOrder[i].m_dwPrice = dwPrice;
                    m_sOrder[i].m_dwOrders= dwOrders;
                    return i;
                }
            }
        }else
        {
            for (int i=0;i<10;i++)
            {
                if (m_sOrder[i].m_dwPrice > 0)
                {
                    if (m_sOrder[i].m_dwPrice > dwPrice)
                    {
                        memmove(&m_sOrder[i+1], &m_sOrder[i], sizeof(L2_Order)*(9-i));
                        memset(&m_sOrder[i], 0, sizeof(L2_Order));
                        m_sOrder[i].m_dwPrice = dwPrice;
                        m_sOrder[i].m_dwOrders= dwOrders;
                        return i;
                    }
                }else
                {
                    m_sOrder[i].m_dwPrice = dwPrice;
                    m_sOrder[i].m_dwOrders= dwOrders;
                    return i;
                }
            }
        }
        return -1;

    }
};

#define MAX_MMP_COUNT 25
struct SHL2_MMP
{
	DWORD m_dwBuyPrice[MAX_MMP_COUNT];
	DWORD m_dwBuyVol[MAX_MMP_COUNT];
	DWORD m_dwSellPrice[MAX_MMP_COUNT];
	DWORD m_dwSellVol[MAX_MMP_COUNT];
	SHL2_MMP()
	{
		memset(this, 0, sizeof(SHL2_MMP));
	}
	//把10档买卖盘数据拷入
	void  setMMP(STK_DYNA* pDyna, SH_L2_MMPEX* pDynaEx)
	{
		int nBuyIndex = 0, nSellIndex = 0;
		for (int i=0;i<10;i++)
		{
			if (i < 5)
			{//0-4
				if (pDyna->m_dwBuyPrice[i] > 0)
				{
					m_dwBuyPrice[nBuyIndex] = pDyna->m_dwBuyPrice[i];
					m_dwBuyVol[nBuyIndex]   = pDyna->m_dwBuyVol[i];

					nBuyIndex++;
				}

				if (pDyna->m_dwSellPrice[i] > 0)
				{
					m_dwSellPrice[nSellIndex] = pDyna->m_dwSellPrice[i];
					m_dwSellVol[nSellIndex]   = pDyna->m_dwSellVol[i];

					nSellIndex++;
				}

			}else
			{//5-9
				if (pDynaEx->m_dwBuyPrice[i-5] > 0)
				{
					m_dwBuyPrice[nBuyIndex] = pDynaEx->m_dwBuyPrice[i-5];
					m_dwBuyVol[nBuyIndex]   = pDynaEx->m_dwBuyVol[i-5];

					nBuyIndex++;
				}

				if (pDyna->m_dwSellPrice[i-5] > 0)
				{
					m_dwSellPrice[nSellIndex] = pDynaEx->m_dwSellPrice[i-5];
					m_dwSellVol[nSellIndex]   = pDynaEx->m_dwSellVol[i-5];

					nSellIndex++;
				}
			}
		}

	}

	void getMMP(STK_DYNA* pDyna, SH_L2_MMPEX* pDynaEx)
	{
		int nBuyIndex = 0, nSellIndex = 0;
		for (int i=0; i<MAX_MMP_COUNT; i++)
		{
			if (m_dwBuyPrice[i] > 0)
			{
				if (nBuyIndex < 5)
				{
					pDyna->m_dwBuyPrice[nBuyIndex] = m_dwBuyPrice[i];
					pDyna->m_dwBuyVol[nBuyIndex]   = m_dwBuyVol[i];
				}else
				{
					pDynaEx->m_dwBuyPrice[nBuyIndex-5] = m_dwBuyPrice[i];
					pDynaEx->m_dwBuyVol[nBuyIndex-5]   = m_dwBuyVol[i];
				}
				nBuyIndex++;
			}

			if (m_dwSellPrice[i] > 0)
			{
				if (nSellIndex < 5)
				{
					pDyna->m_dwSellPrice[nSellIndex] = m_dwSellPrice[i];
					pDyna->m_dwSellVol[nSellIndex]   = m_dwSellVol[i];
				}else
				{
					pDynaEx->m_dwSellPrice[nSellIndex-5] = m_dwSellPrice[i];
					pDynaEx->m_dwSellVol[nSellIndex-5]   = m_dwSellVol[i];
				}
				nSellIndex++;
			}

		}

	}

	//新加一个买价格,价格从大到小排序
	void addBuyPrice(DWORD dwPrice, DWORD dwVolume)
	{
		for (int i=0;i<MAX_MMP_COUNT;i++)
		{
			if (dwPrice > m_dwBuyPrice[i])
			{
				memmove(&m_dwBuyPrice[i+1], &m_dwBuyPrice[i], sizeof(DWORD)*(MAX_MMP_COUNT-1-i));
				memmove(&m_dwBuyVol[i+1], &m_dwBuyVol[i], sizeof(DWORD)*(MAX_MMP_COUNT-1-i));
				m_dwBuyPrice[i] = dwPrice;
				m_dwBuyVol[i]   = dwVolume;
				break;
			}
		}
	}

	void deleteBuyPrice(DWORD dwPrice)
	{
		for (int i=0; i<MAX_MMP_COUNT; i++)
		{
			if (dwPrice == m_dwBuyPrice[i])
			{
				memmove(&m_dwBuyPrice[i], &m_dwBuyPrice[i+1], sizeof(DWORD)*(MAX_MMP_COUNT-1-i));
				memmove(&m_dwBuyVol[i], &m_dwBuyVol[i+1], sizeof(DWORD)*(MAX_MMP_COUNT-1-i));
				m_dwBuyPrice[MAX_MMP_COUNT - 1] = 0;
				m_dwBuyVol[MAX_MMP_COUNT - 1] = 0;
				break;
			}
		}
	}

	void updateBuyPrice(DWORD dwPrice, DWORD dwVolume)
	{
		for (int i=0; i<MAX_MMP_COUNT; i++)
		{
			if (dwPrice == m_dwBuyPrice[i])
			{
				m_dwBuyVol[i] = dwVolume;
				break;
			}
		}
	}

	//新加一个卖价格,价格从小到大排序
	void addSellPrice(DWORD dwPrice, DWORD dwVolume)
	{
		for (int i=0;i<MAX_MMP_COUNT;i++)
		{
			if (m_dwSellPrice[i] > 0)
			{
				if (dwPrice < m_dwSellPrice[i])
				{
					memmove(&m_dwSellPrice[i+1], &m_dwSellPrice[i], sizeof(DWORD)*(MAX_MMP_COUNT-1-i));
					memmove(&m_dwSellVol[i+1], &m_dwSellVol[i], sizeof(DWORD)*(MAX_MMP_COUNT-1-i));
					m_dwSellPrice[i] = dwPrice;
					m_dwSellVol[i]   = dwVolume;
					break;
				}
			}else
			{
				m_dwSellPrice[i] = dwPrice;
				m_dwSellVol[i]   = dwVolume;
				break;
			}
		}
	}
	void deleteSellPrice(DWORD dwPrice)
	{
		for (int i=0; i<MAX_MMP_COUNT; i++)
		{
			if (dwPrice == m_dwSellPrice[i])
			{
				memmove(&m_dwSellPrice[i], &m_dwSellPrice[i+1], sizeof(DWORD)*(MAX_MMP_COUNT-1-i));
				memmove(&m_dwSellVol[i], &m_dwSellVol[i+1], sizeof(DWORD)*(MAX_MMP_COUNT-1-i));
				m_dwSellPrice[MAX_MMP_COUNT - 1] = 0;
				m_dwSellVol[MAX_MMP_COUNT - 1] = 0;
				break;
			}
		}
	}
	void updateSellPrice(DWORD dwPrice, DWORD dwVolume)
	{
		for (int i=0; i<MAX_MMP_COUNT; i++)
		{
			if (dwPrice == m_dwSellPrice[i])
			{
				m_dwSellVol[i] = dwVolume;
				break;
			}
		}
	}

};

struct ZX_ID
{
    union
    {
        INT64 int64ID;
        struct
        {
            INT64 m_type:18;
            INT64 m_id  :16;
            INT64 m_time:30;
        };

    };
};

struct RAW_FILE_INFO
{
    char m_strFileName[32];
    int  m_mttime;
    int  m_nBufferLen;
    char* m_pBuffer;
};

struct DataHead
{
    WORD  m_wHeadID;      // 0x9988       2
    WORD  m_wVersion;     // 0x0101       4
    WORD  m_wDataType;    // Main Class   6
    DWORD m_dwDataLength; //Frame Length  10
    char  m_pcReserve[2]; //              12
};

struct MarketData
{
    DWORD m_wMarketCode;        //市场代码
    WORD m_wMarketProtocol;     //协议
    int  m_nMarketStatus;       //状态
    int  m_nHQChanged;          //行情变化数
    char m_strDescrible[128];   //描述
};

struct ClientData
{
    char  m_strUserName[16];     //用户
    char  m_strAddress[16];     //地址
    WORD  m_wPort;              //端口
    DWORD  m_wMarketCode;       //市场
    DWORD m_dwRecvBytes;        //接收
    DWORD m_dwSendBytes;        //发送
    DWORD m_dwRecvSppeed;       //接收带宽
    int   m_tLoginTime;         //连接时间
};

struct UserPassIP
{
    DC_USERPASS userpass;
    DWORD       ip;
};

typedef struct
{
    BYTE res0;
    BYTE year;
    BYTE mon;
    BYTE day;
    DWORD recnum;
    WORD headlen;
    WORD recordlen;
    BYTE res1[21];
} dbfhead;

typedef struct
{
    char name[11];
    char type;
    WORD st;
    char res1[2];
    char len;
    char pre;
    char res2[14];
} dbfstru;

struct MY_SZL2_ORDER_STAT
{
	short	m_nNum;					//数量
	time_t	m_time;					//数据时间
	SZ_L2_ORDER_STAT	m_data[400];//共有m_nNum个
};

struct FE_CODE_LIST
{
	char m_strCode[10];
	char m_strName[32];
	char m_strDBCode[16];
	char m_strDBName[64];
	int  m_nShift;
};

#pragma pack(pop)

#endif


