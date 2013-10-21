#ifndef __DS_DAT_H
#define __DS_DAT_H

#include "stdafx.h"
#include "mword.h"
#include "datacollect.h"

#include "dzh_time_t.h"
#include "pfloat.h"
#include "vfloat.h"


#define  STKID "id" //关键字
const int MAX_FIELD_NAME = 32;//字段名称最大长度
const int MAX_FIELDS_NUM = 1024;//每个did个里每级每组字段最大数目
const int MAX_TYPE_NAME = 16;//数据类型名称最大长度
const int MAX_ORULES_LEN = 32;
const int MAXSEQCODELEN = 32;//股票代码最大长度

const int SHL2_EXMMPBS_DAT = 100000;
const int SHL2_EXZB_DAT = 100001;
enum
{
	NO_HDIDS_ELM_ERR=-49,///hdids下无数据
	NO_HDIDS_ATTR_ERR=-48,//无hdids节
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
	NO_CIDS_ELM_ERR=-19,//cids下无数据
	NO_FIELDS_ELM_ERR=-18,//fields下无数据
	NO_FIELDS_ATTR_ERR=-17,//无fields节
	NO_DIDS_ELM_ERR=-16,///dids下无数据
	NO_DIDS_ATTR_ERR=-15,//无dids节
	CLASS_NODATA_ERR=-14,//该对象还没初始化过
	KEY_POS_ERR=-13,//stkid关键字不在记录头一个位置
	KEY_TYPE_ERR=-12,//stkid关键字数据类型不为unsigned int
	NO_KEY_FIELD=-11,//没有关键字段
	MAXMIN_ERR=-10,//maxsize不是minsize的整数倍
	MALLOC_ERR=-9,//分配内存失败
	DID_VAL_ERR=-8,//did值非法
	NO_DID_ATTR_ERR=-7,//无did字段
	NO_DATA_ELM_ERR=-6,//data下无数据
	NO_DATA_NODE_ERR=-5,//无data节
	NO_DATAS_ELM_ERR=-4,//datas下无数据
	NO_DATAS_NODE_ERR=-3,//无datas节
	PARSE_ERR=-2,//xml文件解析失败
	ARGS_ERR=-1,//参数非法
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

//上海L2扩展逐笔
struct ExSH_L2_REPORT
{
	DWORD m_dwStkID;			
	dzh_time_t m_dtTime;//成交时间
	PFloat m_pfPrice;//成交价格
	DWORD m_dwVolume;//成交量
	BYTE m_byteInOutFlag;
	UINT64 m_uint64BuyOrderId;
	UINT64 m_uint64SellOrderId;
};

//上海L2扩展逐笔
struct ExSH_L2_REPORTV2
{
	DWORD m_dwStkID;			
	dzh_time_t m_dtTime;//成交时间
	PFloat m_pfPrice;//成交价格
	VFloat m_vfVolume;//成交量
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
	UINT64 m_uint64Offset;//偏移
	UINT64 m_uint64Len;//长度
};

struct SinTableLastAddress
{
	DWORD m_dwDid;
	DWORD m_dwSeq;
	UINT64 m_uint64Address;//地址
};

struct ds_type
{
	char m_cTypeLabel[MAX_TYPE_NAME];
	unsigned char m_byteTypeSize;
};

struct DidName
{
	char m_cName[MAX_FIELD_NAME+1];//DID名称
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
	BYTE m_cBlockCmp;//一般为0，如果为1，需要对后面的字段与0做比较编码
	UINT64 m_uint64BmaVal;//可能取值:字段id,字符串列表id,实际数值,0xFFFFFFFFFFFFFFFF表示跟静态码表的日期bmav
	UINT64 m_uint64BmbVal;//可能取值:字段id m_vBmType为EQUAL有效bmbv
	char m_cOperRules[MAX_ORULES_LEN];//x2-x1,bmor
	DWORD m_dwCbId;//CodeBits ID,cbid
	BYTE m_cOperType;//m_cOperRules
};

struct CidField
{
	enum BMT_TYPE//相关类型
	{
		VAL = 0,				//跟后面bmav的具体取值相关,BmaV默认值为0
		LAST = 1,				//跟相同关键字的前一条记录bmav值ID字段相关
		PREV = 2,				//跟一个数据包里前面的一条记录bmav值ID字段相关
		NOW = 3,				//跟本记录bmav值ID字段相关
		STR = 4,				//后面bmav值的字符串列表里匹配
		EQUAL = 5,				//跟后面bmbv取值字段相等的前一条记录相关
		STATICDAY = 6,			//跟静态数据里的日期相关
	};
	CidFieldInfo mCFInfo[3];//0为一般字段,1为pfloat小数部分的压缩或者dtime毫秒数部分的压缩,2为dtime微秒数部分的压缩
};

struct CidInfo
{
	char m_cName[MAX_FIELD_NAME+1];//CID名称
	DWORD m_dwCid;//CID
	DWORD m_dwDCid;//当找不到比较的记录采用的压缩方式
	DWORD m_dwFiledsNum;//字段数目
	UINT64 m_uint64Offset;//偏移量，指向具体每个字段的压缩相关方式信息
};
struct Cids
{
	DWORD m_wCidsNum;//有多少种压缩相关方式,一般有prev,last,now
	CidInfo  mCidInfo[1];//相关方式信息
};

struct CodeBitInfo
{
	char m_cName[MAX_FIELD_NAME+1];//CodeBit名称
	DWORD m_dwCid;//CID
	DWORD m_dwNum;//编码数目
	UINT64 m_uint64Offset;//偏移量，指向具体编码信息
};

struct CodeBitIds
{
	DWORD m_wCodeBitIdNum;//有多少种codebitid
	CodeBitInfo  mCodeBitInfo[1];//相关方式信息
};

struct StrList
{
	char m_cName[MAX_FIELD_NAME+1];//strid名称
	DWORD m_dwStrId;//strid
	DWORD m_wStrNum;//有多少个字符串
	DWORD m_wMaxStrLen;//最大字符串长度
	UINT64 m_uint64Offset;//偏移量，指向具体的字符串列表
};

struct StrIds
{
	DWORD m_wStrIdNum;//有多少个strid
	StrList  mStrList[1];//相关方式信息
};

//BitCodeField::m_cCode

//'B':数据不超过m_dwCodeData位, 
//'b':数据不超过m_dwCodeData位, 带符号数，所以存贮时有一位用于存贮符号位
//'m':数据不超过m_dwCodeData位, 负数
//'S':数据右移HIWORD(m_dwCodeData)后不超过LOWORD(m_dwCodeData)位
//'E':数据等于CodeData
//'Z':数据为x*10^N, x<CodeData,N==1-4
//'P':在m_qwCodeData位数据中，存在一个1，这个1的位置
//'M':编码包括其后多个字段统一比较
//'Q':原始64位整形数据,原始数据放入Bit流,总是放在最后面，不进行匹配
//'s':字符串
struct BitCodeField
{
	WORD	m_wCodeBits;	//编码
	char	m_nCodeLen;		//编码长度
	char	m_nDataLen;		//编码后跟随的数据长度,单位Bit，最大64Bit
	char	m_cCode;		//编码方法	  
	UINT64	m_uint64CodeData;	//编码数据
	UINT64	m_uint64DataBias;	//编码数据偏移量
#ifdef _DEBUG
	DWORD	m_dwCodeCount;	//频度
#endif
	BOOL IsOriginalData() const {return  (m_cCode=='Q');}
	BOOL IsBitPos() const {return m_cCode=='P';}
};

struct ds_did_info
{
	unsigned int did;//did
	unsigned char index_key;//为1表示,可以根据关键字段直接定位
	char m_cName[MAX_FIELD_NAME+1];//DID名称
	short m_sKeyFieldPos;//关键字段位置,每级目前只支持一个关键字段
	unsigned short m_wSortSecs;//多少秒排一次序
	unsigned short m_wFieldsNum;//字段数目
	unsigned short m_wAllFieldsNum;//总字段数目
	unsigned short m_wTablesNum;//结构体数目，表数目
	unsigned int m_dwRRecSize;//一条记录长度，每个R类型字段再加上一个偏移地址长度8字节
	unsigned int m_dwVRecSize;//一条记录长度
	unsigned int m_dwAllVRecSize;//一条记录长度
	unsigned char m_byteMaxBits;//编码最大长度
	unsigned char m_byteEqual;//该级字段长度，类型，位置完全一模一样为1
	unsigned int drvver;//驱动版本号
	unsigned char m_byteFullPush:1;//全推，为1表示需要全推
	unsigned char m_byteDatType:7;//0切片数据，1序列数据
	dzh_time_t m_dtGmtMinTimes;//最小起始时间
	UINT64 m_uint64TimeLen;//时间段
};
struct ds_field_info
{
	unsigned char m_byteTypeIndex;//数据类型索引
	unsigned char m_byteTypeSize;//数据类型大小
	unsigned char m_byteSortFlag;
	unsigned char m_byteSaveFlag;//表示客户端该级是否需求存贮
	unsigned short m_wId;
	unsigned short m_wASize;//数组大小
	unsigned int m_dwOffset;//该字段距离记录头的长度
	unsigned int m_dwROffset;//该字段距离记录头的长度,包括R类型字段偏移地址
	char m_cName[MAX_FIELD_NAME+1];//字段名称
	int idpos;//对应存贮模板的字段位置

	UINT64 m_uint64Offset;//偏移量，指向嵌套的字段信息
	unsigned short m_wFieldsNum;//嵌套的字段数目,该值为0表示没嵌套
	unsigned int m_dwRRecSize;//一条记录长度，每个R类型字段再加上一个偏移地址长度8字节
	unsigned int m_dwVRecSize;//一条记录长度
	short m_sKeyFieldPos;//关键字段位置
	unsigned char m_byteEqual;//该级字段长度，类型，位置完全一模一样为1
	unsigned char index_key;//为1表示,可以根据关键字段直接定位
	unsigned short tableid;//第几个table的
};

struct timemyval
{
	UINT64 m_uint64TimeSec;//最后更新时间秒
	UINT64 m_uint64TimeUSec;//最后更新时间微秒
};

struct stock_head
{
	timemyval m_tmTime;
	UINT64 m_uint64Ver;//版本号
};
struct first_head
{
	unsigned int m_dwRecsNum;//目前已有记录数，关键字为stkid,stkid最大值，否则就是已有记录数目
	UINT64 m_uint64FullCompressCurPos;//全量包当前数据存贮位置+1
	UINT64 m_uint64FullCompressDatVer;//全量包当前数据版本号
	stock_head mStockHead;//总表更新数据
};

struct table_head
{
	//short m_sTableFieldsOffset;//指向表字段信息,为-1表示起点
	UINT64 m_uint64TableHeadOffset;//指向first_head
	UINT64 m_uint64TableDataOffset;//指向表记录数据信息
	unsigned int m_uintTableRecsMaxNum;//表记录最大数目
	unsigned int m_uintTableBlockRecsNum;//表分块记录数目
	unsigned int m_uintTableBlockSize;//表分块大小,单位bytes=8+m_uintTableBlockRecsNum*RecSize
	UINT64 m_uint64TableCurOffset;//指向目前已分配的位置,单位bytes
};

struct repeat_head
{
	unsigned int num;//数据量
	UINT64 m_uint64OffSet;//偏移位置
};

struct CircularBufferArgs
{
	UINT64 m_qwBufferSize;//存贮区总长度
	DWORD m_dwPosNum;//存贮位置数目
};

struct LastArgs
{
	ds_field_info* pRevF;//前一级表述该级记录信息的字段，如果为应该取pDidStruct->mDidInfo
	ds_field_info* pF;//该层级字段信息起始位置
	char* pRec;//记录该层级起始地址
	int index;//该级需要比较的字段索引值,默认取值-1
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
	DWORD m_dwDiskSavePartsNum;//最大存盘的索引条数
	/*DWORD m_dwMemSavePartsNum;*/
	DWORD m_dwMemSaveRevPartsNum;//预留多少条索引的内存，一条索引指向一个固定时间段的数据，比如日线可以把90天划为一个固定时间段，一起压缩
	DWORD m_dwBlkSize;//每块大小
	UINT64 m_uint64BufSize;//数据总内存大小
};
struct SeqNumArgs
{
	DWORD m_dwMaxCodeNum;//最大股票数目
	DWORD m_dwMemMaxIndexNum;//内存里保存的最大索引条数包括预留部分

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
	int m_nUpdateDatType;//数据更新方式
	int m_nMinTimeMins;//数据最小时间分钟数，可以为负数
	int m_nMaxTimeMins;//数据最大时间分钟数
};

struct SeqArgs
{
	WeekTime mLoadTime;
	WeekTime mCloseTime;
	SeqNumArgs mSeqNumArgs;
	SeqBufArgs mSeqBufArgs;
	SeqFilter mSeqFilter;
};

//idx文件头
struct SeqIdxHead
{
	DWORD m_dwNum;//数目
	DWORD m_dwCrc;//模板crc
};
//dat文件头
struct SeqDatHead
{
	DWORD m_dwDid;//did
	DWORD m_dwCrc;//模板crc
	DWORD m_dwNum;//记录数
	DWORD m_dwBlkNum;//目前数据块数
};
//+最后一条记录数据
//+压缩块数据

struct SeqFixHead
{
	WORD m_wDidNum;
	UINT64 m_uint64BufLen;
};

//m_wDidNum个
struct SeqDidFixHead
{
	DWORD m_dwDid;//数据did
	DWORD m_dwXmlCrc;//did模板crc
	SeqNumArgs mSeqNumArgs;//数目参数
	SeqBufArgs mSeqBufArgs;//内存参数
	MemOffsetLen m_moSeqMem;
};

//每个did的seq块
struct SeqMemHead
{
	DWORD m_dwChangeStkNum;//有效股票数目
	UINT64 m_uint64OffSet;//指向第四块已用的内存偏移
	MemOffsetLen m_moIdx;//idx索引内存，相对于基地址,m_pBase
	MemOffsetLen m_moLast;//最近一次变化内存,相对于基地址,m_pBase
	MemOffsetLen m_moRcnt;//最近n笔变化内存,相对于基地址,m_pBase
	MemOffsetLen m_moDat;//全部数据内存,相对于基地址,m_pBase
	BYTE m_byteReadFalg;//为0可读，为1不可读，需从内存访问
};
//m_dwChangeMaxNum个*2
struct SeqCodeIndex
{
	char code[MAXSEQCODELEN];
	DWORD m_dwIndex;
	int m_nTodayIndex;//每次增加码表都要重新赋值和排序
};
//m_dwChangeMaxNum个
//{
	struct SeqMemIdxHead
	{
		DWORD m_dwNum;//实际索引数目
		UINT64 m_uint64NewId;//最新id
	};
	
	//最后一条记录内容

	//m_dwMemMaxIndexNum个
	struct SeqIdx
	{
		dzh_time_t m_dtStartTime;//数据起始时间
		dzh_time_t m_dtEndTime;//数据结束时间
		DWORD m_dwId;//位置,从1开始,对于所有数据来说
		DWORD m_dwNum;//该块保存的数据数目
		DWORD m_dwCrc;//该块数据的crc
		UINT64 m_uint64Offset;//该块数据的内存偏移地址,从m_pJSegBase算起
		DWORD m_dwDatBitSize;//该块数据压缩后的位长度
	};
	struct MSeqIdx
	{
		SeqIdx mSeqIdx;
		UINT64 m_uint64Segment;//第一块segment,从m_pJSegBase算起
		DWORD m_dwPutBitStartPosByte;//该索引块数据在最后一块blk里开始起始位置,如果最后一块blk只保存了一个索引块，为0
		DWORD m_dwPutBitSize;//该索引块数据在最后一块blk里已使用的位数
	};
//}
#pragma pack()

#endif
