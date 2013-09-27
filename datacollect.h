/*--------------------------------------------------------------------------
Copyright(C) Shanghai Great Wisdom Co. Ltd (DZH)
���ࣺ���ݲɼ�ѹ��
˵�����������ݶ���
--------------------------------------------------------------------------*/
#ifndef _DATACOLLECT_H_INCLUDE
#define _DATACOLLECT_H_INCLUDE

/*
�޸���ʷ��
 08/4/29  No.5.d No.6.b No.8  No.11.c DC_LOGIN����m_wPriority DC_READY����SERVER_NOTREADY,m_dwDelayConnectTime
 08/6/20  STK_DYNA���� m_mInnerVol����������,�ɷ������������ݲɼ�����Ϊ0����
 08/7/31   DC_READY����IDLEָ��޸ĵ�½����No.5,No.6���޸�DC_KEEPALIVE��������
 08/7/31  ���� DC_GENERAL�������ͺ�STK_HKDYNA
 08/8/10  DC_XML ����m_dwData����ID
 08/8/14  �޸ĵ�½���̣�֧�ַ����������������
 08/8/14  ����DC_USERSTAT�ͻ�ͳ������
 08/01/24 ����DC_RAWDATAԭʼ���ݣ�DC_NEWS����֧��
 08/08/08 ���ӻ��ʱ���г�֧�֣���DC_RAWDATA���ͣ��μ� MK_VARTIME
 08/11/25
	1.��������Level2���ί���������ͣ� DCT_SZL2_ORDER_QUEUE,���������������̶���һ��ʹ�� DC_SHL2_QUEUE ���ݽṹ��ѹ������
	2.DCT_LOGIN ���� DCATTR_SZLEVEL2 ��־����ʾ���г�����level2���ί�����ݣ���������Ҫ׼����������
 09.2.17
	����RAW����Դ������˴������ݹ��ܣ����ڶ��������ķ���˽��й㲥(����RAW_BROADCAST��־)��DC_RAWDATA�������� RAW_BROADCAST �㲥��־
 09.4.10  RAW��������Ȩ�޶����־ RAW_AUTHORIZE
		  ����DZH_AUTHORIZE�ṹ����������Ȩ�ṹ
 09.7.23  ��������Level2���ί��ͳ����������,DCT_SZL2_ORDER_STAT
 09.7.25  RAW��ʽ�������Ӹ���һ�����ݣ�GNLxxxxx.GNL
 12.10.19 
		1.����DC_DSCRC
		2.����DC_DSDID
		3.����DC_DSDATA
		4.����DC_DID����,��������ѹ��DC_XMLCID_CPS��DC_SOCID_CPS
 13.01.17
		1,STK_STATIC�ṹ��STK_TYPE�����ӻ���ľ������
 13.04,01
		1,����DCT_DIDSTATIC�����ӱ��
		2,����DCT_INCODESTATIC
		3,����DCATTR_INCODESTATIC,DCATTR_NOSTATIC,DCATTR_DIDSTATIC���
		����STATIC���ݲ���ʱ��
			����˳��DCT_STKSTATIC--DCT_DIDSTATIC--DCT_INCODESTATIC
			STKID�ڶ���STATIC����Ҫ���
			��ʼ������Ϊ����STATIC���������ڶ���ȶ��Ҳ����ڱ��ر������������
			����ĳֻSTKID��������ǰ��Ҫ��֤��ֻSTKID��Ӧ����STATIC�����ѷ���

*/

#pragma pack(push,1)

#include "mword.h"	//���� MWORD
//////////////////////////////////////////////////////////////////////////////////
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
//////////////////////////////////////////////////////////////////////////////////
//ͨ�ýṹ

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

//��̬���ݣ����
struct STK_STATIC
{
	enum STK_TYPE
	{
		INDEX = 0,				//ָ��
		STOCK = 1,				//��Ʊ
		FUND = 2,				//����
		BOND = 3,				//ծȯ
		OTHER_STOCK = 4,		//������Ʊ
		OPTION = 5,				//ѡ��Ȩ
		EXCHANGE = 6,			//���
		FUTURE = 7,				//�ڻ�
		FTR_IDX = 8,			//��ָ
		RGZ = 9,				//�Ϲ�֤
		ETF = 10,				//ETF
		LOF = 11,				//LOF
		COV_BOND = 12,			//��תծ
		TRUST = 13,				//����
		WARRANT = 14,			//Ȩ֤
		REPO = 15,				//�ع�
		COMM = 16,				//��Ʒ�ֻ�
		STOCKFUND = 17,			//��Ʊ����
		BONDFUND = 18,			//ծȯ����
		HYBRIDFUND = 19,		//����ͻ���
		CPTPSVFUND = 20,		//�����ͻ��� Capital preservation
		MONEYFUND = 21,			//����һ�����
		NVMONEYFUND = 22,		//���Ҿ�ֵ����,net value
		SMFUND = 23,			//������ƻ���
		QDIIFUND = 24,			//QDII����
	};
	WORD	m_wStkID;			//���г���Ψһ��ʾ,�ڱ��г��ڵ����
	char	m_strLabel[10];		//����

	char	m_strName[32];		//����
	BYTE	m_cType;			//STK_TYPE
	BYTE	m_nPriceDigit;		//�۸���С�ֱ��ʣ��ǳ���Ҫ��ÿһ��DWORD���͵ļ۸�Ҫ����10^m_nPriceDigit���������ļ۸�
	short	m_nVolUnit;			//�ɽ�����λ��ÿһ�ɽ�����λ��ʾ���ٹ�
	MWORD	m_mFloatIssued;		//��ͨ�ɱ�
	MWORD	m_mTotalIssued;		//�ܹɱ�
	
	DWORD	m_dwLastClose;		//����
	DWORD	m_dwAdvStop;		//��ͣ
	DWORD	m_dwDecStop;		//��ͣ

	int		GetPriceMul() const;		//�۸��������m_nPriceDigit����
};

//��չ��̬����,��Ϊ��̬���ݵĲ��䣬����ÿһ����Ʊ������ģ�����ѡ�������ɲ��ֹ�Ʊ�����ݣ��ڷ���STK_STATIC����,ʹ��DCT_GENERAL����
struct STK_STATICEx
{
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
	char	m_cType;			//STK_TYPE,�ɸñ�־����������ʹ����һ���ṹ
	char	m_cSubType;			//STK_SUBTYPE
	union
	{
		struct		//��Ʊ	(STOCK,OTHER_STOCK)
		{
			float	m_fFaceValue;		//	��ֵ
			float	m_fProfit;			//	ÿ������
			WORD	m_wIndustry;		//	��ҵ'A' -- 'M',�μ� industryClasify
			char	m_cTradeStatus;		//	����״̬��'N'=Normal, 'H'=Halt, 'X'=Not trade on this Market
			float	m_fCashDividend;	//	ÿ�ɺ���
			char	m_cSecurityProperties;//��������ǣ�ȡֵ����STK_SP�е�ֵ
			DWORD	m_dwLastTradeDate;//������գ�ĿǰΪ�գ���Ԥ����

		} m_equitySpec;
		struct		//����,ETF,LOF	(FUND,ETF,LOF)
		{
			float	m_fFaceValue;		//	��ֵ
			float	m_fTotalIssued;		//	�ܹɱ�
			float	m_fIOPV;			//	IOPV��ֵ,����ETF,LOF��Ч��***
		} m_fundSpec;
		struct		//Ȩ֤,ѡ��Ȩ	(OPTION,WARRANT)
		{
			char	m_cStyle;			//  'A' or 'E'	American or European Style
			char	m_cCP;				//	'C' or 'P' Call or Put
			float	m_fCnvtRatio;		//  �һ�/��Ȩ����
			float	m_fStrikePrice;		//	��Ȩ�۸�
			DWORD	m_dwMaturityDate;	//	������,YYYYMMDD
			char	m_strUnderLine[12];	//	��Ӧ��Ʊ,�����г����룬��SH600000
			float	m_fBalance;			//  ��ͨ���
		} m_warrantSpec;
		struct		//ծȯ	(BOND)
		{
			DWORD	m_dwMaturityDate;	//	������,YYYYMMDD
			DWORD	m_dwIntAccruDate;	//	��Ϣ�գ�YYYYMMDD
			float	m_fIssuePrice;		//	���м�
			float	m_fCouponRate;		//	����
			float	m_fFaceValue;		//	��ֵ
			float	m_fAccruedInt;		//	Ӧ����Ϣ,***
		} m_bondSpec;
		struct		//��תծ	(COV_BOND)
		{
			char	m_cStyle;			//  'A' or 'E'	American or European Style
			char	m_cCP;				//	'C' or 'P' Call or Put
			float	m_fCnvtRatio;		//  �һ�/��Ȩ����
			float	m_fStrikePrice;		//	��Ȩ�۸�
			DWORD	m_dwMaturityDate;	//	������,YYYYMMDD
			char	m_strUnderLine[12];	//	��Ӧ��Ʊ,�����г����룬��SH600000
			float	m_fAccruedInt;		//	Ӧ����Ϣ
		} m_CnvtSpec;
		struct		//�ڻ�,��Ʒ	(FUTURE,FTR_IDX,COMM)
		{
			DWORD	m_dwLastDayOI;			//���ճֲ���
			float	m_fLastSettlePrice;		//���ս����
		} m_futureSpec;
		struct		//����	(TRUST)
		{
			float	m_dwfAsset;			//���ʲ�
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
	{'A',"ũ��ҵ"},
	{'B',"�ɾ�ҵ"},
	{0x3043,"ʳƷ����"},	//C0
	{0x3143,"��֯��װ"},	//C1
	{0x3243,"ľ�ļҾ�"},	//C2
	{0x3343,"��ֽӡˢ"},	//C3
	{0x3443,"ʯ�ͻ���"},	//C4
	{0x3543,"����"},		//C5
	{0x3643,"��������"},	//C6
	{0x3743,"��е�Ǳ�"},	//C7
	{0x3843,"����ҽҩ"},	//C8
	{0x3943,"��������"},	//C9
	{'D',"��ˮ����"},
	{'E',"����ҵ"},
	{'F',"��ͨ�ִ�"},
	{'G',"��Ϣ����"},
	{'H',"��ҵó��"},
	{'I',"���ڱ���"},
	{'J',"���ز�"},
	{'K',"������"},
	{'L',"�Ļ�����"},
	{'M',"�ۺ���"}
};

//��̬����
struct STK_DYNA
{
	WORD	m_wStkID;			//��ƱID
	time_t	m_time;				//�ɽ�ʱ��
	DWORD	m_dwOpen;			//����
	DWORD	m_dwHigh;			//���
	DWORD	m_dwLow;			//���
	DWORD	m_dwNew;			//����
	MWORD	m_mVolume;			//�ɽ���
	MWORD	m_mAmount;			//�ɽ���
	MWORD	m_mInnerVol;		//���̳ɽ���,<0��ʾ�ñʳɽ�Ϊ��������>=0��ʾ������,����ֵ��ʾ���̳ɽ���
	DWORD	m_dwTickCount;			//�ۼƳɽ�����
	DWORD	m_dwBuyPrice[5];		//ί��۸�
	DWORD	m_dwBuyVol[5];			//ί����
	DWORD	m_dwSellPrice[5];		//ί���۸�
	DWORD	m_dwSellVol[5];			//ί����
	DWORD	m_dwOpenInterest;		//�ֲ���(�ڻ���ָ����)
	DWORD	m_dwSettlePrice;		//�����(�ڻ���ָ�ֻ�����)
};

//�۹ɶ�̬����,ʹ��DCT_GENERAL����
struct STK_HKDYNA
{
	DWORD	m_dwIEPrice;	//ƽ���
	MWORD	m_mIEVolume;	//ƽ����

	//�����̱���
	WORD	m_wBuyOrderNum[5];
	WORD	m_wSellOrderNum[5];

	//���Ͷ���
	struct HK_BROKER	
	{
		WORD	m_wNumBroker;
		WORD	m_wBrokerNo[40];
		char	m_cType[40];		//'B':broker,'R':register trader,'S':number of spread
	}m_buyBroker,m_sellBroker;
};

//ծȯƱ�����ʡ�Ӧ����Ϣ����Ʊÿ�������ʹ��STK_STATICEx����

////////////////////////////////////////////////////////////////////////////////////////////////
//�Ϻ� Level2

//��չ������
#define  SHL2_MMP_SIZE	5

struct SH_L2_MMPEX
{
	WORD	m_wStkID;			//��ƱID
	DWORD	m_dwAvgBuyPrice;	//��Ȩƽ��ί��۸�
	MWORD	m_mAllBuyVol;		//ί������
	DWORD	m_dwAvgSellPrice;	//��Ȩƽ��ί���۸�
	MWORD	m_mAllSellVol;		//ί������
	DWORD	m_dwBuyPrice[SHL2_MMP_SIZE];	//ί���6-10
	DWORD	m_dwBuyVol[SHL2_MMP_SIZE];		//ί����6-10
	DWORD	m_dwSellPrice[SHL2_MMP_SIZE];	//ί����6-10
	DWORD	m_dwSellVol[SHL2_MMP_SIZE];		//ί����6-10
};

//ί�ж���

//ί�ж��������붨�壬��ÿ��DWORD�����2λ�����������ͣ�ÿ����Ʊ�Ķ�������˳��Ϊ
//  ��ƱID | ���̼۸�1 | �۸�1�ܵ��� | ��1 | ��2 ... | ���̼۸�2 | �۸�2�ܵ��� | ��1 | ��2 ... | ���̼۸�1 | �۸�1�ܵ��� | ����1 | ����2 ... 
//�۸�������Ǹ���һ���ܵ���

#define QUEUE_TYPE_MASK	0xC0000000

#define QUEUE_ORDER		0x0
#define QUEUE_BUYPRICE	0x40000000
#define QUEUE_SELLPRICE	0x80000000
#define QUEUE_STOCKID	0xC0000000

//��������ί�ж���

//ί�ж��������붨�壬��ÿ��DWORD�����2λ�����������ͣ�ÿ����Ʊ�Ķ�������˳��Ϊ,�����ܵ����ĵ���λ�����������ӻ��Ǹ�Inorder��Ŀ + InOrder
//  ��ƱID | ���̼۸�1 | �۸�1�ܵ��� | ����Ŀ| ��1 | ��2 ... | ���̼۸�2 | �۸�2�ܵ��� | Inorder��Ŀ| InOrder1 | InOrder2 ... | ���̼۸�1 | �۸�1�ܵ��� |������Ŀ| ����1 | ����2 ... 
//�۸�������Ǹ���һ���ܵ���,�۸��ܵ����������Ǹ���һ������Ŀ
#define QUEUE_ORDER_IN 0x20000000 //����۸��ܵ���add�����ֵΪ1����ʾ�ü�λ���ܹ���x������,������Inorder��Ŀ
//��������
struct InOrder
{
	BYTE type;//Ϊ0����,Ϊ1���ֳɽ�,Ϊ2��ȫ�ɽ�,Ϊ3����			
	WORD pos;//λ�������ǰһ��״̬
	DWORD vol;//����,��
};

//��ʳɽ���ϸ
struct SH_L2_REPORT
{
	WORD	m_wStkID;			
	time_t	m_time;				//�ɽ�ʱ��
	DWORD	m_dwPrice;			//�ɽ��۸�
	DWORD	m_dwVolume;			//�ɽ���
};

//������󳷵�
struct SH_L2_BIG_WD			
{
	WORD	m_wStkID;
	time_t	m_time;				//����ʱ��
	time_t	m_submitTime;		//ί��ʱ��
	DWORD	m_dwPrice;			//ί�м۸�
	DWORD	m_dwWidthdrawVol;	//������
};
	
//�ۼƳ���
struct SH_L2_ACCU_WD	
{
	WORD	m_wStkID;			//��ƱID
	time_t	m_time;				//ʱ��
	DWORD	m_dwWidthdrawVol;	//������
};

//ί�е�ͳ�ƣ���������Level2
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
	WORD	m_wStkID;				//��ƱID
	MWORD	m_nBuyOrderVol[4];		//���뵥����С���С����ش�
	MWORD	m_nSellOrderVol[4];		//����������С���С����ش�
	UINT24	m_dwOrderNum[2];		//ί�е�����������/����
	MWORD	m_nWDVol[2];			//����/����������,������ʱ���޷�����������ܳ���������һ��

	void	Init(WORD wStkID);		//��ʼ������������
	void	AddNewOrder(int nDir,DWORD dwPrice,DWORD dwVol,BYTE nPriceDigit);	//����ί�е�ʱ���ã�nPriceDigit���� STK_STATIC::m_nPriceDigit,nDir>0��ʾ�򵥣�<0��ʾ����
	void	AddNewWithdarw(int nDir,DWORD dwVol);			//�����ɹ�����ʱ���ã�nDir=0��ʾ��������1��ʾ���볷����-1��ʾ��������
};

//didģ��crc����
struct DC_DSCRC
{
	DWORD m_dwDid;//����did��
	DWORD m_dwCrc;//did��ģ��crc
};

//��������֧�ֵ�dids
struct DC_DSDID
{
	DWORD m_dwDid;//����did��
	BYTE m_bSend:1;//Ϊ1��Ҫ���·���ģ���ļ�
	BYTE m_bFull:1;//Ϊ1ֻ�ܷ���ȫ��ѹ�����ݣ�Ϊ0ȫ��������ѹ�����ݶ����Է���
	BYTE m_bNoCompress:1;//Ϊ1������ѹ�����ݣ�Ϊ0����ѹ������
	BYTE recv:5;//
};

struct DC_DSLEN
{
	DWORD m_dwDid;//����did��
	DWORD m_dwLen;//did��ģ�峤��
};

//didģ������
struct DC_DSDATA
{
	DWORD m_dwDidNum;//did����
	DC_DSLEN mDsLen[1];//��m_dwDidNum��
};
//����������ģ������


enum DC_RECFULL_TYPE	//ѹ������
{
	DC_RECFULL_FLAG = 0x80000000,//��¼��ȫ�����MASK
};

//did����,�������ݸ���did��Ӧ��ģ�����ɺͽ���,DC_DID����=DC_DIDHead+������DC_DIDCompress��

//������һ���䳤�������ͱ�������С3���ֽڣ����9���ֽڣ����λ��Ϊ������;,�����Ͳ�����sizeof����,��GetLen����
struct DC_DIDHead
{
	BYTE m_bDidType:2;//0��ʾ�����m_dwDidΪ13λ,1Ϊ21λ,2Ϊ29λ,3Ϊ61λ
	BYTE m_bNumType:2;//0��ʾ�����m_dwRecNumΪ1���ֽ�BYTE,1��ʾΪ2���ֽ�WORD,2��ʾΪ3���ֽڵ�UINT24,3��ʾΪ4��DWORD
	BYTE m_bFullRec:1;//ȫ��¼,���ο��Խ���ǰ������ȫ������
	BYTE rec:3;//����
	BYTE m_bEx[1];//���8���ֽ�

	//��ȡdid
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
	//��ȡ���ݼ�¼����
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

    //����did�ͼ�¼����
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

    //��ȡ�ṹ���С
	int GetLen(void)
	{
		return (m_bDidType+m_bNumType+3);
	}
};

//���DCHEAD���ѹ������ΪDC_XMLCID_CPS����DC_SOCID_CPS,DC_DIDHead��������½ṹ������
//һ���䳤�������ͱ�������С2���ֽڣ����5���ֽڣ���һ���ֽ���Ϊ������;,�����Ͳ�����sizeof����,��GetLen����
struct DC_DIDCompress
{
	BYTE m_bLastCmp:1;//Ϊ1��ʾѹ�������ݸ�ǰ�����Ƭ��أ���Ҫ����GetBmData����
	BYTE m_bUnComLenType:2;//0��ʾ�����m_dwDidΪ1���ֽ�BYTE,1��ʾΪ2���ֽ�WORD,2��ʾΪ3���ֽڵ�UINT24,3��ʾΪ4��DWORD
	BYTE m_bCid:5;//ѹ��ID

	BYTE m_bEx[1];//���4���ֽڣ���С1���ֽ�

    //��ȡѹ��ǰ���ݳ���
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

    //����ѹ��ǰ���ݳ���
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

    //��ȡ�ṹ���С
	int GetLen(void)
	{
		return (m_bUnComLenType+2);
	}
};


//�����������������


//////////////////////////////////////////////////////////////////////////////////
//ͨ����ؽṹ

//ͨ�Ŷ˿�
#define DC_PORT	9966

//��������
//if dc_type is changed, parse.cc file will be changed too.
enum DC_TYPE
{
	DCT_NUL = 0,
	DCT_KEEPALIVE,			//<-->��������1
	DCT_LOGIN,				//<-->��½�Լ�����������	2
	DCT_REQPASS,			//<-->Ҫ���û��������½3
	DCT_USERnPASS,			//<-->�û�������4
	DCT_READY,				//-->��½�ɹ������Խ�������5
	DCT_RESEND,				//-->�ط����ݰ�6
	DCT_STKSTATIC,			//<--��̬����7
	DCT_STKDYNA,			//<--��̬����8
	DCT_SHL2_MMPEx,			//<--level2��չ������9
	DCT_SHL2_REPORT,		//<--level2�ֱʳɽ�10
	DCT_SHL2_BIG_WD,		//<--level2��ʱ��󳷵�11
	DCT_SHL2_ACCU_WD,		//<--level2�ۼ���󳷵�12
	DCT_HK_STATIC,			//<--�۹ɾ�̬13
	DCT_HK_DYNA,			//<--�۹ɶ�̬14
	DCT_XML,				//<--XML��ʽ����15
	DCT_SHL2_QUEUE,			//<--�����̶��У�level2����16
	DCT_GENERAL,			//<--ͨ������17
	DCT_USERSTAT,			//-->�û�����ͳ��18
	DCT_RAWDATA,			//<-->ԭʼ����19
	DCT_NEWS,				//<--�������ŵ��ı���������20
	DCT_SZL2_ORDER_QUEUE,	//<--ί�ж��У�����Level2����21
	DCT_SZL2_ORDER_STAT,	//<--ί�ж���ͳ�ƣ�����Level2����22
	DCT_DSCRC,				//<--didģ��crc,23
	DCT_DSDID,				//<--��Ҫ����ģ���did 24
	DCT_DSDATA,				//<---didģ������ 25
	DCT_DID,				//DID����,����ģ����� 26
    DCT_DIDSTATIC,          //DID���ݣ�ģ���Ϊ100003����̬������ݣ��ṹΪDC_HEAD+DC_STKSTATIC������STK_STATIC��+DC_DIDHEAD+DC_DIDCompress+DATA
	DCT_INCODESTATIC,		//<--����ת���������ľ�̬����,28

	DCT_SZL2_ORDER_FIVE=100,//<--ί�ж��ж����ϸ������Level2����
	DCT_SZL2_TRADE_FIVE=101,//<--�ɽ�,�������ж����ϸ������Level2����
};

#define SIMPLE_ENCY_MASK	0x7C59ACDC

enum DC_ENCY_TYPE	//���ܷ���
{
	DC_NOENC = 0,			//���봫��
	DC_ENCY_SIMPLE,			//�򵥼���
	DC_ENCY_DES,			//DES����	(##)
	DC_ENCY_AES,			//AES����

	DC_ENCY_MASK = 7		//���ܷ���MASK
};
enum DC_COMPRESS_TYPE	//ѹ������
{
	DC_NOCOMPRESS = 0,		//��ѹ��
	DC_STD_CPS =	0x8,	//��׼����
	DC_ZLIB_CPS =	0x10,	//ZLIBѹ��
	DC_ZIP_CPS =	0x18,	//ZIP��ʽ
	DC_EXT_CPS =	0x20,	//��չѹ�����Զ���
	DC_XMLCID_CPS = 0x28,	//��ʾ��������xmlcidģ��ѹ����ѹ
	DC_SOCID_CPS =	0x30,	//��ʽ��������so����socid����ģ��ѹ����ѹ

	DC_CPS_MASK = 0x38		//ѹ������MASK
};

//ZLIB����ͷ���������ZLIB,ZIPѹ������DC_HEAD�������ZLIB_HEADͷ��Ȼ����ѹ���������,ѹ��������ݳ�����DC_HEAD::m_nLen-sizeof(ZLIB_HEAD)
struct ZLIB_HEAD
{
	DWORD m_dwSourceLen;	//Դ���ݳ���
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//���ݰ�ͷ
//ÿ�����ݰ�����DC_HEAD��ͷ������DC_HEAD::m_cType���������ͬ�Ľṹ

#define DC_TAG			0x6C
//define for DC_HEAD::m_wAttrib
#define DC_DUP			0x8000		//�����ݰ����ط����ݰ�
//bit0--bit3: DC_ENCY_TYPE���ܷ�����DC_HEAD֮��������Ѿ�ʹ�ø÷�������
//bit4--bit6: DC_COMPRESS_TYPEѹ��������DC_HEAD֮��������Ѿ�ʹ�ø÷���ѹ�������ͬʱ���ܣ������ڽ��ܺ��ٽ�ѹ��
//bit7--bit14: ����������Ϊ0
struct DC_HEAD
{
	BYTE	m_cTag;			//����ΪDC_TAG
	BYTE	m_cType;		//DC_TYPE
	WORD	m_wAttrib;		//����
	UINT24	m_nLen;			//������,��������ͷ
	UINT24	m_nSeq;			//�����,����ȫ��������ֵ,����did����//���к�,�ӳ�ʼ���Ժ�ʼ��0���ӵ�0xFFFFFFFF,Ȼ���ֱ��0��������
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_KEEPALIVE,			//<-->�������ӣ�ÿ30��˫������Ҫ��Է�����һ�����ݰ�����û����������DC_KEEPALIVE��
struct DC_KEEPALIVE 
{
	UINT24 m_wLastRcvSeq;		//����������仯������ÿһ����Ʊ���鷢���仯�������ݼ�1�������ʼ�����������0
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_LOGIN <--��½�Լ�����������
//define for DC_LOGIN::m_dwAttrib
#define DCATTR_SHLEVEL2		0x1			//���������ڸ��г���Ҫ��ʼ��level2��չ�����̿⣬��������ۼƳ�����
#define DCATTR_HKSEC		0x2			//���������ڸ��г���Ҫ��ʼ���۹ɿ�
#define DCATTR_VARTIME		0x4			//���г�Ʒ�ִ��ڲ�ͬ�Ľ���ʱ�Σ���Ҫ��RAWDATA���Ͷཻ��ʱ�ζ����ļ�"MK_VT.txt"
#define DCATTR_SZLEVEL2		0x8			//����Level2,�ñ�־����Ҫͬʱӵ��DCATTR_SHLEVEL2,��ͬ��ʾ����Level2�����������ڸ��г���Ҫ��ʼ��level2���ί�п�

#define DCATTR_INCODESTATIC	0x4000000	//��������DCT_STATIC
#define DCATTR_NOSTATIC		0x8000000	//ȡ����������DCT_STATIC����static��
#define DCATTR_DIDSTATIC	0x10000000	//����100003.xml��ʽ��did����

#define DCATTR_DIDDAT		0x20000000	//֧��did����Э���־
#define DCATTR_FULLDATA		0x40000000	//ȫ�����ݱ�־�����ڲ��Ժ�¼�����ݵ�ͨ��
#define DCATTR_RAWSVR		0x80000000	//RAW����Դ��½��־

struct DC_LOGIN 
{
	WORD	m_wMarket;				//�г�����
	char	m_strMarketName[32];	//�г�����
	int		m_nTimeZoneBias;		//����ʱ��
	short	m_nNumOpenTime;			//����ʱ������������ʹ�ö��ٸ�m_nOpenTime,m_nCloseTime
	short	m_nOpenTime[8];			//����ʱ�䣬HHMM
	short	m_nCloseTime[8];		//����ʱ��
	DWORD	m_dwAttrib;				//����������
	WORD	m_wPriority;			//���ȼ�������������Դ�ĺû���0��ߣ�0xFFFF���
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_REQPASS -->Ҫ���û��������½
struct DC_REQPASS 	//��##��
{
	DC_ENCY_TYPE m_encyType;
	DWORD	m_dwKey[2];		//���ܹ�Կ
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_USERnPASS,			//<--�û�������
struct DC_USERPASS 
{
	char	m_strUser[16];
	char	m_strPass[24];
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_READY,				//-->��½�ɹ������Խ�������
struct DC_READY 
{
	enum DC_INSTRUCT
	{
		SUCCESS = 0,			//�ɹ���ָʾ���������Կ�ʼ��������
		WAIT,					//�ɹ���ָʾ�������ȴ���ֱ���յ�����SUCCESS��־��DC_READY���ݰ��ٿ�ʼ��������
		DISCONNECT,				//���ɹ����Ͽ�����
		IDLE,					//�ɹ���������ָ��������Ϊ��������Դ��ָʾ�������ȴ���ֱ���յ�����SUCCESS��־��DC_READY���ݰ��ٿ�ʼ��������
		QUOTEDAT,				//�ɹ�,֪ͨclient���跢��QUOT���ݸ�������
		DEALDATAOVER,//���ݴ������
	} m_instruct;				//ָ��
	enum DC_ERROR
	{
		NOERR = 0,
		INVALID_USERPASS,		//�Ƿ��û�������
		MARKET_ALREADY_EXSIST,	//�г��Ѿ�����
		NOT_SUPPORT_MARKET,		//��������֧�ָ��г�
		SERVER_NOTREADY,			//��������ʱ�����ã��Ժ�������
	} m_errCode;				//������
	DWORD	m_dwDelayConnectTime;	//�ȴ�ʱ�䣬�����������ΪSERVER_NOTREADY������m_dwDelayConnectTime��Ϊ0����ȴ���ô����ʱ���ٴ�����
	DWORD	m_dwStaticLabVersion;	//��̬��汾������������ľ�̬��汾�����£��Ͳ���Ҫ���;�̬����
	BOOL	m_bCompress;			//Ҫ��������ѹ��
	DC_ENCY_TYPE	m_dataEncyType;	//����Ҫ����ܡ���Ҫ����ܣ��������з��͵������������ݰ����������ܷ���
	DWORD	m_dwDataKey[2];			//���ݼ��ܷ��͹�Կ,����ȫ���е�һ��DWORD���ϴεİ����
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_RESEND -->�ط����ݰ�
struct DC_RESEND 
{
	DWORD m_dwBeginSeq;		//��Ҫ�ط������ݰ���ʼλ��
	int	  m_nNum;			//�ط�����,���Ϊ0���ʾ֮���ȫ��,���<0���ʾȡ���ط�����,������ֹͣ�ط�
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_STKSTATIC,			//<--��̬����,(��̬����Ҫ����һ�����ڷ������?)
//define for DC_STKSTATIC::m_wAttrib and DC_STKSTATIC2::m_wAttrib
#define DC_ADDSTK			0x1		//�����һ���汾�������˹�Ʊ
#define DC_LABEL_CHANGED	0x2		//�����һ���汾������仯��
#define DC_NAME_CHANGED		0x4		//�����һ���汾�����Ʊ仯��
#define DC_DATA_CHANGED		0x8		//�����һ���汾�����ݷ����˱仯


struct DC_STKSTATIC 
{
	DWORD	m_dwVersion;	//��̬���ݰ汾,ÿ�ξ�̬���ݷ����仯�����������ֵ,ÿ������Դ�����һ����Զ����ĸ�λ��������ͬ����Դ�Ͳ�������ͬ�İ汾,0x80000000
	WORD	m_wAttrib;		//�汾����
	UINT	m_nDay;			//��̬�����ڣ�YYYYMMDD
	short	m_nNum;			//m_data����
	STK_STATIC m_data[1];	//����
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_STKDYNA,			//<--��̬����
struct DC_STKDYNA 
{
	WORD	m_wDynaSeq;		//��̬�������кţ����ڿͻ���ת��
	short   m_nNum;
	STK_DYNA m_data[1];
};


////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_SHL2_MMPEx,			//<--level2��չ������
struct DC_SHL2_MMPEx 
{
	short	m_nNum;
	SH_L2_MMPEX m_data[1];
};


////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_SHL2_QUEUE,			//<--level2�����̶���
struct DC_SHL2_QUEUE
{
	DWORD	m_dwQueue[1];	//������������ͷ���Ⱦ���
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_SZL2_ORDER_QUEUE,			//<--level2���ί�ж���,���ݸ�ʽ�������̶��� DC_SHL2_QUEUE һ�£�������һϵ��DWORD���

////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_SHL2_REPORT,		//<--level2��ʳɽ�
struct DC_SHL2_REPORT 
{
	short	m_nNum;			//����
	SH_L2_REPORT m_data[1];	//�ܹ�m_nNum��
};


////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_SHL2_BIG_WD,		//<--level2��ʱ��󳷵�
struct DC_SHL2_BIG_WD 
{
	char	m_cBidOrAsk;	//'B'��ʾ�򵥣�'A'��ʾ����
	short	m_nNum;			//����
	SH_L2_BIG_WD m_data[1];	//�ܹ�m_nNum��
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_SHL2_ACCU_WD,		//<--level2�ۼ���󳷵�
struct DC_SHL2_ACCU_WD 
{
	char	m_cBidOrAsk;	//'B'��ʾ�򵥣�'A'��ʾ����
	short	m_nNum;			//����
	SH_L2_ACCU_WD m_data[1];//�ܹ�m_nNum��
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_SZL2_ORDER_STAT,		//<--level2���ί��ͳ��
struct DC_SZL2_ORDER_STAT
{
	short	m_nNum;					//����
	time_t	m_time;					//����ʱ��
	SZ_L2_ORDER_STAT	m_data[1];	//����m_nNum��
};

////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_XML					//<--XML��ʽ����
struct DC_XML 
{
	enum XML_PROCESS
	{
		SVR_PROCESS = 0,	//����������
		SVR_SEND,			//������ת����ÿ���ͻ�
		INFO,				//��Ѷ,����F10�����ŵ��ı�����
		LIST_MENU,			//��̬�б��ļ��˵�
		LIST_TABLE,			//��̬�б��ļ�

		PUSHINFO_ROLL = 100,		//������Ѷ
		PUSHINFO_ROLL1,			//������Ѷ1,�������ʹ��
		PUSHINFO_ROLL2,			//������Ѷ2,�������ʹ��
		PUSHINFO_ROLL3,			//������Ѷ3,�������ʹ��
		PUSHINFO_ROLL4,			//������Ѷ4,�������ʹ��
		PUSHINFO_ROLL5,			//������Ѷ5,�������ʹ��
		PUSHINFO_ROLL6,			//������Ѷ6,�������ʹ��
		PUSHINFO_ROLL7,			//������Ѷ7,�������ʹ��
		PUSHINFO_ROLL8,			//������Ѷ8,�������ʹ��
		PUSHINFO_ROLL9,			//������Ѷ9,�������ʹ��

		PUSHINFO_MOTION = 200,	//�����2���յ������2�󣬻��滻�����1���У������2�Ƕ�����Ϣ������֧�ֳ���
	};
	XML_PROCESS	m_processType;	//XML_PROCESS
	WORD	m_wMarket;			//�г����룬���г���������0
	DWORD	m_dwData;			//���ݱ�ʶ��ͬһ�������£�ÿ��������һ��Ψһ�ı�ʶ�����շ��Ḳ����һ���յ���m_processType��m_dwData����ͬ������
	DWORD	m_dwExtra;			//�����ʶ����ÿһ�������Լ����壬���ڹ�����Ѷ����������ʾYYYYMMDD��ʽ���ļ�����
	int		m_nLen;				//���ݳ���
	char	m_strXML[1];		//����Ϊm_nLen;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//DCT_GENERAL ͨ������

//define for DC_GENERAL::m_dwAttrib
#define GENERAL_FIXED		0				//��������
#define GENERAL_VAR			1				//�䳤����
#define GENERAL_ATTR_MASK	0xFF

#define GENERAL_FLOAT		0x100			//������
#define GENERAL_DWORD		0x200			//DWORD
#define GENERAL_CHAR		0x300			//�ַ���
#define GENERAL_TIMEFLOAT	0x400			//time_t + float����
#define GENERAL_STRUCT		0x500			//�ṹ����
#define GENERAL_TYPE_MASK	0xF00

#define GENERAL_STATIC		0x1000			//��̬���ݣ�����һ�㲻�����仯
			
#define GENERAL_WHOLE		0x10000000			//�������ݣ������ݰ��������г�����������
#define GENERAL_INTR_SEND	0x20000000			//��ע��Ʊ���ͣ������ݷ����仯�������͹�ע��Ʊ������
#define GENERAL_CONN_SEND	0x40000000			//�ͻ�������ʱ���͸��ͻ�
#define GENERAL_DIR_SEND	0x80000000			//�������յ���ֱ��ת�������пͻ���,��������û�����ʱ����

#define GENERAL_FLOAT_FIX	(GENERAL_FIXED|GENERAL_FLOAT)	//float�͵Ķ�������
#define GENERAL_FLOAT_VAR	(GENERAL_VAR|GENERAL_TIMEFLOAT)	//float�͵ı䳤ʱ������ֵ
#define GENERAL_STRING_VAR	(GENERAL_VAR|GENERAL_CHAR)		//�ַ���
#define GENERAL_STRING_FIX	(GENERAL_FIXED|GENERAL_CHAR)		//�����ַ���
#define GENERAL_STRUCT_FIX	(GENERAL_FIXED|GENERAL_STRUCT)	//�ṹ��

struct DC_GENERAL
{
	WORD	m_wDataID;			//����ID
	WORD	m_wMarket;			//�г�
	DWORD	m_dwDataAttr;		//��������
	short	m_nDataSize;		//ÿ�����ݵĳ���,���ڱ䳤����,��ʾÿ�����ݵ����ֵ(0��ʾû�����ֵ�����ⳤ��)
	UINT24	m_nTotalLen;		//����������ͷ���ڵ������ܳ���
	WORD	m_wNum;				//����
	DWORD	m_dwReserved;		//��������Ϊ0
	WORD	m_wStock[1];		//m_wNum��,���m_wStock==0xFFFF����ʾ�����ݲ����κι�Ʊ����
};


/*-------------------------------------------------------------------------------------------------
���ڶ������ݣ��ṹ����
DC_GENERAL | m_wNum��StockID | m_wNum�����ݣ�ÿ�����ݳ���Ϊm_nDataSize

���ڱ䳤���ݣ��ṹ����
DC_GENERAL | m_wNum��StockID | m_wNum������λ������(����ΪUINT24,�ӱ��ṹͷλ�ÿ�ʼ����) | m_wNum�����ݣ�ÿ�����ݳ�����������ֵ����,���һ����Ҫ��m_nTotalLen����
--------------------------------------------------------------------------------------------------*/

#define GE_MAX_EXTRA	10000	//<GE_MAX_EXTRA���ڱ�ʾ��������,����������չ
//Ԥ����ͨ����������
#define GE_IOPV			5		//ETF,LOF��̬��ֵ��ֵ,GENERAL_FLOAT_FIX
#define GE_MATU_YLD		6		//ծȯ���������ʣ�GENERAL_FLOAT_FIX
#define GE_HKREL_TXT	1001	//�۹������Ϣ��GENERAL_STRING_VAR,(���ڱ�ʾHKSEC_STATIC::m_cRelateText)

#define GE_STATIC_EX	(GE_MAX_EXTRA+1)	//��̬��չ��GENERAL_STRUCT_FIX,(���ڱ�ʾ STK_STATICEx)
#define GE_HKDYNA		(GE_MAX_EXTRA+2)	//�۹ɶ�̬��GENERAL_STRUCT_FIX,(���ڱ�ʾ STK_HKDYNA)
#define GE_BLK_STK		(GE_MAX_EXTRA+3)	//���ָ���ɷݹ�,GENERAL_VAR,��ʾ���ָ���ɷݹ�

/////////////////////////////////////////////////////////////////////////////////////
//DCT_USERSTAT

#define USERNAMELEN 50//�û�������
#define USERPWDLEN 20//���볤��

typedef struct tagStatSingleHqsInfo
{
	char exist_flag;//Ϊ-1û�и�����Դ,Ϊ1�и�����Դ
	char hqs_ip[32];
	int m_wLastSeq;//Ŀǰ�յ��ı仯����
	time_t fresh_time;//
}StatSingleHqsInfo,*LPStatSingleHqsInfo;

typedef struct tagStatHqsInfo
{
	char market_code[3];//�г�����
	int main_hqs_flag;//-1û��������Դ,0Ϊ��һ�����ȼ�����ԴΪ������Դ,�Դ��������ֵΪ3
	int init_flag;//�뿪��ʱ��ֻ��10���ӵ���û�г�ʼ��Ϊ3,1Ϊ����,2Ϊ�ȴ���ʼ��;
	StatSingleHqsInfo mStatSingleHqsInfo[4];
}StatHqsInfo,*LPStatHqsInfo;

typedef struct tagUserType
{
	unsigned short YYBID;
	int online_user_num;//�����û���
}UserType,*LPUserType;

typedef struct tagUserVer
{
	short ver;
	short build;//build��

	int online_user_num;//�����û���
}UserVer,*LPUserVer;

typedef struct tagServDataInfo
{
	unsigned char m_ucMonitorFlag; //���״̬,1Ϊ����״̬,2Ϊ�쳣״̬,3Ϊ�޷����,4Ϊ�ȴ�����
	unsigned char m_ucZmFlag; //ת�빤��״̬,3Ϊ�ȴ���ʼ��,1Ϊ��ʼ��ʼ��,2Ϊ��ʼ�����
	unsigned char m_ucUserFlag; //�û����빤��״̬,1Ϊ�ȴ���ʼ��,2Ϊ��ʼ��ʼ��,3Ϊ��ʼ�����

	unsigned short m_usZmProcessNum;//ת�������
	unsigned short m_usUserProcessNum;//�û����������
	unsigned short m_usOnlienUserNum;//�����û���
	unsigned short m_usMaxUserNum;//������ʷ��ֵ
	unsigned char m_ucNfsFlag;//��Ѷ�Ҿ�״̬
	char m_chZhdocData[16]; //��Ѷ�������ʱ��
	unsigned short m_usZmCpu;//ת�����CPUռ����
	unsigned short m_usUserCpu;//�û��������cpuռ����
	unsigned short m_usMemorySize;//ϵͳ�ڴ��С(MBytes)
	unsigned short m_usSwapSize;//SWAP��С(MBytes)
	unsigned short m_usUseSwapSize;//����SWAP��С(MBytes)

	unsigned short m_usLicUserNum;//�����û���
	DWORD m_ulLicDate;//��������
	DWORD m_ulUserPort;//DzhUserNetʹ�õķ���˿�
	DWORD m_ulDlPort;//��������ʹ�õķ���˿�
	char osname[32];//����ϵͳ���ư汾��
	unsigned short m_usCpuNum;//ϵͳcpu��Ŀ
	DWORD m_ulSysBootDate;//ϵͳ��������(20050707)
	DWORD m_ulSysBootTime;//ϵͳ����ʱ��(095930)

	DWORD m_ulRunTime;//ϵͳ��������
	DWORD m_ulSysDate;//ϵͳ����(20050707)
	DWORD m_ulSysTime;//ϵͳʱ��(095930)

	DWORD m_ulZmStartDate;//zm��������(20050707)
	DWORD m_ulZmStartTime;//zm����ʱ��(095930)
	time_t m_nZmRunTime;//zm����ʱ��

	DWORD m_ulUserStartDate;//user��������(20050707)
	DWORD m_ulUserStartTime;//user����ʱ��(095930)
	time_t m_nUserRunTime;//user����ʱ��

	int valid_user_type_num;
	UserType mUserType[10];

	int valid_user_ver_num;
	UserVer mUserVer[20];

	int valid_market_num;//��Ч�г���Ŀ
	StatHqsInfo mStatHqsInfo[10];//���10���г�
}ServDataInfo,*LPServDataInfo;

struct DC_USERSTAT
{
	in_addr	m_svrAddr;		//������IP
	WORD	m_wSvrPort;		//�������˿�
	ServDataInfo  m_data;
};

/////////////////////////////////////////////////////////////////////////////////////
//DZH_AUTHORIZE  ��Ȩ�ṹ
//���������κ���Ҫ�����û������Ȩʹ�õ����
//�ֳ��������֣�һ�Ƿ�����Ȩ����ʾ�û�������һ�࣬�����Ʒ��ȯ�̡�Ӫҵ������Ϣ�����ǲ�Ʒ���ܡ��г���Ȩ����bit���

enum DZH_GROUP_TYPE
{
	DG_NIL = 0,					//��Ч
	DG_PRODUCT,					//��Ʒ
	DG_SC_CPY,					//ȯ��
	DG_SC_DEP,					//Ӫҵ����������DG_SC_CPYȯ�̷���֮���������
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
	DZH_AUT_GROUP	m_groupAuth[DZH_AUT_GROUP_SIZE];		//����࣬����8�����ʹ�ã���������0-8������Ȩ����Ч��������ǰ�棬��Ч��m_wData==0
	UINT64			m_nMarketAuth;							//�г���Ȩ��ÿһ��bit��ʾһ���г�
	UINT64			m_nFuncAuth;							//������Ȩ��ÿһ��bit��ʾһ������

	void Init();
	BOOL IsGroupChecked(DZH_GROUP_TYPE type,DWORD dwGroup);
	BOOL IsBitChecked(BOOL bMarket,int nBit);
	BOOL HasAuth();
	int GetNumGroup() const;
	BOOL IsChecked(const DZH_AUTHORIZE* pAuth);				//pAuth��ʾ�û�Ȩ�ޣ���������ѶȨ��
};

/////////////////////////////////////////////////////////////////////////////////////
//DCT_RAWDATA ԭʼ����
//ԭʼ�����ǲ����˻�õ����ݣ�ֱ���ṩ���ͻ�������ʹ�õģ������ֻ��Ҫ���ݱ�־��ɴ洢������ȹ�������
//�����ݰ�����֧��ѹ���������ݰ�ͷDC_HEAD��Ӧ�ð����κ�ѹ����־
//ԭʼ����Ҳ֧��RAW����Դ������˴��͹��ܣ����ڶ��������ķ���˽��й㲥(����RAW_BROADCAST��־)

#define RAW_INITDEL		0x1				//�г���ʼ����ʱ�����������
#define RAW_SVRUSE		0x2				//����˴�����ļ������ṩ���ͻ���ʹ��
#define RAW_BROADCAST	0x4				//�㲥�ļ����������յ�DCT_RAWDATA�ļ�����������ñ�־����ת�������еĿͻ���������ø��ļ����ڷ����ָ��Ŀ¼��������
#define RAW_SVRSAVE_BC	0x8				//����˱�����ļ����û���½��ʱ����Ҫ��������
#define RAW_CLIENT_BC	0x10			//�ͻ��˹㲥�����������յ������ݺ�Ҫ�����û�����(�����RAW_AUTHORIZE)����ÿһ�����������Ŀͻ������͸�RAW����

#define RAW_AUTHORIZE	0x4000			//m_cData��ͷ����һ��DZH_AUTHORIZE�ṹ��֮�������ʽ���ݣ�m_dwOrgLen��m_dwDataLenҲ�����ó��ȣ������Ҫ����Ȩ�޾������ת��
										//�����ת�����ݸ��ͻ��ˣ����뽫DZH_AUTHORIZE�ṹɾ������ʱ��Ҫ����m_dwOrgLen��m_dwDataLen����ɾ��RAW_AUTHORIZE��־
										//��ͬʱ����RAW_ZLIBCPS��־��DZH_AUTHORIZE�ṹ�ǲ�ѹ���ģ���ʱ���������Ҫ��ѹ�����ݣ���Ҫ����m_dwOrgLen��m_dwDataLen���ȶ���Ҫ��ȥDZH_AUTHORIZE�ṹ����
#define RAW_ZLIBCPS		0x8000			//zlibѹ�����ݣ�m_cData�б���ʹ��zlibѹ��������
#define RAW_AES			0x10000			//AES����
#define RAW_NOSAVE		0x20000			//������

struct DC_RAWDATA
{
	WORD	m_wMarket;				//�г����������г������ݣ���Ϊ0����Ҫ���͵����г�������Ϊ'WN',��0x574E
	char	m_strFileName[14];		//�г���ԭʼ���ݵ�Ψһ��ʶ�������Ҳ�Ը��ļ�������ʹ������ݣ��ļ�������8.3����
	DWORD	m_dwAttrib;				//����
	DWORD	m_dwVersion;			//���ݰ汾
	DWORD	m_dwOrgLen;				//m_cDataԭʼ����
	DWORD	m_dwDataLen;			//ʵ�ʳ��ȣ����û��ѹ��������m_dwOrgLenһ��
	BYTE	m_cData[1];				//�ܳ���Ϊm_dwDataLen,ԭʼ���ݣ����ն����ļ���m_strFileName�����ļ�
};

//Ԥ������ļ���
#define FIX_BJ		"ZQ_QDBJ.txt"		//�̶�����ƽ̨����
#define FIX_HQ		"ZQ_CJHQ.txt"		//�̶�����ƽ̨����
#define FIX_MX		"ZQ_CJMX.txt"		//�̶�����ƽ̨�ɽ���ϸ
#define FIX_XX		"ZQ_ZQXX.txt"		//�̶�����ƽ̨֤ȯ��Ϣ

//��Ͻ���ʱ���г�����
//��������ȫ��ָ�����г�����ÿ��Ʒ�ֵĿ�����ʱ�䲻һ�£���Ҫ�������г��ṩÿ�����ɵĽ���ʱ��
//��Ͻ���ʱ���г�����Ҫ��DC_LOGIN::m_dwAttrib�а���DCATTR_VARTIME��־�������г�����ʱ������Ϊ���翪��Ʒ��--��������Ʒ�֣���һ��ʱ�Σ���Ʒ�ֵĽ���ʱ����������������
/*��ʽΪ:ÿһ����������ʱ�λ���һ��Ʒ�֣�����������������ʱ�Σ�����ÿһ�������ڸ�ʱ���µ�һ��Ʒ�ֵĴ���(�������г���־)������ʱ���ʽΪʮ����HHMM��
  ����������ڶ���Ҳ���ԣ������ܵ�ʱ�����<24Сʱ
���磺
[0930-1130 1300-1500]
600000
600001
[1000-1200 1300-1600 2100-0300]
600123
600999
*/
#define MK_VARTIME	"MK_VT.txt"			//�ཻ��ʱ�ζ����ļ�

#define L2_STAT_FILE	"L2STAT.ZIP"	//Level2ͳ���ļ������ڸ�����L1����Դ�ṩLevel2ͳ�ƣ�Ӧ�þ������� RAW_INITDEL|RAW_SVRUSE  (ע�⣬��Ȼ��չ����ZIP�����ǲ�����ZIPѹ���ļ�)
#define L2_STAT_CPS		"L2STAT.CPS"	//Level2ͳ���ļ������ڸ�L2����Դ�ṩLevel2ͳ�ƣ�Ӧ�þ������� RAW_INITDEL|RAW_SVRUSE,���ļ�����Level2ͳ������Դ���ɺ��͸�������Ȼ���ٹ㲥��ȥ

#define RAW_POOL_FILE	"PLSVC.XML"		//ϵͳ��Ʊ�ط����ļ���000PLSVC.XML--999PLSVC.XML,һ�����Զ���1000���ļ�
#define RAW_PLANA_FILE	"PLANA.XML"		//ϵͳ��Ʊ����ʷ���������ļ���000PLANA.XML--999PLANA.XML,һ�����Զ���1000���ļ�

#define C_SOH	0x1		// <SOH>=0x1,���ݷָ�����֮��������
#define C_LBL	0x2		// <LBL>=0x2,��Ʊ���룬�����Ʊ����
#define C_ID	0x3		// <ID>=0x3, ��ƱID����ϢID�������ƱID
#define C_ATR	0x4		// <ATR>=0x4, �����ַ�����������Լ��������У�'F'����������ʾ�����ݰ��Ǹ��г���ȫ�����ݣ��ͻ��˴���ʱ�ļ��в����ڵĹ�Ʊ����ɾ��������
#define C_TIM	0x5		// <TIM>=0x5,ʱ���ַ������������ʱ�䣬��ʽΪ"HH:MM:SS"���� "HH:MM"
#define C_DAT	0x6		// <DAT>=0x6,�����ַ���������������ڣ���ʽΪ"YYYYMMDD"
#define C_ENCODE 0xF	// <ENCODE>=0xF,�ַ����룬�����ǵ�һ����¼��Ĭ��ΪGB2312����GB2312����Ҫָ��������UTF-8,UTF-16��

//����һ�����ݣ����ڶ�����ɾ�̬����/�仯��������
//һ�����ݲ����ı���ʽ��ţ�ÿ����Ʊ������һ������
//�ļ������� "GNLxxxxx.GNL",xxxxx��ʾ�������ͣ���00000--99999�ܹ����Զ���1����,һ�����FIXЭ���еı�׼����
//�ļ�������"�ָ���+����"�ķ�ʽ��� �ָ�����С��0x10�Ķ���������������ASCII��
//���ݿ��Է�Ϊ���ַ�ʽ��һ���� ����+���ݣ���һ���� ID+���ݣ��������������ID������ʡ��ID
//������
//��ʽ1�����뷽ʽ
//<LBL>000001<SOH>100.01<LBL>000002<SOH>23.34<LBL>000012<SOH>11.23<LBL>000013<SOH>12.00
//
//��ʽ2��ID��ʽ
//<ID>0<SOH>100.01<ID>1<SOH>23.34<ID>11<SOH>11.23<ID>12<SOH>12.00
//��������ID�����Կ���ʡ��Ϊ
//<SOH>100.01<SOH>23.34<ID>11<SOH>11.23<SOH>12.00
//�ͻ���ͨ����ͬ�������ͣ�����GNLxxxxxx.GNL�ļ��������
//�������ݵ����ݰ汾�Ų���ԭʼ����CRC��������ʹ��zlibѹ��������RAW_ZLIBCPS��
//Ŀǰ�������ݣ�ÿ����������ֻ�ܴ���һ���ļ��������и��ɶ���������һ���ļ��д��䣬��һ�������֧��������ʽ���ټ���������ʽ����

//Ԥ�������һ�������������ͣ�
#define GNL_SZSTK_STATAS	8901	//֤ȯ���ǰ׺,�����г�ר�ã�4�ֽ��ַ���
#define GNL_STKNAME_ENG		107		//֤ȯӢ�ļ��,�ַ���
#define GNL_ISIN			48		//֤ȯISIN����,�ַ���
#define GNL_ROUND_LOT		561		//���׵�λ�����ڳɽ�����λ�뽻�׵�λ��ͬ�Ĺ�Ʊ������۹��г�

//ʵʱ�۲�����
//ʵʱ�۲����ݲ�����FIXЭ��,ʹ��<TIM>,<LBL>��<SOH>�ָ�����,�ֱ��ʾʱ�䣬���⣬���ݣ����б����ǿ�ѡ�ģ����û�б�����ʹ�����ݵ�ǰ70�ֽ���Ϊ����
//���ݿ��Բ��ô��ǻ�RichText��������ʹ��<* *>������ɫ�����壬�ַ���С�����ԣ���ʹ��<*a href=URL*>xxx<*/a*>����������xxx����ʾΪ����
//ʵʱ�۲���Դ�����ѶID,��<ID>�����������ظ�ID���Ḳ��ǰһ����ͬID����Ѷ,���û������ֻ��ID�����ʾɾ����ID������һ������1��20��֮�������,
//���� <TIM>9:30<SOH>������<*a href=DZH://VIEWSTOCK?LABEL=SH600000*>600000�ַ�����<*/a*>����,���������<*a href=http://info.gw.com.cn/viewinfo.asp?id=123413*>��ϸ����<*/a*>
//��RAW�ļ����԰���Ȩ��������ֻ��ӵ����ӦȨ�޵Ŀͻ��˲Ż��յ���ʵʱ�۲�����
//RAW�ļ�������� RAW_SVRUSE | RAW_CLIENT_BC ��־
//�ļ���Ϊ "xxxxNTFY.NTF",����xxxxΪ0000--9999��ÿһ����ʵʱ�۲���һ���ļ�����Ĭ��Ϊ0000NTFY.NTF
#define RAW_NOTIFY_FILE		"NTFY.NTF"		

//�������
//������������Flash��html����jpeg�ļ�,����ѹ��ΪZIP��ʽ����
//�����������ǵ����ļ�����ѹ���ļ�����Ҫ������Ƕ���ļ���������Ǹ�Ŀ¼�µ�"default.htm"��"default.swf"��"default.jpg"
//�����ļ�������Ϊdefault.cfg,���Ƶ���������ʾʱ��ͳߴ磬ÿһ�б�ʾһ�����������ʽ����
//  size=���*�߶ȣ��������ĳߴ�
//  popup=ʱ�� [random] [repeat(N)]�� ����������ʾʱ�䣬�����ж���
//  link=URL, ����jpg��ʽ�Ĺ�棬����֧�ֳ��������ͼ�����ӵ�����
//  ���磺 
//  size=800*600					�ߴ�Ϊ800*600
//  popup=14:40						����ʱ��Ϊ14:40,����һ��
//  popup=11:00 random repeat(10)	����ʱ��Ϊ11:00��10����֮�ڵ����ʱ�䣬�ظ�10��
//����ļ�Ĭ�ϴ�СΪ400*300�������������ߴ磬jpge�����Զ������ߴ磬���������ļ�����Ҫ�ڿ����ļ��м���ߴ�����
//��RAW�ļ����԰���Ȩ��������ֻ��ӵ����ӦȨ�޵Ŀͻ��˲Ż��յ�������
//���ļ�һ�����RAW_CLIENT_BC�����͵��ͻ���
//�ļ���Ϊ "xxxxADVT.ZIP",����xxxxΪ0000--9999��ÿһ���͵��������һ���ļ�����Ĭ��Ϊ0000ADVT.ZIP
#define RAW_ADVERTISE		"ADVT.ZIP"

//����ָ��
//����ָ�����xml��ʽ����������ͻ���ִ��һ���Ĳ���
//��RAW�ļ����԰���Ȩ��������ֻ��ӵ����ӦȨ�޵Ŀͻ��˲Ż��յ�������
//���ļ�һ�����RAW_CLIENT_BC�����͵��ͻ���
//�ļ���Ϊ "xxxxCOMD.XML",����xxxxΪ0000--9999��ÿһ����������һ���ļ�����Ĭ��Ϊ0000COMD.XML
#define RAW_COMMAND_FILE	"COMD.XML"		

//DFIX����
//DFIX��ʽ�Դ����͡�������ͣ�����˸��ݽ������������ݴ�ŵ���Ӧ�ķ�����
//������RAW_CLIENT_BC����ͬʱ���͵��ͻ���
//�ļ���Ϊ "xxxxDFIX.DAT",����xxxxΪ0000--9999�������������ֲ�ͬ���͡�Ȩ�޵�DFIX���ݣ�������xxxx������ʵ��������ֱ�ӹ�ϵ��Ĭ��Ϊ0000DFIX.DAT
//0000DFIX.DATר���ڴ�������ģ�壬ÿ�����Ӻ����ȴ���ģ�壬Ȼ����ܴ�����
#define RAW_DFIX_DATA		"DFIX.DAT"

/////////////////////////////////////////////////////////////////////////////////////
//DCT_NEWS	������������
//�����Ա�����Ϊ��ʶ������溬Ŀ¼���ͱ��������Է�б�ָܷ��������ж༶Ŀ¼�����һ����б��֮���Ǳ���
//�������ŷ�Ϊ��ͨ���ź͸������ţ���ͨ������"INFO"��Ϊ��Ŀ¼������������"STOCK\"+�г�����+���ɴ�����Ϊ��Ŀ¼������"STOCK\SH\600000"
//��ͬ�г���Դ�����ţ������ڷ���˽����е�һ�������ţ����Բ����˲������ⴴ��Ŀ¼����ҪЭ�̰���һ���淶����
//�г����ţ�������"INFO\�г�����"��Ϊ��Ŀ¼����ʱ��Ϊ���������г�����Ŀ¼��������������Ŀ¼

struct DC_NEWS
{
	DWORD	m_dwSerialNo;			//��ţ������˴�0��ʼ��������������ʱ����Ը�����ŵõ���һ�����֮����ļ���Ҫ���´���
	WORD	m_wAttrib;
	WORD	m_wTitleLen;			//���ⳤ��
	DWORD	m_dwTextLen;			//���ĳ���
	char	m_cText[1];				//���⼰���ģ�����Ϊm_wTitleLen
};
//���ն�����յ������ţ����ڷ�����DC_READY֮���ٷ���һ���յ�DC_NEWS�������Ͷˣ��м�ֻ����m_dwSerialNo��������һ���յ����������
//���ն˳�ʼ��ʱ����Ҫ���յ�������ȫ��ɾ����ͬʱ���Ѿ����յ����ݵ����к���0

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//ͨ�Ź���(���������ӷ�����ģʽ)
//			��������														��������
//1����DC_PORT�˿ڣ��ȴ�����
//
//															2�����ӷ�����DC_PORT�����Ӻ���DC_LOGIN����
//
//3���յ�DC_LOGIN����¼�г����ԣ��Ժ��ʼ��ʱ����
//��Ҫ��ʼ������Ŀ⡣�����Ҫ��֤���룬����DC_REQPASS�����������5
//
//															4�����ݷ�����Ҫ�󣬽��û���������ܺ���DCT_USERnPASS��
//
//////////(���������Ӳ�����ģʽ)////////////////////////////////////////////////////////
//			��������														��������
//															1����DC_PORT�˿ڣ��ȴ�����(�˿ںſ�����)
//
//2�����Ӳ�����DC_PORT(������)�����Ӻ���DC_LOGIN����(�������е�m_wMarket��Ϣ)��
//
//															3�����֧�ָ��г�������DC_REQPASS�������ʹ���DISCONNECTָ���DC_READY�����������ΪNOT_SUPPORT_MARKET
//
//4�����յ�DC_REQPASS,����ݷ�����Ҫ�󣬽��û���������ܺ���DCT_USERnPASS����
//   �����յ�����DISCONNECTָ���DC_READY����Ͽ�����
//
//															4.1������DC_LOGIN����
//
//4.2���յ�DC_LOGIN����¼�г����ԣ��Ժ��ʼ��ʱ����
//     ��Ҫ��ʼ������Ŀ⡣
//
//															
//
////////////////////////////////////////////////////////////////////////////////////////
//5������DC_READY����
//		a,���벻�ԣ�����DISCONNECTָ�������INVALID_USERPASS���Ͽ�����
//		b,�Ѿ����ڸ��г��Ĳ����������ô�����MARKET_ALREADY_EXSIST
//			1,����Ҫ�ñ����������ã�����IDLEָ��;֮��ÿ30�뷢�ʹ�IDLEָ���
//				DC_READY��������һ������Դ���ϣ���Ҫ���ñ���������
//				���ʹ�SUCCESSָ���DC_READY���������������
//			2,������DISCONNECTָ��Ͽ�����
//		c,���������ڽ��г�ʼ��������WAITָ�ֱ����ʼ����ɺ󣬷���SUCCESSָ��
//		d,����֧�ָ��г�������DISCONNECTָ�������NOT_SUPPORT_MARKET���Ͽ�����
//		e,����������ʱ�����ã�����DISCONNECTָ�������SERVER_NOTREADY,�趨��������������ʱ�䣻�Ͽ�����
//		f,���򣬷���SUCCESSָ�׼����������
//															6, �յ�DCT_READY����ָ��Ϊ��
//																a,DISCONNECT����Ͽ�����
//																b,DISCONNECT,��������ΪSERVER_NOTREADY,��m_dwDelayConnectTime��Ϊ0�������ȶϿ��������ô������ٴ�����
//																c,WAIT����������շ��������͵����ݰ����ȴ���һ��ָ����ֲ�������������
//																d,IDLE����������շ��������͵����ݰ����ȴ���һ��ָ����ֲ���������DC_KEEPALIVE֪ͨ������������״̬
//																e,SUCCESS����½�ɹ����򱣴��Ƿ���Ҫѹ�������ܵ���Ϣ������6.1
//															
//															6.1,����յ���DCT_LOGIN���ݰ���m_dwAttrib������DCATTR_DIDDAT������DC_DSCRC��,��������֧�ֵ�did����ģ���crc�ļ��·�
//																����ֱ�ӽ���7.				
//
//6.2 �յ�DC_DSCRC��
//ѡȡ����֧�ֵ�DID,�Ƚ�crc,����Ҫ���·���ģ���did
//ͨ��DC_DSDID����
//															6.3,�յ�DC_DSDID,��¼������֧�ֵ�did,ͬʱͨ��DC_DSDATA������Ҫ���µ�didģ��
//
//															7���뱾�ؾ�̬����հ汾������ͬ���;�̬��DC_STKSTATIC �� ʹ��DC_GENERAL���� STK_STATICEx
//																
//
//															8������������̬�⡣����������������µľ�̬��ʱ��Ӧ��ʱ�����µ�������̬��
//9��ÿ���յ��µľ�̬�⣬��Ҫ���³�ʼ���������ݣ����ÿ����
//����¼��Ʊm_wID������ʼ�����������¼��������ڲ�һ�£�
//����Ҫ������ڵķֱʡ���ʱ��
//   �ڳ�ʼ���ڼ�������κη�������Ϊ�����˽��������ڼ䣬		�������յ�����WAITָ���DC_READY������ͣ��������������ݣ�ֱ���յ�SUCCESSָ��
//   ����������ʹ���WAITָ���DC_READY����
//		
//													......
//															10, ÿ���ɼ����µ�����仯������DCT_STKDYNA���ݰ������ݰ���С�����ʺϴ���ͬʱҲ��Ӱ����ʱΪ��
//11���յ�DCT_STKDYNA��̬���ݣ�
//		a,���±��������¼
//		b,�Թ�ע��Ʊ���б��ע��Ʊ���Ͷ�̬����
//		c,ѹ���仯�˵Ĺ�Ʊ��̬���飬�����������͵��ͻ����У��ɷ�������͵�һ�����Ա
//		d,���·ֱʳɽ���ͷ�ʱ��
//													......
//
//															12, level2�ɼ���̬������仯������DCT_STKDYNA��DCT_SHL2_MMPEx���ݰ�
//13���յ�DCT_SHL2_MMPEx���ݰ�
//		a,������չ�����̿���,�ȴ��û�����
//		b,�Թ�ע��Ʊ����
//
//													......
//														 	13, level2�ɼ����ֱʳɽ��仯������DCT_SHL2_REPORT���ݰ�
//14���յ�DCT_SHL2_REPORT
//		���Level2���ʳɽ���(���Ƿֱʳɽ���)��
//		�Թ�ע��Ʊ�������յ�������
//
//													......
//															15,level2�ɼ����󵥳����ر������� DC_SHL2_BIG_WD���ݰ�
//16���յ�DC_SHL2_BIG_WD��
//		a,���±��ش󵥳�����
//		b,�Թ�ע��Ʊ����
//		c,�����ڲ鿴�󵥻ر��Ŀͻ���ȫ������
//		d,�ֹ�Ʊ���´󵥳����Ᵽ����ʷ
//
//													......
//															17,level2�ɼ����ۼƴ󵥱仯������DC_SHL2_ACCU_WD���ݰ�
//18���յ�DC_SHL2_ACCU_WD��
//		a,���±����ۼƴ󵥿�
//		b,�����ڿ��󵥻ر��Ŀͻ���ȫ������
//
//													......
//															19,�۹ɲɼ����۹ɾ�̬���ݰ������±��أ���������DC_STKSTATIC,ʹ��DC_GENERAL���� STK_STATICEx
//20���յ�DC_GENERAL�۹���ؾ�̬����
//		a,���±��ظ۹ɾ�̬��
//		b,�����пͻ�����
//		c,����¼�Ŀͻ���Ҫ��������
//
//													......
//															21,�۹ɲɼ����۹ɶ�̬���ݰ�������DC_STKDYNA��DC_GENERAL(����STK_HKDYNA)
//22���յ�DC_GENERAL(STK_HKDYNA)�۹ɶ�̬
//		a,���±��ظ۹ɶ�̬��
//		b,�Թ�ע��Ʊ����
//
//													......
//															23,�������������������Ƚ�С���������ʵ����ݣ����Բ���DC_XML����
//24���յ�DC_XML���ݰ�������Ҫ�������Ҫ
//		a,�������ݰ����Լ������ַ�
//		b,ֱ�ӷַ����ݰ����ͻ�
//
//													......
//															24.1,�ɼ����õ�did���ݣ����ݼ�¼������֧�ֵ�did����DC_DID����
//24.2���յ�DC_DID���ݰ�������Ҫ�������Ҫ
//		a,�������ݰ����Լ������ַ�
//		b,ֱ�ӷַ����ݰ����ͻ�
//
//													......
//
//25���������������ϣ��������Ӻ��������������ط����ݰ�
//		����DC_RESEND���ݰ�
//															26������¼�����ݰ����·��ͣ�ע�⣬�������������ط��ڼ��ٴη����ط��������ۼ���������
//																���������ȡ���ط����룬��ֹͣ���е��ط���̡���##��
//
//													......
//27,ÿ30�뷢��DC_KEEPALIVE֪ͨ���������ݰ��������
//															28������������ݰ������д����ӳ٣�����DC_DELAY�������
//29���յ�DC_DELAY���������������ѡ�������������Դ������������
//
//														ע��															
//																1��������˳����̵��췢����ÿһ�����ݰ������Ҽ�¼����
//
//30��ÿ15�룬�����������������DC_USERSTAT�ͻ�ͳ������
//
//															31���������յ�DCT_USERSTAT��ʹ��ip-port.sta�ļ��������������̣�������ServDataInfo����������һ��������ת��������
//
//
//
//
//
//
//
//////////(RAW����Դ���Ӳ�����ģʽ)////////////////////////////////////////////////////////
//			RAW����Դ														��������
//															1����DC_PORT�˿ڣ��ȴ�����(�˿ںſ�����)
//
//2�����Ӳ�����DC_PORT(������)�����Ӻ���DC_LOGIN����(�������е�m_wMarket��־��m_strMarketName����Ϊ"RawDataServer", m_dwAttrib����DCATTR_RAWSVR��־)��
//
//															3�����֧�ָ��г�������DC_REQPASS�������ʹ���DISCONNECTָ���DC_READY�����������ΪNOT_SUPPORT_MARKET
//
//4�����յ�DC_REQPASS,����ݷ�����Ҫ�󣬽��û���������ܺ���DCT_USERnPASS����
//   �����յ�����DISCONNECTָ���DC_READY����Ͽ�����
//
//															5,����DC_READY����
//																	a,���벻�ԣ�����DISCONNECTָ�������INVALID_USERPASS���Ͽ�����
//																	b,���򣬷���SUCCESSָ�׼����������
//6���յ�DC_READY����ֻҪ���µ�RAW���ݾͷ���DC_RAWDATA����
//
//															7���յ�DC_RAWDATA,�������RAW_BROADCAST��־���������ݰ��·������������ķ�����/��ת������ʱҪȥ��RAW_BROADCAST��־��
//															8,ÿ30�뷢��DC_KEEPALIVE֪ͨRAW����Դ
//															
//
//
//
////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//��##�������ݲ�֧��
#pragma pack(pop)

#endif //_DATACOLLECT_H_INCLUDE
