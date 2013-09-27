/*--------------------------------------------------------------------------
Copyright(C) Shanghai Great Wisdom Co. Ltd (DZH)
归类：数据采集压缩
说明：基本数据定义
--------------------------------------------------------------------------*/
#ifndef _DATACOLLECT_H_INCLUDE
#define _DATACOLLECT_H_INCLUDE

/*
修改历史：
 08/4/29  No.5.d No.6.b No.8  No.11.c DC_LOGIN增加m_wPriority DC_READY增加SERVER_NOTREADY,m_dwDelayConnectTime
 08/6/20  STK_DYNA增加 m_mInnerVol内盘量数据,由服务器处理，数据采集设置为0即可
 08/7/31   DC_READY增加IDLE指令，修改登陆流程No.5,No.6，修改DC_KEEPALIVE参数含义
 08/7/31  增加 DC_GENERAL数据类型和STK_HKDYNA
 08/8/10  DC_XML 增加m_dwData数据ID
 08/8/14  修改登陆过程，支持服务器向采数机连接
 08/8/14  增加DC_USERSTAT客户统计数据
 08/01/24 增加DC_RAWDATA原始数据，DC_NEWS数据支持
 08/08/08 增加混合时段市场支持，用DC_RAWDATA传送，参见 MK_VARTIME
 08/11/25
	1.增加深圳Level2逐笔委托数据类型， DCT_SZL2_ORDER_QUEUE,该类数据与买卖盘队列一样使用 DC_SHL2_QUEUE 数据结构和压缩方法
	2.DCT_LOGIN 增加 DCATTR_SZLEVEL2 标志，表示该市场具有level2逐笔委托数据，服务器需要准备该类数据
 09.2.17
	增加RAW数据源向采数端传送数据功能，用于对其下属的服务端进行广播(带有RAW_BROADCAST标志)，DC_RAWDATA数据增加 RAW_BROADCAST 广播标志
 09.4.10  RAW数据增加权限定义标志 RAW_AUTHORIZE
		  增加DZH_AUTHORIZE结构用于描述授权结构
 09.7.23  增加深圳Level2逐笔委托统计数据类型,DCT_SZL2_ORDER_STAT
 09.7.25  RAW格式数据增加个股一般数据：GNLxxxxx.GNL
 12.10.19 
		1.增加DC_DSCRC
		2.增加DC_DSDID
		3.增加DC_DSDATA
		4.增加DC_DID数据,增加两种压缩DC_XMLCID_CPS，DC_SOCID_CPS
 13.01.17
		1,STK_STATIC结构的STK_TYPE里增加基金的具体分类
 13.04,01
		1,增加DCT_DIDSTATIC，增加标记
		2,增加DCT_INCODESTATIC
		3,增加DCATTR_INCODESTATIC,DCATTR_NOSTATIC,DCATTR_DIDSTATIC标记
		多种STATIC数据并存时：
			发送顺序，DCT_STKSTATIC--DCT_DIDSTATIC--DCT_INCODESTATIC
			STKID在多种STATIC数据要相等
			初始化条件为多种STATIC数据里日期都相等而且不等于本地保存的数据日期
			发送某只STKID其它数据前，要保证该只STKID相应多种STATIC数据已发送

*/

#pragma pack(push,1)

#include "mword.h"	//定义 MWORD
//////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////
//通用结构

struct UINT24
{
	WORD m_wLow;
	BYTE m_cHigh;
	DWORD Get() const {return m_wLow + (m_cHigh<<16);}
	void Set(DWORD dw){m_wLow = (WORD)dw;m_cHigh = (BYTE)(dw>>16);}
	void operator=(DWORD dw){Set(dw);}
	BOOL operator==(DWORD dw) const {return Get()==dw;}
	operator DWORD() const {return Get();}
};

//静态数据，码表
struct STK_STATIC
{
	enum STK_TYPE
	{
		INDEX = 0,				//指数
		STOCK = 1,				//股票
		FUND = 2,				//基金
		BOND = 3,				//债券
		OTHER_STOCK = 4,		//其它股票
		OPTION = 5,				//选择权
		EXCHANGE = 6,			//外汇
		FUTURE = 7,				//期货
		FTR_IDX = 8,			//期指
		RGZ = 9,				//认购证
		ETF = 10,				//ETF
		LOF = 11,				//LOF
		COV_BOND = 12,			//可转债
		TRUST = 13,				//信托
		WARRANT = 14,			//权证
		REPO = 15,				//回购
		COMM = 16,				//商品现货
		STOCKFUND = 17,			//股票基金
		BONDFUND = 18,			//债券基金
		HYBRIDFUND = 19,		//混合型基金
		CPTPSVFUND = 20,		//保本型基金 Capital preservation
		MONEYFUND = 21,			//货币一般基金
		NVMONEYFUND = 22,		//货币净值基金,net value
		SMFUND = 23,			//短期理财基金
		QDIIFUND = 24,			//QDII基金
	};
	WORD	m_wStkID;			//本市场内唯一标示,在本市场内的序号
	char	m_strLabel[10];		//代码

	char	m_strName[32];		//名称
	BYTE	m_cType;			//STK_TYPE
	BYTE	m_nPriceDigit;		//价格最小分辨率，非常重要，每一个DWORD类型的价格都要除以10^m_nPriceDigit才是真正的价格
	short	m_nVolUnit;			//成交量单位，每一成交量单位表示多少股
	MWORD	m_mFloatIssued;		//流通股本
	MWORD	m_mTotalIssued;		//总股本
	
	DWORD	m_dwLastClose;		//昨收
	DWORD	m_dwAdvStop;		//涨停
	DWORD	m_dwDecStop;		//跌停

	int		GetPriceMul() const;		//价格乘数，由m_nPriceDigit决定
};

//扩展静态数据,作为静态数据的补充，不是每一个股票都必须的，可以选择性生成部分股票的数据，在发送STK_STATIC后发送,使用DCT_GENERAL发送
struct STK_STATICEx
{
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
	char	m_cType;			//STK_TYPE,由该标志决定联合中使用哪一个结构
	char	m_cSubType;			//STK_SUBTYPE
	union
	{
		struct		//股票	(STOCK,OTHER_STOCK)
		{
			float	m_fFaceValue;		//	面值
			float	m_fProfit;			//	每股收益
			WORD	m_wIndustry;		//	行业'A' -- 'M',参见 industryClasify
			char	m_cTradeStatus;		//	交易状态，'N'=Normal, 'H'=Halt, 'X'=Not trade on this Market
			float	m_fCashDividend;	//	每股红利
			char	m_cSecurityProperties;//特殊分类标记，取值集合STK_SP中的值
			DWORD	m_dwLastTradeDate;//最后交易日，目前为空，做预留用

		} m_equitySpec;
		struct		//基金,ETF,LOF	(FUND,ETF,LOF)
		{
			float	m_fFaceValue;		//	面值
			float	m_fTotalIssued;		//	总股本
			float	m_fIOPV;			//	IOPV净值,仅对ETF,LOF有效，***
		} m_fundSpec;
		struct		//权证,选择权	(OPTION,WARRANT)
		{
			char	m_cStyle;			//  'A' or 'E'	American or European Style
			char	m_cCP;				//	'C' or 'P' Call or Put
			float	m_fCnvtRatio;		//  兑换/行权比例
			float	m_fStrikePrice;		//	行权价格
			DWORD	m_dwMaturityDate;	//	到期日,YYYYMMDD
			char	m_strUnderLine[12];	//	对应股票,包含市场代码，如SH600000
			float	m_fBalance;			//  流通余额
		} m_warrantSpec;
		struct		//债券	(BOND)
		{
			DWORD	m_dwMaturityDate;	//	到期日,YYYYMMDD
			DWORD	m_dwIntAccruDate;	//	起息日，YYYYMMDD
			float	m_fIssuePrice;		//	发行价
			float	m_fCouponRate;		//	利率
			float	m_fFaceValue;		//	面值
			float	m_fAccruedInt;		//	应计利息,***
		} m_bondSpec;
		struct		//可转债	(COV_BOND)
		{
			char	m_cStyle;			//  'A' or 'E'	American or European Style
			char	m_cCP;				//	'C' or 'P' Call or Put
			float	m_fCnvtRatio;		//  兑换/行权比例
			float	m_fStrikePrice;		//	行权价格
			DWORD	m_dwMaturityDate;	//	到期日,YYYYMMDD
			char	m_strUnderLine[12];	//	对应股票,包含市场代码，如SH600000
			float	m_fAccruedInt;		//	应计利息
		} m_CnvtSpec;
		struct		//期货,商品	(FUTURE,FTR_IDX,COMM)
		{
			DWORD	m_dwLastDayOI;			//昨日持仓量
			float	m_fLastSettlePrice;		//昨日结算价
		} m_futureSpec;
		struct		//信托	(TRUST)
		{
			float	m_dwfAsset;			//净资产
			DWORD	m_dwAssetDate;		//YYYYMMDD
		} m_trustSpec;
	};
};

struct INDUSTRY_CLASIFY
{
	WORD m_wCode;
	const char* m_pName;
};

const INDUSTRY_CLASIFY industryClasify[] =
{
	{'A',"农牧业"},
	{'B',"采掘业"},
	{0x3043,"食品饮料"},	//C0
	{0x3143,"纺织服装"},	//C1
	{0x3243,"木材家具"},	//C2
	{0x3343,"造纸印刷"},	//C3
	{0x3443,"石油化工"},	//C4
	{0x3543,"电子"},		//C5
	{0x3643,"金属材料"},	//C6
	{0x3743,"机械仪表"},	//C7
	{0x3843,"生物医药"},	//C8
	{0x3943,"其他制造"},	//C9
	{'D',"供水供电"},
	{'E',"建筑业"},
	{'F',"交通仓储"},
	{'G',"信息技术"},
	{'H',"商业贸易"},
	{'I',"金融保险"},
	{'J',"房地产"},
	{'K',"社会服务"},
	{'L',"文化传播"},
	{'M',"综合类"}
};

//动态行情
struct STK_DYNA
{
	WORD	m_wStkID;			//股票ID
	time_t	m_time;				//成交时间
	DWORD	m_dwOpen;			//开盘
	DWORD	m_dwHigh;			//最高
	DWORD	m_dwLow;			//最低
	DWORD	m_dwNew;			//最新
	MWORD	m_mVolume;			//成交量
	MWORD	m_mAmount;			//成交额
	MWORD	m_mInnerVol;		//内盘成交量,<0表示该笔成交为主动卖，>=0表示主动买,绝对值表示内盘成交量
	DWORD	m_dwTickCount;			//累计成交笔数
	DWORD	m_dwBuyPrice[5];		//委买价格
	DWORD	m_dwBuyVol[5];			//委买量
	DWORD	m_dwSellPrice[5];		//委卖价格
	DWORD	m_dwSellVol[5];			//委卖量
	DWORD	m_dwOpenInterest;		//持仓量(期货期指特有)
	DWORD	m_dwSettlePrice;		//结算价(期货期指现货特有)
};

//港股动态数据,使用DCT_GENERAL发送
struct STK_HKDYNA
{
	DWORD	m_dwIEPrice;	//平衡价
	MWORD	m_mIEVolume;	//平衡量

	//买卖盘笔数
	WORD	m_wBuyOrderNum[5];
	WORD	m_wSellOrderNum[5];

	//经纪队列
	struct HK_BROKER	
	{
		WORD	m_wNumBroker;
		WORD	m_wBrokerNo[40];
		char	m_cType[40];		//'B':broker,'R':register trader,'S':number of spread
	}m_buyBroker,m_sellBroker;
};

//债券票面利率、应得利息，股票每股收益等使用STK_STATICEx传送

////////////////////////////////////////////////////////////////////////////////////////////////
//上海 Level2

//扩展买卖盘
#define  SHL2_MMP_SIZE	5

struct SH_L2_MMPEX
{
	WORD	m_wStkID;			//股票ID
	DWORD	m_dwAvgBuyPrice;	//加权平均委买价格
	MWORD	m_mAllBuyVol;		//委买总量
	DWORD	m_dwAvgSellPrice;	//加权平均委卖价格
	MWORD	m_mAllSellVol;		//委卖总量
	DWORD	m_dwBuyPrice[SHL2_MMP_SIZE];	//委买价6-10
	DWORD	m_dwBuyVol[SHL2_MMP_SIZE];		//委买量6-10
	DWORD	m_dwSellPrice[SHL2_MMP_SIZE];	//委卖价6-10
	DWORD	m_dwSellVol[SHL2_MMP_SIZE];		//委卖量6-10
};

//委托队列

//委托队列类型码定义，由每个DWORD的最高2位决定它的类型，每个股票的队列数据顺序为
//  股票ID | 买盘价格1 | 价格1总单数 | 买单1 | 买单2 ... | 买盘价格2 | 价格2总单数 | 买单1 | 买单2 ... | 卖盘价格1 | 价格1总单数 | 卖单1 | 卖单2 ... 
//价格后面总是跟着一个总单数

#define QUEUE_TYPE_MASK	0xC0000000

#define QUEUE_ORDER		0x0
#define QUEUE_BUYPRICE	0x40000000
#define QUEUE_SELLPRICE	0x80000000
#define QUEUE_STOCKID	0xC0000000

//包含增量委托队列

//委托队列类型码定义，由每个DWORD的最高2位决定它的类型，每个股票的队列数据顺序为,对于总单数的第三位决定其后跟单子还是跟Inorder数目 + InOrder
//  股票ID | 买盘价格1 | 价格1总单数 | 买单数目| 买单1 | 买单2 ... | 买盘价格2 | 价格2总单数 | Inorder数目| InOrder1 | InOrder2 ... | 卖盘价格1 | 价格1总单数 |卖单数目| 卖单1 | 卖单2 ... 
//价格后面总是跟着一个总单数,价格总单数后面总是跟着一个买单数目
#define QUEUE_ORDER_IN 0x20000000 //如果价格总单数add上这个值为1，表示该价位上总共有x个单子,其后跟着Inorder数目
//差量单子
struct InOrder
{
	BYTE type;//为0增单,为1部分成交,为2完全成交,为3撤单			
	WORD pos;//位置相对于前一个状态
	DWORD vol;//数量,股
};

//逐笔成交明细
struct SH_L2_REPORT
{
	WORD	m_wStkID;			
	time_t	m_time;				//成交时间
	DWORD	m_dwPrice;			//成交价格
	DWORD	m_dwVolume;			//成交量
};

//单笔最大撤单
struct SH_L2_BIG_WD			
{
	WORD	m_wStkID;
	time_t	m_time;				//撤单时间
	time_t	m_submitTime;		//委托时间
	DWORD	m_dwPrice;			//委托价格
	DWORD	m_dwWidthdrawVol;	//撤单量
};
	
//累计撤单
struct SH_L2_ACCU_WD	
{
	WORD	m_wStkID;			//股票ID
	time_t	m_time;				//时间
	DWORD	m_dwWidthdrawVol;	//撤单量
};

//委托单统计，用于深圳Level2
struct SZL2_ORDER_FIVE
{
	WORD   nIndex;
	char strOrderKind;
	char strFunCode;
	DWORD dwPrice;
	DWORD dwAmount;
	DWORD dwRecNO;
	int   nSetNO;
	DWORD   nRecTime;
};

/////////////////////////////////////////
struct SZL2_TRADE_FIVE
{
	WORD   nIndex;
	char strOrderKind;
	char strFunCode;
	DWORD dwBuyOrderRecNo;
	DWORD dwSellOrderRecNo;
	DWORD dwPrice;
	DWORD dwAmount;
	DWORD dwTradeNO;
	int   nSetNO;
	DWORD   nRecTime;
};

struct SZ_L2_ORDER_STAT
{
	WORD	m_wStkID;				//股票ID
	MWORD	m_nBuyOrderVol[4];		//买入单量，小、中、大、特大
	MWORD	m_nSellOrderVol[4];		//卖出单量，小、中、大、特大
	UINT24	m_dwOrderNum[2];		//委托单数量，买入/卖出
	MWORD	m_nWDVol[2];			//买入/卖出撤单量,现在暂时将无法判明方向的总撤单量各送一半

	void	Init(WORD wStkID);		//初始化并清零数据
	void	AddNewOrder(int nDir,DWORD dwPrice,DWORD dwVol,BYTE nPriceDigit);	//新增委托单时调用，nPriceDigit引用 STK_STATIC::m_nPriceDigit,nDir>0表示买单，<0表示卖单
	void	AddNewWithdarw(int nDir,DWORD dwVol);			//新增成功撤单时调用，nDir=0表示不明方向，1表示买入撤单，-1表示卖出撤单
};

//did模板crc数据
struct DC_DSCRC
{
	DWORD m_dwDid;//数据did号
	DWORD m_dwCrc;//did号模板crc
};

//下游连接支持的dids
struct DC_DSDID
{
	DWORD m_dwDid;//数据did号
	BYTE m_bSend:1;//为1需要重新发送模板文件
	BYTE m_bFull:1;//为1只能发生全量压缩数据，为0全量和增量压缩数据都可以发送
	BYTE m_bNoCompress:1;//为1发送无压缩数据，为0发送压缩数据
	BYTE recv:5;//
};

struct DC_DSLEN
{
	DWORD m_dwDid;//数据did号
	DWORD m_dwLen;//did号模板长度
};

//did模板数据
struct DC_DSDATA
{
	DWORD m_dwDidNum;//did个数
	DC_DSLEN mDsLen[1];//共m_dwDidNum个
};
//紧跟二进制模板数据


enum DC_RECFULL_TYPE	//压缩方法
{
	DC_RECFULL_FLAG = 0x80000000,//记录数全量标记MASK
};

//did数据,该类数据根据did相应的模板生成和解析,DC_DID数据=DC_DIDHead+（可能DC_DIDCompress）

//首先是一个变长数据类型变量，最小3个字节，最大9个字节，最高位做为特殊用途,该类型不能用sizeof函数,用GetLen函数
struct DC_DIDHead
{
	BYTE m_bDidType:2;//0表示后面的m_dwDid为13位,1为21位,2为29位,3为61位
	BYTE m_bNumType:2;//0表示后面的m_dwRecNum为1个字节BYTE,1表示为2个字节WORD,2表示为3个字节的UINT24,3表示为4个DWORD
	BYTE m_bFullRec:1;//全记录,下游可以将以前的数据全部覆盖
	BYTE rec:3;//保留
	BYTE m_bEx[1];//最多8个字节

	//获取did
	DWORD GetDid(void)
	{
		if (m_bDidType == 0)
			return m_bEx[0];
		else if(m_bDidType == 1)
			return (*(WORD*)(m_bEx));
		else if(m_bDidType == 2)
			return (*(UINT24*)(m_bEx));
		else if(m_bDidType == 3)
			return (*(DWORD*)(m_bEx));
	}
	//获取数据记录个数
	DWORD GetRecNum(void)
	{
		if (m_bNumType == 0)
			return m_bEx[m_bDidType+1];
		else if(m_bNumType == 1)
			return (*(WORD*)(m_bEx+m_bDidType+1));
		else if(m_bNumType == 2)
			return (*(UINT24*)(m_bEx+m_bDidType+1));
		else if(m_bNumType == 3)
			return (*(DWORD*)(m_bEx+m_bDidType+1));
	}

    //设置did和记录个数
	void SetRecNum(DWORD m_dwDid,DWORD dwRecNum)
	{
		if (m_dwDid <= 0xFF)
		{
			m_bEx[0] = m_dwDid;
			m_bDidType=0;
		}
		else if (m_dwDid <= 0xFFFF)
		{
			*(WORD*)(m_bEx) = m_dwDid;
			m_bDidType=1;

		}
		else if (m_dwDid <= 0xFFFFFF)
		{
			*(UINT24*)(m_bEx) = m_dwDid;
			m_bDidType=2;

		}
		else
		{
			*(DWORD*)(m_bEx) = m_dwDid;
			m_bDidType=3;
		}

		if (dwRecNum <= 0xFF)
		{
			m_bEx[m_bDidType+1] = dwRecNum;
			m_bNumType=0;
		}
		else if (dwRecNum <= 0xFFFF)
		{
			*(WORD*)(m_bEx+m_bDidType+1) = dwRecNum;
			m_bNumType=1;

		}
		else if (dwRecNum <= 0xFFFFFF)
		{
			*(UINT24*)(m_bEx+m_bDidType+1) = dwRecNum;
			m_bNumType=2;

		}
		else
		{
			*(DWORD*)(m_bEx+m_bDidType+1) = dwRecNum;
			m_bNumType=3;
		}
	}

    //获取结构体大小
	int GetLen(void)
	{
		return (m_bDidType+m_bNumType+3);
	}
};

//如果DCHEAD里的压缩类型为DC_XMLCID_CPS或者DC_SOCID_CPS,DC_DIDHead后紧跟以下结构体数据
//一个变长数据类型变量，最小2个字节，最大5个字节，第一个字节做为特殊用途,该类型不能用sizeof函数,用GetLen函数
struct DC_DIDCompress
{
	BYTE m_bLastCmp:1;//为1表示压缩的数据跟前面的切片相关，需要定义GetBmData函数
	BYTE m_bUnComLenType:2;//0表示后面的m_dwDid为1个字节BYTE,1表示为2个字节WORD,2表示为3个字节的UINT24,3表示为4个DWORD
	BYTE m_bCid:5;//压缩ID

	BYTE m_bEx[1];//最多4个字节，最小1个字节

    //获取压缩前数据长度
	DWORD GetUnComLen(void)
	{
		if (m_bUnComLenType == 0)
			return m_bEx[0];
		else if(m_bUnComLenType == 1)
			return (*(WORD*)(m_bEx));
		else if(m_bUnComLenType == 2)
			return (*(UINT24*)(m_bEx));
		else if(m_bUnComLenType == 3)
			return (*(DWORD*)(m_bEx));
	}

    //设置压缩前数据长度
	void SetUnComLen(DWORD dwUnComLen)
	{
		if (dwUnComLen <= 0xFF)
		{
			m_bEx[0] = dwUnComLen;
			m_bUnComLenType=0;
		}
		else if (dwUnComLen <= 0xFFFF)
		{
			*(WORD*)(m_bEx) = dwUnComLen;
			m_bUnComLenType=1;

		}
		else if (dwUnComLen <= 0xFFFFFF)
		{
			*(UINT24*)(m_bEx) = dwUnComLen;
			m_bUnComLenType=2;

		}
		else
		{
			*(DWORD*)(m_bEx) = dwUnComLen;
			m_bUnComLenType=3;
		}
	}

    //获取结构体大小
	int GetLen(void)
	{
		return (m_bUnComLenType+2);
	}
};


//后面紧跟二进制数据


//////////////////////////////////////////////////////////////////////////////////
//通信相关结构

//通信端口
#define DC_PORT	9966

//数据类型
//if dc_type is changed, parse.cc file will be changed too.
enum DC_TYPE
{
	DCT_NUL = 0,
	DCT_KEEPALIVE,			//<-->保持连接1
	DCT_LOGIN,				//<-->登陆以及采数机属性	2
	DCT_REQPASS,			//<-->要求用户名密码登陆3
	DCT_USERnPASS,			//<-->用户名密码4
	DCT_READY,				//-->登陆成功，可以接收数据5
	DCT_RESEND,				//-->重发数据包6
	DCT_STKSTATIC,			//<--静态数据7
	DCT_STKDYNA,			//<--动态行情8
	DCT_SHL2_MMPEx,			//<--level2扩展买卖盘9
	DCT_SHL2_REPORT,		//<--level2分笔成交10
	DCT_SHL2_BIG_WD,		//<--level2即时最大撤单11
	DCT_SHL2_ACCU_WD,		//<--level2累计最大撤单12
	DCT_HK_STATIC,			//<--港股静态13
	DCT_HK_DYNA,			//<--港股动态14
	DCT_XML,				//<--XML格式数据15
	DCT_SHL2_QUEUE,			//<--买卖盘队列，level2特有16
	DCT_GENERAL,			//<--通用数据17
	DCT_USERSTAT,			//-->用户数量统计18
	DCT_RAWDATA,			//<-->原始数据19
	DCT_NEWS,				//<--公告新闻等文本类型数据20
	DCT_SZL2_ORDER_QUEUE,	//<--委托队列，深圳Level2特有21
	DCT_SZL2_ORDER_STAT,	//<--委托队列统计，深圳Level2特有22
	DCT_DSCRC,				//<--did模板crc,23
	DCT_DSDID,				//<--需要发送模板的did 24
	DCT_DSDATA,				//<---did模板数据 25
	DCT_DID,				//DID数据,根据模板解析 26
    DCT_DIDSTATIC,          //DID数据，模板号为100003，静态码表数据，结构为DC_HEAD+DC_STKSTATIC（不含STK_STATIC）+DC_DIDHEAD+DC_DIDCompress+DATA
	DCT_INCODESTATIC,		//<--外码转换成内码后的静态数据,28

	DCT_SZL2_ORDER_FIVE=100,//<--委托队列多笔明细，深圳Level2特有
	DCT_SZL2_TRADE_FIVE=101,//<--成交,撤单队列多笔明细，深圳Level2特有
};

#define SIMPLE_ENCY_MASK	0x7C59ACDC

enum DC_ENCY_TYPE	//加密方法
{
	DC_NOENC = 0,			//明码传送
	DC_ENCY_SIMPLE,			//简单加密
	DC_ENCY_DES,			//DES加密	(##)
	DC_ENCY_AES,			//AES加密

	DC_ENCY_MASK = 7		//加密方法MASK
};
enum DC_COMPRESS_TYPE	//压缩方法
{
	DC_NOCOMPRESS = 0,		//不压缩
	DC_STD_CPS =	0x8,	//标准方法
	DC_ZLIB_CPS =	0x10,	//ZLIB压缩
	DC_ZIP_CPS =	0x18,	//ZIP格式
	DC_EXT_CPS =	0x20,	//扩展压缩，自定义
	DC_XMLCID_CPS = 0x28,	//表示该数据用xmlcid模板压缩解压
	DC_SOCID_CPS =	0x30,	//表式该数据用so库里socid函数模板压缩解压

	DC_CPS_MASK = 0x38		//压缩方法MASK
};

//ZLIB数据头，如果采用ZLIB,ZIP压缩，在DC_HEAD后面紧跟ZLIB_HEAD头，然后是压缩后的数据,压缩后的数据长度是DC_HEAD::m_nLen-sizeof(ZLIB_HEAD)
struct ZLIB_HEAD
{
	DWORD m_dwSourceLen;	//源数据长度
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//数据包头
//每个数据包均以DC_HEAD开头，根据DC_HEAD::m_cType，后面跟不同的结构

#define DC_TAG			0x6C
//define for DC_HEAD::m_wAttrib
#define DC_DUP			0x8000		//本数据包是重发数据包
//bit0--bit3: DC_ENCY_TYPE加密方法，DC_HEAD之后的数据已经使用该方法加密
//bit4--bit6: DC_COMPRESS_TYPE压缩方法，DC_HEAD之后的数据已经使用该方法压缩，如果同时加密，则须在解密后再解压缩
//bit7--bit14: 保留，必须为0
struct DC_HEAD
{
	BYTE	m_cTag;			//必须为DC_TAG
	BYTE	m_cType;		//DC_TYPE
	WORD	m_wAttrib;		//属性
	UINT24	m_nLen;			//包长度,不包含包头
	UINT24	m_nSeq;			//包序号,深圳全队列已填值,对于did数据//序列号,从初始化以后开始从0增加到0xFFFFFFFF,然后又变成0重新增加
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_KEEPALIVE,			//<-->保持连接，每30秒双方至少要向对方发送一个数据包，若没有数据则发送DC_KEEPALIVE。
struct DC_KEEPALIVE 
{
	UINT24 m_wLastRcvSeq;		//采数机行情变化次数，每一个股票行情发生变化，该数据加1，行情初始化后该数据清0
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_LOGIN <--登陆以及采数机属性
//define for DC_LOGIN::m_dwAttrib
#define DCATTR_SHLEVEL2		0x1			//服务器对于该市场需要初始化level2扩展买卖盘库，撤单库和累计撤单库
#define DCATTR_HKSEC		0x2			//服务器对于该市场需要初始化港股库
#define DCATTR_VARTIME		0x4			//该市场品种存在不同的交易时段，需要用RAWDATA传送多交易时段定义文件"MK_VT.txt"
#define DCATTR_SZLEVEL2		0x8			//深圳Level2,该标志亦需要同时拥有DCATTR_SHLEVEL2,共同表示深圳Level2，服务器对于该市场需要初始化level2逐笔委托库

#define DCATTR_INCODESTATIC	0x4000000	//发送内码DCT_STATIC
#define DCATTR_NOSTATIC		0x8000000	//取消发送外码DCT_STATIC（老static）
#define DCATTR_DIDSTATIC	0x10000000	//发送100003.xml格式的did数据

#define DCATTR_DIDDAT		0x20000000	//支持did数据协议标志
#define DCATTR_FULLDATA		0x40000000	//全部数据标志，用于测试和录制数据的通道
#define DCATTR_RAWSVR		0x80000000	//RAW数据源登陆标志

struct DC_LOGIN 
{
	WORD	m_wMarket;				//市场代码
	char	m_strMarketName[32];	//市场名称
	int		m_nTimeZoneBias;		//所处时区
	short	m_nNumOpenTime;			//开市时段数量，决定使用多少个m_nOpenTime,m_nCloseTime
	short	m_nOpenTime[8];			//开市时间，HHMM
	short	m_nCloseTime[8];		//闭市时间
	DWORD	m_dwAttrib;				//采数机属性
	WORD	m_wPriority;			//优先级，表明该行请源的好坏，0最高，0xFFFF最低
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_REQPASS -->要求用户名密码登陆
struct DC_REQPASS 	//（##）
{
	DC_ENCY_TYPE m_encyType;
	DWORD	m_dwKey[2];		//加密公钥
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_USERnPASS,			//<--用户名密码
struct DC_USERPASS 
{
	char	m_strUser[16];
	char	m_strPass[24];
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_READY,				//-->登陆成功，可以接收数据
struct DC_READY 
{
	enum DC_INSTRUCT
	{
		SUCCESS = 0,			//成功，指示采数机可以开始发送数据
		WAIT,					//成功，指示采数机等待，直到收到带有SUCCESS标志的DC_READY数据包再开始发送数据
		DISCONNECT,				//不成功，断开连接
		IDLE,					//成功，服务器指定采数机为备份行情源，指示采数机等待，直到收到带有SUCCESS标志的DC_READY数据包再开始发送数据
		QUOTEDAT,				//成功,通知client仅需发生QUOT数据给服务器
		DEALDATAOVER,//数据处理完毕
	} m_instruct;				//指令
	enum DC_ERROR
	{
		NOERR = 0,
		INVALID_USERPASS,		//非法用户名密码
		MARKET_ALREADY_EXSIST,	//市场已经存在
		NOT_SUPPORT_MARKET,		//服务器不支持该市场
		SERVER_NOTREADY,			//服务器暂时不可用，稍后再连接
	} m_errCode;				//错误码
	DWORD	m_dwDelayConnectTime;	//等待时间，如果错误类型为SERVER_NOTREADY，而且m_dwDelayConnectTime不为0，则等待这么多秒时间再次连接
	DWORD	m_dwStaticLabVersion;	//静态库版本，如果服务器的静态库版本号最新，就不需要发送静态数据
	BOOL	m_bCompress;			//要求发送数据压缩
	DC_ENCY_TYPE	m_dataEncyType;	//数据要求加密。若要求加密，则今后所有发送到服务器的数据包都尽量加密发送
	DWORD	m_dwDataKey[2];			//数据加密发送公钥,深圳全队列第一个DWORD填上次的包序号
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_RESEND -->重发数据包
struct DC_RESEND 
{
	DWORD m_dwBeginSeq;		//需要重发的数据包开始位置
	int	  m_nNum;			//重发数量,如果为0则表示之后的全部,如果<0则表示取消重发申请,采数机停止重发
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_STKSTATIC,			//<--静态数据,(静态数据要求在一个包内发送完成?)
//define for DC_STKSTATIC::m_wAttrib and DC_STKSTATIC2::m_wAttrib
#define DC_ADDSTK			0x1		//相对上一个版本，增加了股票
#define DC_LABEL_CHANGED	0x2		//相对上一个版本，代码变化了
#define DC_NAME_CHANGED		0x4		//相对上一个版本，名称变化了
#define DC_DATA_CHANGED		0x8		//相对上一个版本，数据发生了变化


struct DC_STKSTATIC 
{
	DWORD	m_dwVersion;	//静态数据版本,每次静态数据发生变化则必须增加数值,每个行情源最好有一个相对独立的高位，这样不同行情源就不会有相同的版本,0x80000000
	WORD	m_wAttrib;		//版本属性
	UINT	m_nDay;			//静态库日期，YYYYMMDD
	short	m_nNum;			//m_data数量
	STK_STATIC m_data[1];	//数据
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_STKDYNA,			//<--动态行情
struct DC_STKDYNA 
{
	WORD	m_wDynaSeq;		//动态行情序列号，用于客户端转发
	short   m_nNum;
	STK_DYNA m_data[1];
};


////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_SHL2_MMPEx,			//<--level2扩展买卖盘
struct DC_SHL2_MMPEx 
{
	short	m_nNum;
	SH_L2_MMPEX m_data[1];
};


////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_SHL2_QUEUE,			//<--level2买卖盘队列
struct DC_SHL2_QUEUE
{
	DWORD	m_dwQueue[1];	//总数量由数据头长度决定
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_SZL2_ORDER_QUEUE,			//<--level2逐笔委托队列,数据格式与买卖盘队列 DC_SHL2_QUEUE 一致，亦是由一系列DWORD组成

////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_SHL2_REPORT,		//<--level2逐笔成交
struct DC_SHL2_REPORT 
{
	short	m_nNum;			//数量
	SH_L2_REPORT m_data[1];	//总共m_nNum个
};


////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_SHL2_BIG_WD,		//<--level2即时最大撤单
struct DC_SHL2_BIG_WD 
{
	char	m_cBidOrAsk;	//'B'表示买单，'A'表示卖单
	short	m_nNum;			//数量
	SH_L2_BIG_WD m_data[1];	//总共m_nNum个
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_SHL2_ACCU_WD,		//<--level2累计最大撤单
struct DC_SHL2_ACCU_WD 
{
	char	m_cBidOrAsk;	//'B'表示买单，'A'表示卖单
	short	m_nNum;			//数量
	SH_L2_ACCU_WD m_data[1];//总共m_nNum个
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_SZL2_ORDER_STAT,		//<--level2逐笔委托统计
struct DC_SZL2_ORDER_STAT
{
	short	m_nNum;					//数量
	time_t	m_time;					//数据时间
	SZ_L2_ORDER_STAT	m_data[1];	//共有m_nNum个
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_XML					//<--XML格式数据
struct DC_XML 
{
	enum XML_PROCESS
	{
		SVR_PROCESS = 0,	//服务器处理
		SVR_SEND,			//服务器转发给每个客户
		INFO,				//资讯,包括F10，新闻等文本数据
		LIST_MENU,			//动态列表文件菜单
		LIST_TABLE,			//动态列表文件

		PUSHINFO_ROLL = 100,		//滚动资讯
		PUSHINFO_ROLL1,			//滚动资讯1,保留今后使用
		PUSHINFO_ROLL2,			//滚动资讯2,保留今后使用
		PUSHINFO_ROLL3,			//滚动资讯3,保留今后使用
		PUSHINFO_ROLL4,			//滚动资讯4,保留今后使用
		PUSHINFO_ROLL5,			//滚动资讯5,保留今后使用
		PUSHINFO_ROLL6,			//滚动资讯6,保留今后使用
		PUSHINFO_ROLL7,			//滚动资讯7,保留今后使用
		PUSHINFO_ROLL8,			//滚动资讯8,保留今后使用
		PUSHINFO_ROLL9,			//滚动资讯9,保留今后使用

		PUSHINFO_MOTION = 200,	//跑马灯2，收到跑马灯2后，会替换跑马灯1运行，跑马灯2是多条信息，并且支持超链
	};
	XML_PROCESS	m_processType;	//XML_PROCESS
	WORD	m_wMarket;			//市场代码，无市场的数据填0
	DWORD	m_dwData;			//数据标识，同一类数据下，每个数据有一个唯一的标识，接收方会覆盖上一次收到的m_processType和m_dwData都相同的数据
	DWORD	m_dwExtra;			//额外标识，由每一类数据自己定义，对于滚动资讯、跑马灯则表示YYYYMMDD格式的文件日期
	int		m_nLen;				//数据长度
	char	m_strXML[1];		//长度为m_nLen;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_GENERAL 通用数据

//define for DC_GENERAL::m_dwAttrib
#define GENERAL_FIXED		0				//定长数据
#define GENERAL_VAR			1				//变长数据
#define GENERAL_ATTR_MASK	0xFF

#define GENERAL_FLOAT		0x100			//浮点数
#define GENERAL_DWORD		0x200			//DWORD
#define GENERAL_CHAR		0x300			//字符串
#define GENERAL_TIMEFLOAT	0x400			//time_t + float数据
#define GENERAL_STRUCT		0x500			//结构数据
#define GENERAL_TYPE_MASK	0xF00

#define GENERAL_STATIC		0x1000			//静态数据，盘中一般不发生变化
			
#define GENERAL_WHOLE		0x10000000			//完整数据，本数据包包含该市场的完整数据
#define GENERAL_INTR_SEND	0x20000000			//关注股票发送，本数据发生变化，仅推送关注股票的数据
#define GENERAL_CONN_SEND	0x40000000			//客户端连接时发送给客户
#define GENERAL_DIR_SEND	0x80000000			//服务器收到后直接转发给所有客户端,否则接受用户请求时发送

#define GENERAL_FLOAT_FIX	(GENERAL_FIXED|GENERAL_FLOAT)	//float型的定长数据
#define GENERAL_FLOAT_VAR	(GENERAL_VAR|GENERAL_TIMEFLOAT)	//float型的变长时间序列值
#define GENERAL_STRING_VAR	(GENERAL_VAR|GENERAL_CHAR)		//字符串
#define GENERAL_STRING_FIX	(GENERAL_FIXED|GENERAL_CHAR)		//定长字符串
#define GENERAL_STRUCT_FIX	(GENERAL_FIXED|GENERAL_STRUCT)	//结构体

struct DC_GENERAL
{
	WORD	m_wDataID;			//数据ID
	WORD	m_wMarket;			//市场
	DWORD	m_dwDataAttr;		//数据属性
	short	m_nDataSize;		//每个数据的长度,对于变长数据,表示每个数据的最大值(0表示没有最大值，任意长度)
	UINT24	m_nTotalLen;		//包含本数据头在内的数据总长度
	WORD	m_wNum;				//数量
	DWORD	m_dwReserved;		//保留，置为0
	WORD	m_wStock[1];		//m_wNum个,如果m_wStock==0xFFFF，表示该数据不与任何股票关联
};


/*-------------------------------------------------------------------------------------------------
对于定长数据，结构如下
DC_GENERAL | m_wNum个StockID | m_wNum个数据，每个数据长度为m_nDataSize

对于变长数据，结构如下
DC_GENERAL | m_wNum个StockID | m_wNum个数据位置索引(类型为UINT24,从本结构头位置开始计算) | m_wNum个数据，每个数据长度由索引差值决定,最后一个需要用m_nTotalLen计算
--------------------------------------------------------------------------------------------------*/

#define GE_MAX_EXTRA	10000	//<GE_MAX_EXTRA用于表示附加数据,其他用于扩展
//预定义通用数据类型
#define GE_IOPV			5		//ETF,LOF动态净值估值,GENERAL_FLOAT_FIX
#define GE_MATU_YLD		6		//债券到期收益率，GENERAL_FLOAT_FIX
#define GE_HKREL_TXT	1001	//港股相关信息，GENERAL_STRING_VAR,(用于表示HKSEC_STATIC::m_cRelateText)

#define GE_STATIC_EX	(GE_MAX_EXTRA+1)	//静态扩展，GENERAL_STRUCT_FIX,(用于表示 STK_STATICEx)
#define GE_HKDYNA		(GE_MAX_EXTRA+2)	//港股动态，GENERAL_STRUCT_FIX,(用于表示 STK_HKDYNA)
#define GE_BLK_STK		(GE_MAX_EXTRA+3)	//板块指数成份股,GENERAL_VAR,表示板块指数成份股

/////////////////////////////////////////////////////////////////////////////////////
//DCT_USERSTAT

#define USERNAMELEN 50//用户名长度
#define USERPWDLEN 20//密码长度

typedef struct tagStatSingleHqsInfo
{
	char exist_flag;//为-1没有该行情源,为1有该行情源
	char hqs_ip[32];
	int m_wLastSeq;//目前收到的变化次数
	time_t fresh_time;//
}StatSingleHqsInfo,*LPStatSingleHqsInfo;

typedef struct tagStatHqsInfo
{
	char market_code[3];//市场代码
	int main_hqs_flag;//-1没有主行情源,0为第一个优先级行情源为主行情源,以此类推最大值为3
	int init_flag;//离开盘时间只有10分钟但还没有初始化为3,1为正常,2为等待初始化;
	StatSingleHqsInfo mStatSingleHqsInfo[4];
}StatHqsInfo,*LPStatHqsInfo;

typedef struct tagUserType
{
	unsigned short YYBID;
	int online_user_num;//在线用户数
}UserType,*LPUserType;

typedef struct tagUserVer
{
	short ver;
	short build;//build号

	int online_user_num;//在线用户数
}UserVer,*LPUserVer;

typedef struct tagServDataInfo
{
	unsigned char m_ucMonitorFlag; //监控状态,1为正常状态,2为异常状态,3为无法监控,4为等待数据
	unsigned char m_ucZmFlag; //转码工作状态,3为等待初始化,1为开始初始化,2为初始化完成
	unsigned char m_ucUserFlag; //用户接入工作状态,1为等待初始化,2为开始初始化,3为初始化完成

	unsigned short m_usZmProcessNum;//转码进程数
	unsigned short m_usUserProcessNum;//用户接入进程数
	unsigned short m_usOnlienUserNum;//在线用户数
	unsigned short m_usMaxUserNum;//在线历史峰值
	unsigned char m_ucNfsFlag;//资讯挂卷状态
	char m_chZhdocData[16]; //资讯最近更新时间
	unsigned short m_usZmCpu;//转码进程CPU占用率
	unsigned short m_usUserCpu;//用户接入进程cpu占用率
	unsigned short m_usMemorySize;//系统内存大小(MBytes)
	unsigned short m_usSwapSize;//SWAP大小(MBytes)
	unsigned short m_usUseSwapSize;//已用SWAP大小(MBytes)

	unsigned short m_usLicUserNum;//限制用户数
	DWORD m_ulLicDate;//服务到期日
	DWORD m_ulUserPort;//DzhUserNet使用的服务端口
	DWORD m_ulDlPort;//数据下载使用的服务端口
	char osname[32];//操作系统名称版本号
	unsigned short m_usCpuNum;//系统cpu数目
	DWORD m_ulSysBootDate;//系统启动日期(20050707)
	DWORD m_ulSysBootTime;//系统启动时间(095930)

	DWORD m_ulRunTime;//系统运行秒数
	DWORD m_ulSysDate;//系统日期(20050707)
	DWORD m_ulSysTime;//系统时间(095930)

	DWORD m_ulZmStartDate;//zm启动日期(20050707)
	DWORD m_ulZmStartTime;//zm启动时间(095930)
	time_t m_nZmRunTime;//zm运行时间

	DWORD m_ulUserStartDate;//user启动日期(20050707)
	DWORD m_ulUserStartTime;//user启动时间(095930)
	time_t m_nUserRunTime;//user运行时间

	int valid_user_type_num;
	UserType mUserType[10];

	int valid_user_ver_num;
	UserVer mUserVer[20];

	int valid_market_num;//有效市场数目
	StatHqsInfo mStatHqsInfo[10];//最大10个市场
}ServDataInfo,*LPServDataInfo;

struct DC_USERSTAT
{
	in_addr	m_svrAddr;		//服务器IP
	WORD	m_wSvrPort;		//服务器端口
	ServDataInfo  m_data;
};

/////////////////////////////////////////////////////////////////////////////////////
//DZH_AUTHORIZE  授权结构
//用于描述任何需要根据用户情况授权使用的情况
//分成两个部分，一是分类授权，表示用户属于哪一类，比如产品、券商、营业部等信息；二是产品功能、市场授权，按bit组合

enum DZH_GROUP_TYPE
{
	DG_NIL = 0,					//无效
	DG_PRODUCT,					//产品
	DG_SC_CPY,					//券商
	DG_SC_DEP,					//营业部，必须在DG_SC_CPY券商分类之后才有意义
};

#define DZH_AGTYPE_MASK		0xE000
#define DZH_AGGROUP_MASK	0x1FFF

struct DZH_AUT_GROUP
{
	WORD	m_wData;
	
	void SetData(DZH_GROUP_TYPE type,DWORD dwGroup){m_wData = (WORD)(((type<<13)&DZH_AGTYPE_MASK) | (dwGroup&DZH_AGGROUP_MASK));}

	DZH_GROUP_TYPE GetType(){return (DZH_GROUP_TYPE)((m_wData&DZH_AGTYPE_MASK)>>13);}
	void SetType(DZH_GROUP_TYPE t){ m_wData &= ~DZH_AGTYPE_MASK; m_wData |= (t<<13)&DZH_AGTYPE_MASK;}

	DWORD GetGroup(){return m_wData&DZH_AGGROUP_MASK;}
	void SetGroup(DWORD dwGroup){ m_wData &= ~DZH_AGGROUP_MASK; m_wData |= (dwGroup&DZH_AGGROUP_MASK);}
};

#define DZH_AUT_GROUP_SIZE	8
struct DZH_AUTHORIZE
{
	DZH_AUT_GROUP	m_groupAuth[DZH_AUT_GROUP_SIZE];		//组分类，共有8组可以使用，可以设置0-8个组授权，有效组必须放在前面，无效组m_wData==0
	UINT64			m_nMarketAuth;							//市场授权，每一个bit表示一个市场
	UINT64			m_nFuncAuth;							//功能授权，每一个bit表示一个功能

	void Init();
	BOOL IsGroupChecked(DZH_GROUP_TYPE type,DWORD dwGroup);
	BOOL IsBitChecked(BOOL bMarket,int nBit);
	BOOL HasAuth();
	int GetNumGroup() const;
	BOOL IsChecked(const DZH_AUTHORIZE* pAuth);				//pAuth表示用户权限，而不是资讯权限
};

/////////////////////////////////////////////////////////////////////////////////////
//DCT_RAWDATA 原始数据
//原始数据是采数端获得的数据，直接提供给客户端下载使用的，服务端只需要根据标志完成存储、清除等工作即可
//该数据包本身支持压缩，故数据包头DC_HEAD不应该包含任何压缩标志
//原始数据也支持RAW数据源向采数端传送功能，用于对其下属的服务端进行广播(带有RAW_BROADCAST标志)

#define RAW_INITDEL		0x1				//市场初始化的时候，清除该数据
#define RAW_SVRUSE		0x2				//服务端处理的文件，不提供给客户端使用
#define RAW_BROADCAST	0x4				//广播文件，采数端收到DCT_RAWDATA文件，如果包含该标志则将它转发给所有的客户，否则就用该文件名在服务端指定目录保存下来
#define RAW_SVRSAVE_BC	0x8				//服务端保存该文件，用户登陆的时候，需要给他传输
#define RAW_CLIENT_BC	0x10			//客户端广播，服务器端收到该数据后，要根据用户属性(如果有RAW_AUTHORIZE)，给每一个符合条件的客户端推送该RAW数据

#define RAW_AUTHORIZE	0x4000			//m_cData开头包含一个DZH_AUTHORIZE结构，之后才是正式数据，m_dwOrgLen和m_dwDataLen也包含该长度，服务端要根据权限决定如何转发
										//服务端转发数据给客户端，必须将DZH_AUTHORIZE结构删除，此时需要处理m_dwOrgLen和m_dwDataLen并且删除RAW_AUTHORIZE标志
										//若同时包含RAW_ZLIBCPS标志，DZH_AUTHORIZE结构是不压缩的，此时服务端若需要解压缩数据，需要考虑m_dwOrgLen和m_dwDataLen长度都需要减去DZH_AUTHORIZE结构长度
#define RAW_ZLIBCPS		0x8000			//zlib压缩数据，m_cData中保存使用zlib压缩的数据
#define RAW_AES			0x10000			//AES加密
#define RAW_NOSAVE		0x20000			//不落盘

struct DC_RAWDATA
{
	WORD	m_wMarket;				//市场，对于无市场的数据，则为0，需要推送的无市场数据则为'WN',即0x574E
	char	m_strFileName[14];		//市场内原始数据的唯一标识，服务端也以该文件名保存和处理数据，文件名符合8.3规则
	DWORD	m_dwAttrib;				//属性
	DWORD	m_dwVersion;			//数据版本
	DWORD	m_dwOrgLen;				//m_cData原始长度
	DWORD	m_dwDataLen;			//实际长度，如果没有压缩，则与m_dwOrgLen一样
	BYTE	m_cData[1];				//总长度为m_dwDataLen,原始数据，接收端以文件名m_strFileName保存文件
};

//预定义的文件名
#define FIX_BJ		"ZQ_QDBJ.txt"		//固定收益平台报价
#define FIX_HQ		"ZQ_CJHQ.txt"		//固定收益平台行情
#define FIX_MX		"ZQ_CJMX.txt"		//固定收益平台成交明细
#define FIX_XX		"ZQ_ZQXX.txt"		//固定收益平台证券信息

//混合交易时段市场数据
//对于类似全球指数的市场，其每个品种的开收盘时间不一致，需要对这种市场提供每个个股的交易时间
//混合交易时段市场，需要在DC_LOGIN::m_dwAttrib中包含DCATTR_VARTIME标志，并且市场交易时段设置为最早开市品种--最晚收市品种，就一个时段，各品种的交易时段用下列数据描述
/*格式为:每一行描述交易时段或者一个品种，方括号内描述交易时段，其后的每一行描述在该时段下的一个品种的代码(不包含市场标志)，交易时间格式为十进制HHMM，
  如果延续到第二天也可以，但是总的时间必须<24小时
例如：
[0930-1130 1300-1500]
600000
600001
[1000-1200 1300-1600 2100-0300]
600123
600999
*/
#define MK_VARTIME	"MK_VT.txt"			//多交易时段定义文件

#define L2_STAT_FILE	"L2STAT.ZIP"	//Level2统计文件，用于给深圳L1行情源提供Level2统计，应该具有属性 RAW_INITDEL|RAW_SVRUSE  (注意，虽然扩展名是ZIP，但是并不是ZIP压缩文件)
#define L2_STAT_CPS		"L2STAT.CPS"	//Level2统计文件，用于给L2行情源提供Level2统计，应该具有属性 RAW_INITDEL|RAW_SVRUSE,该文件是由Level2统计数据源生成后传送给采数，然后再广播下去

#define RAW_POOL_FILE	"PLSVC.XML"		//系统股票池服务文件，000PLSVC.XML--999PLSVC.XML,一共可以定义1000个文件
#define RAW_PLANA_FILE	"PLANA.XML"		//系统股票池历史分析数据文件，000PLANA.XML--999PLANA.XML,一共可以定义1000个文件

#define C_SOH	0x1		// <SOH>=0x1,数据分隔符，之后是数据
#define C_LBL	0x2		// <LBL>=0x2,股票代码，后跟股票代码
#define C_ID	0x3		// <ID>=0x3, 股票ID或信息ID，后跟股票ID
#define C_ATR	0x4		// <ATR>=0x4, 属性字符串，后跟属性集，可以有：'F'完整包，表示本数据包是该市场的全部数据，客户端处理时文件中不存在的股票将被删除该数据
#define C_TIM	0x5		// <TIM>=0x5,时间字符串，后跟数据时间，格式为"HH:MM:SS"或者 "HH:MM"
#define C_DAT	0x6		// <DAT>=0x6,日期字符串，后跟数据日期，格式为"YYYYMMDD"
#define C_ENCODE 0xF	// <ENCODE>=0xF,字符编码，必须是第一个记录，默认为GB2312，非GB2312得需要指出，比如UTF-8,UTF-16等

//个股一般数据，用于定义个股静态数据/变化慢的数据
//一般数据采用文本方式存放，每个股票可以有一个数据
//文件名采用 "GNLxxxxx.GNL",xxxxx表示数据类型，从00000--99999总共可以定义1万种,一般采用FIX协议中的标准定义
//文件内容用"分隔符+数据"的方式组成 分隔符是小于0x10的二进制数，数据是ASCII码
//数据可以分为两种方式，一种是 代码+数据，另一种是 ID+数据，后者如果是连续ID还可以省略ID
//举例：
//方式1，代码方式
//<LBL>000001<SOH>100.01<LBL>000002<SOH>23.34<LBL>000012<SOH>11.23<LBL>000013<SOH>12.00
//
//方式2：ID方式
//<ID>0<SOH>100.01<ID>1<SOH>23.34<ID>11<SOH>11.23<ID>12<SOH>12.00
//存在连续ID，所以可以省略为
//<SOH>100.01<SOH>23.34<ID>11<SOH>11.23<SOH>12.00
//客户端通过不同数据类型，访问GNLxxxxxx.GNL文件获得数据
//该种数据的数据版本号采用原始数据CRC，并且请使用zlib压缩（包含RAW_ZLIBCPS）
//目前该种数据，每种数据类型只能存在一个文件，即所有个股都必须打包到一个文件中传输，下一步服务端支持增量方式后再加入增量方式传输

//预定义个股一般数据数据类型：
#define GNL_SZSTK_STATAS	8901	//证券简称前缀,深圳市场专用，4字节字符串
#define GNL_STKNAME_ENG		107		//证券英文简称,字符串
#define GNL_ISIN			48		//证券ISIN编码,字符串
#define GNL_ROUND_LOT		561		//交易单位，用于成交量单位与交易单位不同的股票，比如港股市场

//实时观察数据
//实时观察数据采用类FIX协议,使用<TIM>,<LBL>和<SOH>分割数据,分别表示时间，标题，内容，其中标题是可选的，如果没有标题则使用内容的前70字节作为标题
//内容可以采用大智慧RichText描述符，使用<* *>描述颜色，字体，字符大小等属性，并使用<*a href=URL*>xxx<*/a*>描述超链，xxx会显示为超链
//实时观察可以带有资讯ID,用<ID>描述，发送重复ID，会覆盖前一个相同ID的资讯,如果没有内容只有ID，则表示删除。ID必须是一个介于1和20亿之间的数字,
//例如 <TIM>9:30<SOH>增发，<*a href=DZH://VIEWSTOCK?LABEL=SH600000*>600000浦发银行<*/a*>增发,详情请参阅<*a href=http://info.gw.com.cn/viewinfo.asp?id=123413*>详细报道<*/a*>
//该RAW文件可以包含权限描述，只有拥有相应权限的客户端才会收到该实时观察数据
//RAW文件必须带有 RAW_SVRUSE | RAW_CLIENT_BC 标志
//文件名为 "xxxxNTFY.NTF",其中xxxx为0000--9999，每一类型实时观察用一个文件名，默认为0000NTFY.NTF
#define RAW_NOTIFY_FILE		"NTFY.NTF"		

//弹出广告
//弹出广告可以是Flash、html或者jpeg文件,必须压缩为ZIP格式发送
//弹出广告如果是单个文件，则被压缩文件名无要求，如果是多个文件，则必须是根目录下的"default.htm"、"default.swf"、"default.jpg"
//控制文件，名称为default.cfg,控制弹出广告的显示时间和尺寸，每一行表示一个控制命令，格式如下
//  size=宽度*高度，弹出广告的尺寸
//  popup=时间 [random] [repeat(N)]， 弹出广告的显示时间，可以有多条
//  link=URL, 对于jpg格式的广告，可以支持超链，点击图形链接到超链
//  例如： 
//  size=800*600					尺寸为800*600
//  popup=14:40						弹出时间为14:40,弹出一次
//  popup=11:00 random repeat(10)	弹出时间为11:00后10分钟之内的随机时间，重复10天
//广告文件默认大小为400*300；如果不是这个尺寸，jpge可以自动调整尺寸，另外两种文件则需要在控制文件中加入尺寸描述
//该RAW文件可以包含权限描述，只有拥有相应权限的客户端才会收到该数据
//该文件一般带有RAW_CLIENT_BC，推送到客户端
//文件名为 "xxxxADVT.ZIP",其中xxxx为0000--9999，每一类型弹出广告用一个文件名，默认为0000ADVT.ZIP
#define RAW_ADVERTISE		"ADVT.ZIP"

//控制指令
//控制指令采用xml格式，可以命令客户端执行一定的操作
//该RAW文件可以包含权限描述，只有拥有相应权限的客户端才会收到该数据
//该文件一般带有RAW_CLIENT_BC，推送到客户端
//文件名为 "xxxxCOMD.XML",其中xxxx为0000--9999，每一类型命令用一个文件名，默认为0000COMD.XML
#define RAW_COMMAND_FILE	"COMD.XML"		

//DFIX数据
//DFIX格式自带类型、对象解释，服务端根据解析出来的内容存放到相应的服务中
//若带有RAW_CLIENT_BC，则同时推送到客户端
//文件名为 "xxxxDFIX.DAT",其中xxxx为0000--9999，可以用于区分不同类型、权限的DFIX数据，但本身xxxx内容与实际数据无直接关系，默认为0000DFIX.DAT
//0000DFIX.DAT专用于传送数据模板，每次连接后，首先传送模板，然后才能传数据
#define RAW_DFIX_DATA		"DFIX.DAT"

/////////////////////////////////////////////////////////////////////////////////////
//DCT_NEWS	公告新闻数据
//新闻以标题作为标识，标题版含目录名和标题名，以反斜杠分隔，可以有多级目录，最后一个反斜杠之后是标题
//公告新闻分为普通新闻和个股新闻，普通新闻以"INFO"最为根目录，个股新闻以"STOCK\"+市场代码+个股代码作为根目录，例如"STOCK\SH\600000"
//不同市场来源的新闻，最终在服务端将集中到一起检索存放，所以采数端不能随意创造目录，需要协商按照一定规范进行
//市场新闻，可以以"INFO\市场名称"作为根目录，此时因为不与其他市场公用目录，可以随意增减目录

struct DC_NEWS
{
	DWORD	m_dwSerialNo;			//序号，采数端从0开始递增，级联传送时候可以根据序号得到那一个序号之后的文件需要重新传送
	WORD	m_wAttrib;
	WORD	m_wTitleLen;			//标题长度
	DWORD	m_dwTextLen;			//正文长度
	char	m_cText[1];				//标题及正文，长度为m_wTitleLen
};
//接收端如果收到过新闻，则在发送了DC_READY之后，再发送一个空的DC_NEWS包给发送端，中间只包含m_dwSerialNo，表明上一次收到的新闻序号
//接收端初始化时，需要将收到的新闻全部删除，同时将已经接收到数据的序列号清0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//通信过程(采数机连接服务器模式)
//			服务器端														采数机端
//1，打开DC_PORT端口，等待连接
//
//															2，连接服务器DC_PORT，连接后发送DC_LOGIN数据
//
//3，收到DC_LOGIN，记录市场属性，稍后初始化时可能
//需要初始化特殊的库。如果需要验证密码，发送DC_REQPASS包，否则进入5
//
//															4，根据服务器要求，将用户名密码加密后发送DCT_USERnPASS包
//
//////////(服务器连接采数机模式)////////////////////////////////////////////////////////
//			服务器端														采数机端
//															1，打开DC_PORT端口，等待连接(端口号可配置)
//
//2，连接采数机DC_PORT(可配置)，连接后发送DC_LOGIN数据(仅用其中的m_wMarket信息)，
//
//															3，如果支持该市场，发送DC_REQPASS，否则发送带有DISCONNECT指令的DC_READY包，错误代码为NOT_SUPPORT_MARKET
//
//4，若收到DC_REQPASS,则根据服务器要求，将用户名密码加密后发送DCT_USERnPASS包，
//   若干收到带有DISCONNECT指令的DC_READY，则断开连接
//
//															4.1，发送DC_LOGIN数据
//
//4.2，收到DC_LOGIN，记录市场属性，稍后初始化时可能
//     需要初始化特殊的库。
//
//															
//
////////////////////////////////////////////////////////////////////////////////////////
//5，发送DC_READY包，
//		a,密码不对，发送DISCONNECT指令，错误码INVALID_USERPASS；断开连接
//		b,已经存在该市场的采数机，设置错误码MARKET_ALREADY_EXSIST
//			1,若需要让本采数机备用，则发送IDLE指令;之后每30秒发送带IDLE指令的
//				DC_READY包；若另一个数据源故障，需要启用本采数机，
//				则发送带SUCCESS指令的DC_READY包，进入接收数据
//			2,否则发送DISCONNECT指令，断开连接
//		c,服务器正在进行初始化，发送WAIT指令，直到初始化完成后，发送SUCCESS指令
//		d,若不支持该市场，则发送DISCONNECT指令，错误码NOT_SUPPORT_MARKET；断开连接
//		e,若服务器暂时不可用，发送DISCONNECT指令，错误码SERVER_NOTREADY,设定采数机重新连接时间；断开连接
//		f,否则，发送SUCCESS指令，准备接收数据
//															6, 收到DCT_READY包，指令为：
//																a,DISCONNECT，则断开连接
//																b,DISCONNECT,若错误码为SERVER_NOTREADY,且m_dwDelayConnectTime不为0，则首先断开，间隔这么多秒后再次连接
//																c,WAIT，则继续接收服务器发送的数据包，等待进一步指令；保持采数，但不发送
//																d,IDLE，则继续接收服务器发送的数据包，等待进一步指令；保持采数，发送DC_KEEPALIVE通知服务器采数机状态
//																e,SUCCESS，登陆成功，则保存是否需要压缩、加密等信息，进入6.1
//															
//															6.1,如果收到的DCT_LOGIN数据包里m_dwAttrib属性有DCATTR_DIDDAT，发送DC_DSCRC包,将本连接支持的did和其模板的crc文件下发
//																否则直接进入7.				
//
//6.2 收到DC_DSCRC包
//选取本机支持的DID,比较crc,将需要重新发送模板的did
//通过DC_DSDID发送
//															6.3,收到DC_DSDID,记录该连接支持的did,同时通过DC_DSDATA发送需要更新的did模板
//
//															7，与本地静态库对照版本，若不同则发送静态库DC_STKSTATIC 和 使用DC_GENERAL发送 STK_STATICEx
//																
//
//															8，发送完整动态库。进入采数，当读入新的静态库时，应及时发送新的完整静态库
//9，每次收到新的静态库，需要重新初始化本地数据，针对每个市
//场记录股票m_wID，填充初始化本地行情记录，如果日期不一致，
//还需要清除过期的分笔、分时库
//   在初始化期间或其他任何服务器认为不适宜接受数据期间，		采数机收到带有WAIT指令的DC_READY包，暂停向服务器发送数据，直到收到SUCCESS指令
//   向采数机发送带有WAIT指令的DC_READY包，
//		
//													......
//															10, 每当采集到新的行情变化，发送DCT_STKDYNA数据包，数据包大小以最适合传送同时也不影响延时为好
//11，收到DCT_STKDYNA动态数据，
//		a,更新本地行情记录
//		b,对关注股票或列表关注股票推送动态行情
//		c,压缩变化了的股票动态行情，将整个包推送到客户组中，由分组程序发送到一个组成员
//		d,更新分笔成交库和分时库
//													......
//
//															12, level2采集动态行情包变化，发送DCT_STKDYNA和DCT_SHL2_MMPEx数据包
//13，收到DCT_SHL2_MMPEx数据包
//		a,更新扩展买卖盘快照,等待用户请求
//		b,对关注股票推送
//
//													......
//														 	13, level2采集到分笔成交变化，发送DCT_SHL2_REPORT数据包
//14，收到DCT_SHL2_REPORT
//		添加Level2单笔成交库(不是分笔成交库)，
//		对关注股票推送新收到的数据
//
//													......
//															15,level2采集到大单撤单回报，发送 DC_SHL2_BIG_WD数据包
//16，收到DC_SHL2_BIG_WD，
//		a,更新本地大单撤单库
//		b,对关注股票推送
//		c,对正在查看大单回报的客户，全包推送
//		d,分股票更新大单撤单库保存历史
//
//													......
//															17,level2采集到累计大单变化，发送DC_SHL2_ACCU_WD数据包
//18，收到DC_SHL2_ACCU_WD，
//		a,更新本地累计大单库
//		b,对正在看大单回报的客户，全包推送
//
//													......
//															19,港股采集到港股静态数据包，更新本地，发送完整DC_STKSTATIC,使用DC_GENERAL发送 STK_STATICEx
//20，收到DC_GENERAL港股相关静态数据
//		a,更新本地港股静态库
//		b,对所有客户推送
//		c,今后登录的客户需要完整推送
//
//													......
//															21,港股采集到港股动态数据包，发送DC_STKDYNA和DC_GENERAL(发送STK_HKDYNA)
//22，收到DC_GENERAL(STK_HKDYNA)港股动态
//		a,更新本地港股动态库
//		b,对关注股票推送
//
//													......
//															23,其他数据量不大、针对面比较小或其他合适的数据，可以采用DC_XML发送
//24，收到DC_XML数据包，根据要求可能需要
//		a,解释数据包，自己处理并分发
//		b,直接分发数据包给客户
//
//													......
//															24.1,采集配置的did数据，根据记录的连接支持的did发送DC_DID数据
//24.2，收到DC_DID数据包，根据要求可能需要
//		a,解释数据包，自己处理并分发
//		b,直接分发数据包给客户
//
//													......
//
//25，服务器发生故障，重新连接后可以申请采数机重发数据包
//		发送DC_RESEND数据包
//															26，将记录的数据包重新发送，注意，服务器可能在重发期间再次发送重发请求，因累加请求区间
//																如果服务器取消重发申请，则停止所有的重发冲程。（##）
//
//													......
//27,每30秒发送DC_KEEPALIVE通知采数机数据包接收情况
//															28，如果发现数据包接收有大量延迟，则发送DC_DELAY给服务端
//29，收到DC_DELAY，根据情况，可以选择接收其他数据源或者其他处理
//
//														注意															
//																1，采数机顺序存盘当天发出的每一个数据包，并且记录索引
//
//30，每15秒，服务器向采数机发送DC_USERSTAT客户统计数据
//
//															31，采数机收到DCT_USERSTAT，使用ip-port.sta文件名将该数据落盘，内容是ServDataInfo，并且向上一级采数机转发该数据
//
//
//
//
//
//
//
//////////(RAW数据源连接采数机模式)////////////////////////////////////////////////////////
//			RAW数据源														采数机端
//															1，打开DC_PORT端口，等待连接(端口号可配置)
//
//2，连接采数机DC_PORT(可配置)，连接后发送DC_LOGIN数据(仅用其中的m_wMarket标志，m_strMarketName设置为"RawDataServer", m_dwAttrib带有DCATTR_RAWSVR标志)，
//
//															3，如果支持该市场，发送DC_REQPASS，否则发送带有DISCONNECT指令的DC_READY包，错误代码为NOT_SUPPORT_MARKET
//
//4，若收到DC_REQPASS,则根据服务器要求，将用户名密码加密后发送DCT_USERnPASS包，
//   若干收到带有DISCONNECT指令的DC_READY，则断开连接
//
//															5,发送DC_READY包，
//																	a,密码不对，发送DISCONNECT指令，错误码INVALID_USERPASS；断开连接
//																	b,否则，发送SUCCESS指令，准备接收数据
//6，收到DC_READY包，只要有新的RAW数据就发送DC_RAWDATA数据
//
//															7，收到DC_RAWDATA,如果存在RAW_BROADCAST标志，将该数据包下发给所有下属的服务器/中转（发送时要去掉RAW_BROADCAST标志）
//															8,每30秒发送DC_KEEPALIVE通知RAW数据源
//															
//
//
//
////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//（##）功能暂不支持
#pragma pack(pop)

#endif //_DATACOLLECT_H_INCLUDE
