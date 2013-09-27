#ifndef __DS_DAT_H
#define __DS_DAT_H

#include "stdafx.h"
#include "mword.h"
#include "datacollect.h"

#include "dzh_time_t.h"
#include "pfloat.h"
#include "vfloat.h"


#define  STKID "id" //�ؼ���
const int MAX_FIELD_NAME = 32;//�ֶ�������󳤶�
const int MAX_FIELDS_NUM = 1024;//ÿ��did����ÿ��ÿ���ֶ������Ŀ
const int MAX_TYPE_NAME = 16;//��������������󳤶�
const int MAX_ORULES_LEN = 32;
const int MAXSEQCODELEN = 32;//��Ʊ������󳤶�

const int SHL2_EXMMPBS_DAT = 100000;
const int SHL2_EXZB_DAT = 100001;
enum
{
	NO_HDIDS_ELM_ERR=-49,///hdids��������
	NO_HDIDS_ATTR_ERR=-48,//��hdids��
	WEEKDAY_ERR=-47,
	GMTMINTIME_ERR=-46,
	TIMELEN_ERR=-45,
	TABLES_NUM_ERR=-44,
	NO_CBID_ERR=-43,
	CBID_ERR=-42,
	NO_CIDFIELD_ERR=-41,
	NO_CIDINFO_ERR=-40,
	FULLPUSH_VAL_ERR=-39,
	NO_FULLPUSH_ATTR_ERR=-38,
	NO_ARGS_ELM_ERR=-37,
	NO_RFIELDS_ELM_ERR=-36,
	NO_CID_ELM_ERR=-35,
	DRVVER_VAL_ERR=-34,
	NO_DRVVER_ATTR_ERR=-33,
	NO_STR_ERROR=-32,
	NO_STRID_ERR=-31,
	NO_CSTRS_ELM_ERR=-30,
	NO_DB_ERROR=-29,
	NO_CD_ERROR=-28,
	NO_CODE_ERROR=-27,
	NO_DL_ERROR=-26,
	NO_CL_ERROR=-25,
	NO_CB_ERROR=-24,
	NO_CBITS_ELM_ERR=-23,
	ID_ERROR=-22,
	NO_NAME_ERR=-21,
	NO_CID_ERR=-20,
	NO_CIDS_ELM_ERR=-19,//cids��������
	NO_FIELDS_ELM_ERR=-18,//fields��������
	NO_FIELDS_ATTR_ERR=-17,//��fields��
	NO_DIDS_ELM_ERR=-16,///dids��������
	NO_DIDS_ATTR_ERR=-15,//��dids��
	CLASS_NODATA_ERR=-14,//�ö���û��ʼ����
	KEY_POS_ERR=-13,//stkid�ؼ��ֲ��ڼ�¼ͷһ��λ��
	KEY_TYPE_ERR=-12,//stkid�ؼ����������Ͳ�Ϊunsigned int
	NO_KEY_FIELD=-11,//û�йؼ��ֶ�
	MAXMIN_ERR=-10,//maxsize����minsize��������
	MALLOC_ERR=-9,//�����ڴ�ʧ��
	DID_VAL_ERR=-8,//didֵ�Ƿ�
	NO_DID_ATTR_ERR=-7,//��did�ֶ�
	NO_DATA_ELM_ERR=-6,//data��������
	NO_DATA_NODE_ERR=-5,//��data��
	NO_DATAS_ELM_ERR=-4,//datas��������
	NO_DATAS_NODE_ERR=-3,//��datas��
	PARSE_ERR=-2,//xml�ļ�����ʧ��
	ARGS_ERR=-1,//�����Ƿ�
};

enum FIELD_TYPE
{
	drepeat = 0,
	dchar = 1,
	dint16 = 2,
	dint32 = 3,
	dint64 = 4,
	dbyte = 5,
	duint16 = 6,
	duint24 = 7,
	duint32 = 8,
	duint64 = 9,
	dpfloat = 10,
	ddtime = 11,
	dvfloat = 12,
};

class JUncopyable
{
protected:
	JUncopyable(){};
	~JUncopyable(){};
private:
	JUncopyable(const JUncopyable&);
	JUncopyable& operator = (const JUncopyable&);
};

#pragma pack(1)

//�Ϻ�L2��չ���
struct ExSH_L2_REPORT
{
	DWORD m_dwStkID;			
	dzh_time_t m_dtTime;//�ɽ�ʱ��
	PFloat m_pfPrice;//�ɽ��۸�
	DWORD m_dwVolume;//�ɽ���
	BYTE m_byteInOutFlag;
	UINT64 m_uint64BuyOrderId;
	UINT64 m_uint64SellOrderId;
};

//�Ϻ�L2��չ���
struct ExSH_L2_REPORTV2
{
	DWORD m_dwStkID;			
	dzh_time_t m_dtTime;//�ɽ�ʱ��
	PFloat m_pfPrice;//�ɽ��۸�
	VFloat m_vfVolume;//�ɽ���
	BYTE m_byteInOutFlag;
	UINT64 m_uint64BuyOrderId;
	UINT64 m_uint64SellOrderId;
};

struct MmpBs
{
	DWORD m_dwStkID;
	dzh_time_t m_dtUpdateTime;

	DWORD m_dwBuyPriceNum;
	DWORD m_dwBuyWithdrawCount;
	VFloat m_vfBuyWithdrawVol;
	VFloat m_vfBuyWithdrawAmount;

	DWORD m_dwSellPriceNum;
	DWORD m_dwSellWithdrawCount;
	VFloat m_vfSellWithdrawVol;
	VFloat m_vfSellWithdrawAmount;

	DWORD m_dwBuyCount[10];
	DWORD m_dwSellCount[10];
};

struct MemOffsetLen
{
	UINT64 m_uint64Offset;//ƫ��
	UINT64 m_uint64Len;//����
};

struct SinTableLastAddress
{
	DWORD m_dwDid;
	DWORD m_dwSeq;
	UINT64 m_uint64Address;//��ַ
};

struct ds_type
{
	char m_cTypeLabel[MAX_TYPE_NAME];
	unsigned char m_byteTypeSize;
};

struct DidName
{
	char m_cName[MAX_FIELD_NAME+1];//DID����
	DWORD m_dwDid;
};

struct Dids
{
	WORD m_wDidsNum;
	DidName mDidName[1];
};

struct DidXmlInfo
{
	DWORD m_dwDid;
	DWORD m_dwCrc;
	DWORD m_dwLen;
	UINT64 m_qwOffset;
};
struct DidXmlHead
{
	WORD m_wDidsNum;
};

struct CidFieldInfo
{
	DWORD m_dwFid;//id
	BYTE m_cBmType;//bmt
	BYTE m_cBlockCmp;//һ��Ϊ0�����Ϊ1����Ҫ�Ժ�����ֶ���0���Ƚϱ���
	UINT64 m_uint64BmaVal;//����ȡֵ:�ֶ�id,�ַ����б�id,ʵ����ֵ,0xFFFFFFFFFFFFFFFF��ʾ����̬��������bmav
	UINT64 m_uint64BmbVal;//����ȡֵ:�ֶ�id m_vBmTypeΪEQUAL��Чbmbv
	char m_cOperRules[MAX_ORULES_LEN];//x2-x1,bmor
	DWORD m_dwCbId;//CodeBits ID,cbid
	BYTE m_cOperType;//m_cOperRules
};

struct CidField
{
	enum BMT_TYPE//�������
	{
		VAL = 0,				//������bmav�ľ���ȡֵ���,BmaVĬ��ֵΪ0
		LAST = 1,				//����ͬ�ؼ��ֵ�ǰһ����¼bmavֵID�ֶ����
		PREV = 2,				//��һ�����ݰ���ǰ���һ����¼bmavֵID�ֶ����
		NOW = 3,				//������¼bmavֵID�ֶ����
		STR = 4,				//����bmavֵ���ַ����б���ƥ��
		EQUAL = 5,				//������bmbvȡֵ�ֶ���ȵ�ǰһ����¼���
		STATICDAY = 6,			//����̬��������������
	};
	CidFieldInfo mCFInfo[3];//0Ϊһ���ֶ�,1ΪpfloatС�����ֵ�ѹ������dtime���������ֵ�ѹ��,2Ϊdtime΢�������ֵ�ѹ��
};

struct CidInfo
{
	char m_cName[MAX_FIELD_NAME+1];//CID����
	DWORD m_dwCid;//CID
	DWORD m_dwDCid;//���Ҳ����Ƚϵļ�¼���õ�ѹ����ʽ
	DWORD m_dwFiledsNum;//�ֶ���Ŀ
	UINT64 m_uint64Offset;//ƫ������ָ�����ÿ���ֶε�ѹ����ط�ʽ��Ϣ
};
struct Cids
{
	DWORD m_wCidsNum;//�ж�����ѹ����ط�ʽ,һ����prev,last,now
	CidInfo  mCidInfo[1];//��ط�ʽ��Ϣ
};

struct CodeBitInfo
{
	char m_cName[MAX_FIELD_NAME+1];//CodeBit����
	DWORD m_dwCid;//CID
	DWORD m_dwNum;//������Ŀ
	UINT64 m_uint64Offset;//ƫ������ָ����������Ϣ
};

struct CodeBitIds
{
	DWORD m_wCodeBitIdNum;//�ж�����codebitid
	CodeBitInfo  mCodeBitInfo[1];//��ط�ʽ��Ϣ
};

struct StrList
{
	char m_cName[MAX_FIELD_NAME+1];//strid����
	DWORD m_dwStrId;//strid
	DWORD m_wStrNum;//�ж��ٸ��ַ���
	DWORD m_wMaxStrLen;//����ַ�������
	UINT64 m_uint64Offset;//ƫ������ָ�������ַ����б�
};

struct StrIds
{
	DWORD m_wStrIdNum;//�ж��ٸ�strid
	StrList  mStrList[1];//��ط�ʽ��Ϣ
};

//BitCodeField::m_cCode

//'B':���ݲ�����m_dwCodeDataλ, 
//'b':���ݲ�����m_dwCodeDataλ, �������������Դ���ʱ��һλ���ڴ�������λ
//'m':���ݲ�����m_dwCodeDataλ, ����
//'S':��������HIWORD(m_dwCodeData)�󲻳���LOWORD(m_dwCodeData)λ
//'E':���ݵ���CodeData
//'Z':����Ϊx*10^N, x<CodeData,N==1-4
//'P':��m_qwCodeDataλ�����У�����һ��1�����1��λ��
//'M':�������������ֶ�ͳһ�Ƚ�
//'Q':ԭʼ64λ��������,ԭʼ���ݷ���Bit��,���Ƿ�������棬������ƥ��
//'s':�ַ���
struct BitCodeField
{
	WORD	m_wCodeBits;	//����
	char	m_nCodeLen;		//���볤��
	char	m_nDataLen;		//������������ݳ���,��λBit�����64Bit
	char	m_cCode;		//���뷽��	  
	UINT64	m_uint64CodeData;	//��������
	UINT64	m_uint64DataBias;	//��������ƫ����
#ifdef _DEBUG
	DWORD	m_dwCodeCount;	//Ƶ��
#endif
	BOOL IsOriginalData() const {return  (m_cCode=='Q');}
	BOOL IsBitPos() const {return m_cCode=='P';}
};

struct ds_did_info
{
	unsigned int did;//did
	unsigned char index_key;//Ϊ1��ʾ,���Ը��ݹؼ��ֶ�ֱ�Ӷ�λ
	char m_cName[MAX_FIELD_NAME+1];//DID����
	short m_sKeyFieldPos;//�ؼ��ֶ�λ��,ÿ��Ŀǰֻ֧��һ���ؼ��ֶ�
	unsigned short m_wSortSecs;//��������һ����
	unsigned short m_wFieldsNum;//�ֶ���Ŀ
	unsigned short m_wAllFieldsNum;//���ֶ���Ŀ
	unsigned short m_wTablesNum;//�ṹ����Ŀ������Ŀ
	unsigned int m_dwRRecSize;//һ����¼���ȣ�ÿ��R�����ֶ��ټ���һ��ƫ�Ƶ�ַ����8�ֽ�
	unsigned int m_dwVRecSize;//һ����¼����
	unsigned int m_dwAllVRecSize;//һ����¼����
	unsigned char m_byteMaxBits;//������󳤶�
	unsigned char m_byteEqual;//�ü��ֶγ��ȣ����ͣ�λ����ȫһģһ��Ϊ1
	unsigned int drvver;//�����汾��
	unsigned char m_byteFullPush:1;//ȫ�ƣ�Ϊ1��ʾ��Ҫȫ��
	unsigned char m_byteDatType:7;//0��Ƭ���ݣ�1��������
	dzh_time_t m_dtGmtMinTimes;//��С��ʼʱ��
	UINT64 m_uint64TimeLen;//ʱ���
};
struct ds_field_info
{
	unsigned char m_byteTypeIndex;//������������
	unsigned char m_byteTypeSize;//�������ʹ�С
	unsigned char m_byteSortFlag;
	unsigned char m_byteSaveFlag;//��ʾ�ͻ��˸ü��Ƿ��������
	unsigned short m_wId;
	unsigned short m_wASize;//�����С
	unsigned int m_dwOffset;//���ֶξ����¼ͷ�ĳ���
	unsigned int m_dwROffset;//���ֶξ����¼ͷ�ĳ���,����R�����ֶ�ƫ�Ƶ�ַ
	char m_cName[MAX_FIELD_NAME+1];//�ֶ�����
	int idpos;//��Ӧ����ģ����ֶ�λ��

	UINT64 m_uint64Offset;//ƫ������ָ��Ƕ�׵��ֶ���Ϣ
	unsigned short m_wFieldsNum;//Ƕ�׵��ֶ���Ŀ,��ֵΪ0��ʾûǶ��
	unsigned int m_dwRRecSize;//һ����¼���ȣ�ÿ��R�����ֶ��ټ���һ��ƫ�Ƶ�ַ����8�ֽ�
	unsigned int m_dwVRecSize;//һ����¼����
	short m_sKeyFieldPos;//�ؼ��ֶ�λ��
	unsigned char m_byteEqual;//�ü��ֶγ��ȣ����ͣ�λ����ȫһģһ��Ϊ1
	unsigned char index_key;//Ϊ1��ʾ,���Ը��ݹؼ��ֶ�ֱ�Ӷ�λ
	unsigned short tableid;//�ڼ���table��
};

struct timemyval
{
	UINT64 m_uint64TimeSec;//������ʱ����
	UINT64 m_uint64TimeUSec;//������ʱ��΢��
};

struct stock_head
{
	timemyval m_tmTime;
	UINT64 m_uint64Ver;//�汾��
};
struct first_head
{
	unsigned int m_dwRecsNum;//Ŀǰ���м�¼�����ؼ���Ϊstkid,stkid���ֵ������������м�¼��Ŀ
	UINT64 m_uint64FullCompressCurPos;//ȫ������ǰ���ݴ���λ��+1
	UINT64 m_uint64FullCompressDatVer;//ȫ������ǰ���ݰ汾��
	stock_head mStockHead;//�ܱ��������
};

struct table_head
{
	//short m_sTableFieldsOffset;//ָ����ֶ���Ϣ,Ϊ-1��ʾ���
	UINT64 m_uint64TableHeadOffset;//ָ��first_head
	UINT64 m_uint64TableDataOffset;//ָ����¼������Ϣ
	unsigned int m_uintTableRecsMaxNum;//���¼�����Ŀ
	unsigned int m_uintTableBlockRecsNum;//��ֿ��¼��Ŀ
	unsigned int m_uintTableBlockSize;//��ֿ��С,��λbytes=8+m_uintTableBlockRecsNum*RecSize
	UINT64 m_uint64TableCurOffset;//ָ��Ŀǰ�ѷ����λ��,��λbytes
};

struct repeat_head
{
	unsigned int num;//������
	UINT64 m_uint64OffSet;//ƫ��λ��
};

struct CircularBufferArgs
{
	UINT64 m_qwBufferSize;//�������ܳ���
	DWORD m_dwPosNum;//����λ����Ŀ
};

struct LastArgs
{
	ds_field_info* pRevF;//ǰһ�������ü���¼��Ϣ���ֶΣ����ΪӦ��ȡpDidStruct->mDidInfo
	ds_field_info* pF;//�ò㼶�ֶ���Ϣ��ʼλ��
	char* pRec;//��¼�ò㼶��ʼ��ַ
	int index;//�ü���Ҫ�Ƚϵ��ֶ�����ֵ,Ĭ��ȡֵ-1
};

struct UserArgs
{
	int logfd;
	int arg;
	DWORD seqid;
};

struct WeekDayTime
{
	char m_cWeekDay;
	WORD m_wStartTime;
	WORD m_wEndTime;
};

struct WeekTime
{
	BYTE m_byteNum;
	WeekDayTime mWeekDayTime[7];
};

struct SeqBufArgs
{
	DWORD m_dwDiskSavePartsNum;//�����̵���������
	/*DWORD m_dwMemSavePartsNum;*/
	DWORD m_dwMemSaveRevPartsNum;//Ԥ���������������ڴ棬һ������ָ��һ���̶�ʱ��ε����ݣ��������߿��԰�90�컮Ϊһ���̶�ʱ��Σ�һ��ѹ��
	DWORD m_dwBlkSize;//ÿ���С
	UINT64 m_uint64BufSize;//�������ڴ��С
};
struct SeqNumArgs
{
	DWORD m_dwMaxCodeNum;//����Ʊ��Ŀ
	DWORD m_dwMemMaxIndexNum;//�ڴ��ﱣ������������������Ԥ������

	DWORD m_dwLastMaxNum;
	DWORD m_dwLastPartSize;
	DWORD m_dwLastBufSize;

	DWORD m_dwRcntPartNum;
	DWORD m_dwRcntPartSzie;
	DWORD m_dwRcntBufSize;
};

struct SeqFilter
{
	enum UPDATEDATATYPE
	{
		append=1,
		overwrite=2,
	};
	char m_cDatTypeName[MAX_FIELD_NAME+1];
	char m_cKeyFieldName[MAX_FIELD_NAME+1];
	char m_cFunName[MAX_FIELD_NAME+1];
	int m_nUpdateDatType;//���ݸ��·�ʽ
	int m_nMinTimeMins;//������Сʱ�������������Ϊ����
	int m_nMaxTimeMins;//�������ʱ�������
};

struct SeqArgs
{
	WeekTime mLoadTime;
	WeekTime mCloseTime;
	SeqNumArgs mSeqNumArgs;
	SeqBufArgs mSeqBufArgs;
	SeqFilter mSeqFilter;
};

//idx�ļ�ͷ
struct SeqIdxHead
{
	DWORD m_dwNum;//��Ŀ
	DWORD m_dwCrc;//ģ��crc
};
//dat�ļ�ͷ
struct SeqDatHead
{
	DWORD m_dwDid;//did
	DWORD m_dwCrc;//ģ��crc
	DWORD m_dwNum;//��¼��
	DWORD m_dwBlkNum;//Ŀǰ���ݿ���
};
//+���һ����¼����
//+ѹ��������

struct SeqFixHead
{
	WORD m_wDidNum;
	UINT64 m_uint64BufLen;
};

//m_wDidNum��
struct SeqDidFixHead
{
	DWORD m_dwDid;//����did
	DWORD m_dwXmlCrc;//didģ��crc
	SeqNumArgs mSeqNumArgs;//��Ŀ����
	SeqBufArgs mSeqBufArgs;//�ڴ����
	MemOffsetLen m_moSeqMem;
};

//ÿ��did��seq��
struct SeqMemHead
{
	DWORD m_dwChangeStkNum;//��Ч��Ʊ��Ŀ
	UINT64 m_uint64OffSet;//ָ����Ŀ����õ��ڴ�ƫ��
	MemOffsetLen m_moIdx;//idx�����ڴ棬����ڻ���ַ,m_pBase
	MemOffsetLen m_moLast;//���һ�α仯�ڴ�,����ڻ���ַ,m_pBase
	MemOffsetLen m_moRcnt;//���n�ʱ仯�ڴ�,����ڻ���ַ,m_pBase
	MemOffsetLen m_moDat;//ȫ�������ڴ�,����ڻ���ַ,m_pBase
	BYTE m_byteReadFalg;//Ϊ0�ɶ���Ϊ1���ɶ�������ڴ����
};
//m_dwChangeMaxNum��*2
struct SeqCodeIndex
{
	char code[MAXSEQCODELEN];
	DWORD m_dwIndex;
	int m_nTodayIndex;//ÿ���������Ҫ���¸�ֵ������
};
//m_dwChangeMaxNum��
//{
	struct SeqMemIdxHead
	{
		DWORD m_dwNum;//ʵ��������Ŀ
		UINT64 m_uint64NewId;//����id
	};
	
	//���һ����¼����

	//m_dwMemMaxIndexNum��
	struct SeqIdx
	{
		dzh_time_t m_dtStartTime;//������ʼʱ��
		dzh_time_t m_dtEndTime;//���ݽ���ʱ��
		DWORD m_dwId;//λ��,��1��ʼ,��������������˵
		DWORD m_dwNum;//�ÿ鱣���������Ŀ
		DWORD m_dwCrc;//�ÿ����ݵ�crc
		UINT64 m_uint64Offset;//�ÿ����ݵ��ڴ�ƫ�Ƶ�ַ,��m_pJSegBase����
		DWORD m_dwDatBitSize;//�ÿ�����ѹ�����λ����
	};
	struct MSeqIdx
	{
		SeqIdx mSeqIdx;
		UINT64 m_uint64Segment;//��һ��segment,��m_pJSegBase����
		DWORD m_dwPutBitStartPosByte;//�����������������һ��blk�￪ʼ��ʼλ��,������һ��blkֻ������һ�������飬Ϊ0
		DWORD m_dwPutBitSize;//�����������������һ��blk����ʹ�õ�λ��
	};
//}
#pragma pack()

#endif
