require("os")
--local path =  os.getenv("PWD")
--local lualib_path = path..'/lualib/'

require("base_type")
require("logic_process")

local zmq = require("zmq")

local ffi = require("ffi")

--io.write("this is lua script\n")

ffi.cdef [[
#pragma pack(1)

static const int  SHL2_MMP_SIZE=5;

int printf(const char *fmt, ...);

typedef unsigned int time_t;

enum DC_GENERAL_INTYPE
{
	GE_IOPV = 5,
	GE_MATU_YLD = 6,
	GE_HKREL_TXT = 1001,
	GE_STATIC_EX = 10001,
	GE_HKDYNA = 10002,
	GE_BLK_STK = 10003,
};

typedef struct UINT24
{
	unsigned short m_wLow;
	char m_cHigh;
}UINT24;

typedef struct MWORD
{
	enum
	{
		MAXBASE = 0x1FFFFFFF,
	};

	int m_nBase:30;
	unsigned int m_nMul:2;
}MWORD;

enum DC_TYPE
{
	DCT_NUL = 0,
	DCT_KEEPALIVE,			//<-->卤拢鲁脰脕卢陆脫1
	DCT_LOGIN,				//<-->碌脟脗陆脪脭录掳虏脡脢媒禄煤脢么脨脭	2
	DCT_REQPASS,			//<-->脪陋脟贸脫脙禄搂脙没脙脺脗毛碌脟脗陆3
	DCT_USERnPASS,			//<-->脫脙禄搂脙没脙脺脗毛4
	DCT_READY,				//-->碌脟脗陆鲁脡鹿娄拢卢驴脡脪脭陆脫脢脮脢媒戮脻5
	DCT_RESEND,				//-->脰脴路垄脢媒戮脻掳眉6
	DCT_STKSTATIC,			//<--戮虏脤卢脢媒戮脻7
	DCT_STKDYNA,			//<--露炉脤卢脨脨脟茅8
	DCT_SHL2_MMPEx,			//<--level2脌漏脮鹿脗貌脗么脜脤9
	DCT_SHL2_REPORT,		//<--level2路脰卤脢鲁脡陆禄10
	DCT_SHL2_BIG_WD,		//<--level2录麓脢卤脳卯麓贸鲁路碌楼11
	DCT_SHL2_ACCU_WD,		//<--level2脌脹录脝脳卯麓贸鲁路碌楼12
	DCT_HK_STATIC,			//<--赂脹鹿脡戮虏脤卢13
	DCT_HK_DYNA,			//<--赂脹鹿脡露炉脤卢14
	DCT_XML,				//<--XML赂帽脢陆脢媒戮脻15
	DCT_SHL2_QUEUE,			//<--脗貌脗么脜脤露脫脕脨拢卢level2脤脴脫脨16
	DCT_GENERAL,			//<--脥篓脫脙脢媒戮脻17
	DCT_USERSTAT,			//-->脫脙禄搂脢媒脕驴脥鲁录脝18
	DCT_RAWDATA,			//<-->脭颅脢录脢媒戮脻19
	DCT_NEWS,				//<--鹿芦赂忙脨脗脦脜碌脠脦脛卤戮脌脿脨脥脢媒戮脻20
	DCT_SZL2_ORDER_QUEUE,	//<--脦炉脥脨露脫脕脨拢卢脡卯脹脷Level2脤脴脫脨21
	DCT_SZL2_ORDER_STAT,	//<--脦炉脥脨露脫脕脨脥鲁录脝拢卢脡卯脹脷Level2脤脴脫脨22

	DCT_SZL2_ORDER_FIVE=100,//<--脦炉脥脨露脫脕脨露脿卤脢脙梅脧赂拢卢脡卯脹脷Level2脤脴脫脨
	DCT_SZL2_TRADE_FIVE=101,//<--鲁脡陆禄,鲁路碌楼露脫脕脨露脿卤脢脙梅脧赂拢卢脡卯脹脷Level2脤脴脫脨
};

typedef struct DC_STKSTATIC_MY
{
	unsigned long	m_dwVersion;	//戮虏脤卢脢媒戮脻掳忙卤戮,脙驴麓脦戮虏脤卢脢媒戮脻路垄脡煤卤盲禄炉脭貌卤脴脨毛脭枚录脫脢媒脰碌,脙驴赂枚脨脨脟茅脭麓脳卯潞脙脫脨脪禄赂枚脧脿露脭露脌脕垄碌脛赂脽脦禄拢卢脮芒脩霉虏禄脥卢脨脨脟茅脭麓戮脥虏禄禄谩脫脨脧脿脥卢碌脛掳忙卤戮
	unsigned short	m_wAttrib;		//掳忙卤戮脢么脨脭
	unsigned int	m_nDay;			//戮虏脤卢驴芒脠脮脝脷拢卢YYYYMMDD
	short	m_nNum;			//m_data脢媒脕驴
}DC_STKSTATIC_MY;


typedef struct STK_STATIC
{
	unsigned short	id;			//卤戮脢脨鲁隆脛脷脦篓脪禄卤锚脢戮,脭脷卤戮脢脨鲁隆脛脷碌脛脨貌潞脜
	char	label[10];		//麓煤脗毛
	char	name[32];		//脙没鲁脝
	unsigned char	type;			//STK_TYPE
	unsigned char	price_digit;		//录脹赂帽脳卯脨隆路脰卤忙脗脢拢卢路脟鲁拢脰脴脪陋拢卢脙驴脪禄赂枚unsigned int脌脿脨脥碌脛录脹赂帽露录脪陋鲁媒脪脭10^m_nPriceDigit虏脜脢脟脮忙脮媒碌脛录脹赂帽
	short	 vol_unit;			//鲁脡陆禄脕驴碌楼脦禄拢卢脙驴脪禄鲁脡陆禄脕驴碌楼脦禄卤铆脢戮露脿脡脵鹿脡
	MWORD	float_issued;		//脕梅脥篓鹿脡卤戮
	MWORD	total_issued;		//脳脺鹿脡卤戮

	unsigned long	last_close;		//脳貌脢脮
	unsigned long	adv_stop;		//脮脟脥拢
	unsigned long	dec_stop;
}STK_STATIC;

typedef struct DC_STKSTATIC
{
	unsigned long	m_dwVersion;	//戮虏脤卢脢媒戮脻掳忙卤戮,脙驴麓脦戮虏脤卢脢媒戮脻路垄脡煤卤盲禄炉脭貌卤脴脨毛脭枚录脫脢媒脰碌,脙驴赂枚脨脨脟茅脭麓脳卯潞脙脫脨脪禄赂枚脧脿露脭露脌脕垄碌脛赂脽脦禄拢卢脮芒脩霉虏禄脥卢脨脨脟茅脭麓戮脥虏禄禄谩脫脨脧脿脥卢碌脛掳忙卤戮,0x80000000
	unsigned short	m_wAttrib;		//掳忙卤戮脢么脨脭
	unsigned int	m_nDay;			//戮虏脤卢驴芒脠脮脝脷拢卢YYYYMMDD
	short	 m_nNum;			//m_data脢媒脕驴
	STK_STATIC m_data[1];	//脢媒戮脻
}DC_STKSTATIC;

typedef struct DC_STKDYNA_MY
{
	unsigned short	m_wDynaSeq;		//露炉脤卢脨脨脟茅脨貌脕脨潞脜拢卢脫脙脫脷驴脥禄搂露脣脳陋路垄
	short   m_nNum;
}DC_STKDYNA_MY;

typedef struct STK_DYNA
{
	unsigned short	id;			//鹿脡脝卤ID
	time_t	deal_time;				//鲁脡陆禄脢卤录盲
	unsigned long  open;			//驴陋脜脤
	unsigned long	high;			//脳卯赂脽
	unsigned long	low;			//脳卯碌脥
	unsigned long	last;			//脳卯脨脗
	MWORD	vol;			//鲁脡陆禄脕驴
	MWORD	amount;			//鲁脡陆禄露卯
	MWORD	inner_vol;		//脛脷脜脤鲁脡陆禄脕驴,<0卤铆脢戮赂脙卤脢鲁脡陆禄脦陋脰梅露炉脗么拢卢>=0卤铆脢戮脰梅露炉脗貌,戮酶露脭脰碌卤铆脢戮脛脷脜脤鲁脡陆禄脕驴
	unsigned long	tick_count;			//脌脹录脝鲁脡陆禄卤脢脢媒
	unsigned long	buy_price[5];		//脦炉脗貌录脹赂帽
	unsigned long	buy_vol[5];			//脦炉脗貌脕驴
	unsigned long	sell_price[5];		//脦炉脗么录脹赂帽
	unsigned long	sell_vol[5];			//脦炉脗么脕驴
	unsigned long	open_interest;		//鲁脰虏脰脕驴(脝脷禄玫脝脷脰赂脤脴脫脨)
	unsigned long	settle_price;		//陆谩脣茫录脹(脝脷禄玫脝脷脰赂脧脰禄玫脤脴脫脨)
}STK_DYNA;

typedef struct DC_STKDYNA
{
	unsigned short	m_wDynaSeq;		//露炉脤卢脨脨脟茅脨貌脕脨潞脜拢卢脫脙脫脷驴脥禄搂露脣脳陋路垄
	short   m_nNum;
	STK_DYNA m_data[1];
}DC_STKDYNA;

typedef struct SHL2_MMPEX
{
	unsigned short	id;			//鹿脡脝卤ID
	unsigned int	avg_buy_price;	//录脫脠篓脝陆戮霉脦炉脗貌录脹赂帽
	MWORD	all_buy_vol;		//脦炉脗貌脳脺脕驴
	unsigned int	avg_sell_price;	//录脫脠篓脝陆戮霉脦炉脗么录脹赂帽
	MWORD	all_sell_vol;		//脦炉脗么脳脺脕驴
	unsigned int	buy_price[SHL2_MMP_SIZE];	//脦炉脗貌录脹6-10
	unsigned int	buy_vol[SHL2_MMP_SIZE];		//脦炉脗貌脕驴6-10
	unsigned int	sell_price[SHL2_MMP_SIZE];	//脦炉脗么录脹6-10
	unsigned int	sell_vol[SHL2_MMP_SIZE];		//脦炉脗么脕驴6-10
}SHL2_MMPEX;

typedef struct SHL2_Queue
{
	unsigned short mmp;
}SHL2_Queue;

typedef struct DCS_STKSTATIC_Ex_MY
{
	short	m_nNum;
}DC_STKSTATIC_Ex_MY;


typedef struct STK_STATICEx
{
	enum STK_SUBTYPE
	{
		NILTYPE = 0,
		ASHARE	= 'A',			//A鹿脡,陆枚露脭STOCK,WARRANT脫脨脨搂
		BSHARE	= 'B',			//B鹿脡,陆枚露脭STOCK,WARRANT脫脨脨搂
		GOV_BOND = 'G',			//鹿煤脮庐,陆枚露脭BOND脫脨脨搂
		ENT_BOND = 'O',			//脝贸脪碌脮庐,陆枚露脭BOND脫脨脨搂
		FIN_BOND = 'F',			//陆冒脠脷脮庐,陆枚露脭BOND脫脨脨搂
	};
	enum STK_SP//鹿脡脝卤脢么脨脭
	{
		NULLSP = 0,
		NSP	= 'N',//脮媒鲁拢
		SSP	= 'S',//ST鹿脡
		PSP = 'P',//PT鹿脡
		TSP = 'T',//麓煤掳矛脳陋脠脙脰陇脠炉
		LSP = 'L',//脡脧脢脨驴陋路脜脨脥禄霉陆冒拢篓LOF拢漏
		OSP = 'O',//陆枚陆脪脢戮戮禄脰碌碌脛驴陋路脜脢陆禄霉陆冒
		FSP = 'F',//路脟陆禄脪脳脨脥驴陋路脜脢陆禄霉陆冒
		ESP = 'E',//ETF
		ZSP = 'Z',//麓娄脫脷脥脣脢脨脮没脌铆脝脷碌脛脰陇脠炉
	};
	char	m_cType;			//STK_TYPE,脫脡赂脙卤锚脰戮戮枚露篓脕陋潞脧脰脨脢鹿脫脙脛脛脪禄赂枚陆谩鹿鹿
	char	m_cSubType;			//STK_SUBTYPE
	union
	{
		struct 		//鹿脡脝卤	(STOCK,OTHER_STOCK)
		{
			float	m_fFaceValue;		//	脙忙脰碌
			float	m_fProfit;			//	脙驴鹿脡脢脮脪忙
			unsigned short	m_wIndustry;		//	脨脨脪碌'A' -- 'M',虏脦录没 industryClasify
			char	m_cTradeStatus;		//	陆禄脪脳脳麓脤卢拢卢'N'=Normal, 'H'=Halt, 'X'=Not trade on this Market
			float	m_fCashDividend;	//	脙驴鹿脡潞矛脌没
			char	m_cSecurityProperties;//脤脴脢芒路脰脌脿卤锚录脟拢卢脠隆脰碌录炉潞脧STK_SP脰脨碌脛脰碌
			unsigned long	m_dwLastTradeDate;//脳卯潞贸陆禄脪脳脠脮拢卢脛驴脟掳脦陋驴脮拢卢脳枚脭陇脕么脫脙

		} m_equitySpec;
		 struct 		//禄霉陆冒,ETF,LOF	(FUND,ETF,LOF)
		{
			float	m_fFaceValue;		//	脙忙脰碌
			float	m_fTotalIssued;		//	脳脺鹿脡卤戮
			float	m_fIOPV;			//	IOPV戮禄脰碌,陆枚露脭ETF,LOF脫脨脨搂拢卢***
		} m_fundSpec;
		 struct		//脠篓脰陇,脩隆脭帽脠篓	(OPTION,WARRANT)
		{
			char	m_cStyle;			//  'A' or 'E'	American or European Style
			char	m_cCP;				//	'C' or 'P' Call or Put
			float	m_fCnvtRatio;		//  露脪禄禄/脨脨脠篓卤脠脌媒
			float	m_fStrikePrice;		//	脨脨脠篓录脹赂帽
			unsigned long	m_dwMaturityDate;	//	碌陆脝脷脠脮,YYYYMMDD
			char	m_strUnderLine[12];	//	露脭脫娄鹿脡脝卤,掳眉潞卢脢脨鲁隆麓煤脗毛拢卢脠莽SH600000
			float	m_fBalance;			//  脕梅脥篓脫脿露卯
		} m_warrantSpec;
		 struct 		//脮庐脠炉	(BOND)
		{
			unsigned long	m_dwMaturityDate;	//	碌陆脝脷脠脮,YYYYMMDD
			unsigned long	m_dwIntAccruDate;	//	脝冒脧垄脠脮拢卢YYYYMMDD
			float	m_fIssuePrice;		//	路垄脨脨录脹
			float	m_fCouponRate;		//	脌没脗脢
			float	m_fFaceValue;		//	脙忙脰碌
			float	m_fAccruedInt;		//	脫娄录脝脌没脧垄,***
		} m_bondSpec;
		 struct		//驴脡脳陋脮庐	(COV_BOND)
		{
			char	m_cStyle;			//  'A' or 'E'	American or European Style
			char	m_cCP;				//	'C' or 'P' Call or Put
			float	m_fCnvtRatio;		//  露脪禄禄/脨脨脠篓卤脠脌媒
			float	m_fStrikePrice;		//	脨脨脠篓录脹赂帽
			unsigned long	m_dwMaturityDate;	//	碌陆脝脷脠脮,YYYYMMDD
			char	m_strUnderLine[12];	//	露脭脫娄鹿脡脝卤,掳眉潞卢脢脨鲁隆麓煤脗毛拢卢脠莽SH600000
			float	m_fAccruedInt;		//	脫娄录脝脌没脧垄
		} m_CnvtSpec;
	   struct		//脝脷禄玫,脡脤脝路	(FUTURE,FTR_IDX,COMM)
		{
			unsigned long	last_day_oi;			//脳貌脠脮鲁脰虏脰脕驴
			float	last_settle_price;		//脳貌脠脮陆谩脣茫录脹
		} m_futureSpec;
		 struct	//脨脜脥脨	(TRUST)
		{
			float	m_dwfAsset;			//戮禄脳脢虏煤
			unsigned long	m_dwAssetDate;		//YYYYMMDD
		} m_trustSpec;
	}Spec;
}STK_STATICEx;  

typedef struct STK_HKDYNA
{
	unsigned long	m_dwIEPrice;	//脝陆潞芒录脹
	MWORD	m_mIEVolume;	//脝陆潞芒脕驴

	//脗貌脗么脜脤卤脢脢媒
	unsigned short	m_wBuyOrderNum[5];
	unsigned short	m_wSellOrderNum[5];

	//戮颅录脥露脫脕脨
	struct HK_BROKER	
	{
		unsigned short	m_wNumBroker;
		unsigned short	m_wBrokerNo[40];
		char	m_cType[40];		//'B':broker,'R':register trader,'S':number of spread
	}m_buyBroker,m_sellBroker;
}STK_HKDYNA;

typedef struct SZL2_ORDER_STAT
{
	unsigned short m_wStkID;				//鹿脡脝卤ID
	MWORD	m_nBuyOrderVol[4];		//脗貌脠毛碌楼脕驴拢卢脨隆隆垄脰脨隆垄麓贸隆垄脤脴麓贸
	MWORD	m_nSellOrderVol[4];		//脗么鲁枚碌楼脕驴拢卢脨隆隆垄脰脨隆垄麓贸隆垄脤脴麓贸
	UINT24	m_dwOrderNum[2];		//脦炉脥脨碌楼脢媒脕驴拢卢脗貌脠毛/脗么鲁枚
	MWORD	m_nWDVol[2];			
}SZL2_ORDER_STAT;

typedef struct SZL2_ORDER_FIVE
{
	unsigned short nIndex;
	char strOrderKind;
	char strFunCode;
	unsigned long dwPrice;
	unsigned long dwAmount;
	unsigned long dwRecNO;
	int   nSetNO;
	unsigned long nRecTime;
}SZL2_ORDER_FIVE;

typedef struct SZL2_TRADE_FIVE
{
	unsigned short nIndex;
	char strOrderKind;
	char strFunCode;
	unsigned long dwPrice;
	unsigned long dwAmount;
	unsigned long dwRecNO;
	int   nSetNO;
	unsigned long   nRecTime;
}SZL2_TRADE_FIVE;

#pragma unpack()
]]

local C = ffi.C
local ffi_cast = ffi.cast 
local str_format = string.format

function FormatReturnError(dc_type,ret_error)
	local ret_str
	if(ret_error ~= nil) then
		ret_str = str_format("%s:%s\0", dc_type,ret_error)
	else
		ret_str = nil
	end
	return ret_str
end

function GetZMQPattern(pattern)
	if pattern == 0 then
		return zmq.PAIR
	elseif pattern == 1 then
		return zmq.PUB
	elseif pattern == 2 then
		return zmq.SUB
	elseif pattern == 3 then
		return zmq.REQ
	elseif pattern == 4 then
		return zmq.REP
	elseif pattern == 5 then
		return zmq.DEALER
	elseif pattern == 6 then
		return zmq.ROUTER 
	elseif pattern == 7 then
		return zmq.PULL 
	elseif pattern == 8 then
		return zmq.PUSH 
	elseif pattern == 9 then
		return zmq.XPUB
	elseif pattern == 10 then
		return zmq.XSUB
	else
		return nil
	end
end

local ctx
local sock
local sock_pattern
function InitZMQ(pattern, action, addr)
	ctx = zmq.init()
	sock_pattern = GetZMQPattern(pattern)	
	assert(sock_pattern)
	sock = assert(ctx:socket(sock_pattern))
	sock_addr = addr
	if action == "connect" then
		assert(sock:connect(sock_addr))		
		print"connect"
	elseif action == "bind" then
		assert(sock:bind(sock_addr));
		print"bind"
	end
end

function SendErrorMsg(error_msg)
	--print(error_msg)
	local msg = zmq.zmq_msg_t.init()	
	msg:set_data(error_msg)
	--print(type(buf))
	--local s = ffi.string(msg:data())
	sock:send_msg(msg)
end

local did_template_id_table = {}
function init(did_template_id)
	req_id = require(did_template_id)
	table.insert(did_template_id_table,req_id)	
end

function test_process(dctype, num, pdcdata)
	local stk 
	local dc_type
	local ret_error
	local ret_str 

	if dctype == C.DCT_STKSTATIC then
		stk = ffi_cast("STK_STATIC *", pdcdata)
		dc_type = "static"
		for i=1,num do
			--print(stk.id)
			ret_error = handle_stk_static(stk)			
			ret_str = FormatReturnError(dc_type, ret_error)
			--test
			ret_str = "stk-static\0"
			if ret_str ~= nil then 
				SendErrorMsg(ret_str)		
			end
			stk = stk + 1
		end
	elseif dctype == C.DCT_STKDYNA then
		stk = ffi_cast("STK_DYNA *", pdcdata)
		dc_type = "dyna"
		for i=1,num do
			ret_error = handle_stk_dyna(stk)
			ret_str = FormatReturnError(dc_type, ret_error)
			ret_str = "stk-stkdyna\0"
			if ret_str ~= nil then
				SendErrorMsg(ret_str)
			end
			stk = stk + 1
		end
	elseif dctype == C.DCT_SZL2_ORDER_STAT then
		stk = ffi_cast("SZL2_ORDER_STAT *", pdcdata)
		dc_type = "szl2_order_stat"
		for i=1,num do
			ret_error = handle_szl2_order_stat(stk) 
			ret_str = FormatReturnError(dc_type, ret_error)
			ret_str = "shl2-order-stat\0"
			if ret_str ~= nil then
				SendErrorMsg(ret_str)
			end
			stk = stk + 1
		end
	elseif dctype == C.DCT_SZL2_TRADE_FIVE then
		stk = ffi_cast("SZL2_TRADE_FIVE*", pdcdata)
		dc_type = "szl2_trade_five\0"
		for i=1,num do
			ret_error = handle_szl2_trade_five(stk)
			ret_str	= FormatReturnError(dc_type, ret_error)
			ret_str = dc_type
			if ret_str ~= nil then
				SendErrorMsg(ret_str)
			end
			stk = stk + 1
		end
	elseif dctype == C.DCT_SZL2_ORDER_FIVE then
		stk = ffi_cast("SZL2_ORDER_FIVE*", pdcdata)
		dc_type = "szl2_order_five\0"
		for i=1,num do
			ret_error = handle_szl2_order_five(stk)
			ret_str = FormatReturnError(dc_type, ret_error)
			ret_str = dc_type
			if ret_str ~= nil then
				SendErrorMsg(ret_str)
			end
			stk = stk + 1
		end
	elseif dctype == C.DCT_SHL2_MMPEx then
		stk = ffi_cast("SHL2_MMPEX *", pdcdata)
		dc_type = "shl2_mmpex"
		for i=1,num do
			ret_error = handle_shl2_mmpex(stk)
			ret_str = FormatReturnError(dc_type, ret_error)
			ret_str = "shl2-mmpex\0"
			if ret_str ~= nil then
				SendErrorMsg(ret_str)
			end
			stk = stk + 1
		end
	else
		ret_str = nil
	end
end

function test_process_did(template_id, num, pdcdata)
	local ret_error
	local ret_str 
	local pdata
	local template_type 
	local template = require(template_id)
	if template_id == 100000 then
		pdata = ffi_cast("T_BUY_SELL_INFO *", pdcdata)
		template_type = "t_buy_sell_info"
		for i=1,num do
			ret_error = handle_t_buy_sell_info(pdata)
            ret_str = FormatReturnError(template_type, ret_error)
            ret_str = "t_buy_sell_info\0"
            if ret_str ~= nil then
                SendErrorMsg(ret_str)
            end
            pdata = pdata + 1		
		end	
	elseif template_id == 100001 then
		pdata = ffi_cast("T_BUY_SELL_TICK_INFO *", pdcdata)
        template_type = "t_buy_sell_tick_info"
        for i=1,num do
            ret_error = handle_t_buy_sell_tick_info(pdata)
            ret_str = FormatReturnError(template_type, ret_error)
            ret_str = "t_buy_sell_tick_info\0"
            if ret_str ~= nil then
                SendErrorMsg(ret_str)
            end
            pdata = pdata + 1
        end
	elseif template_id == 100002 then
        pdata = ffi_cast("T_IOPV_INFO *", pdcdata)
        template_type = "t_iopv_info"
        for i=1,num do
            ret_error = handle_t_iopv_info(pdata)
            ret_str = FormatReturnError(template_type, ret_error)
            ret_str = "t_iopv_info\0"
            if ret_str ~= nil then
                SendErrorMsg(ret_str)
            end
            pdata = pdata + 1
        end
    elseif template_id == 100012 then
        pdata = ffi_cast("T_CBT_MARKET *", pdcdata)
        template_type = "t_cbt_market"
        for i=1,num do
            ret_error = handle_t_cbt_market(pdata)
            ret_str = FormatReturnError(template_type, ret_error)
            ret_str = "t_cbt_market\0"
            if ret_str ~= nil then
                SendErrorMsg(ret_str)
            end
            pdata = pdata + 1
        end
    elseif template_id == 100030 then
        pdata = ffi_cast("T_ETF_INFO *", pdcdata)
        template_type = "t_etf_info"
        for i=1,num do
            ret_error = handle_t_etf_info(pdata)
            ret_str = FormatReturnError(template_type, ret_error)
            ret_str = "t_etf_info\0"
            if ret_str ~= nil then
                SendErrorMsg(ret_str)
            end
            pdata = pdata + 1
        end
    elseif template_id == 100032 then
        pdata = ffi_cast("T_MMP_INFO *", pdcdata)
        template_type = "t_mmp_info"
        for i=1,num do
            ret_error = handle_t_mmp_info(pdata)
            ret_str = FormatReturnError(template_type, ret_error)
            ret_str = "t_mmp_info\0"
            if ret_str ~= nil then
                SendErrorMsg(ret_str)
            end
            pdata = pdata + 1
        end
	else
		ret_str = nil	
	end
end

function process_general(intype, num, pdata)
	local stk
	local ret_error
	local ret_str 
		for i=1, num do
			if(intype == C.GE_STATIC_EX) then
				stk = ffi_cast("STK_STATICEx *" ,pdata)
				--print(stk.m_cType)
				--print(stk.m_cSubType)
				if(stk.m_cType == 1) then
					dc_type = "ge staticex equity\0"
					ret_error = handle_equity(stk.Spec.m_equitySpec)
					ret_str = FormatReturnError(dc_type,ret_error)	
					ret_str = dc_type
					if ret_str ~= nil then
						SendErrorMsg(ret_str)
					end
				elseif(stk.m_cType == 2) then
					dc_type = "ge staticex fund\0"
					ret_error = handle_fund(stk.Spec.m_fundSpec)
					ret_str = FormatReturnError(dc_type, ret_error)
					ret_str = dc_type 
					if ret_str ~= nil then
						SendErrorMsg(ret_str)
					end
				elseif(stk.m_cType == 3) then
					dc_type = "ge staticex warrant\0"
					ret_error = handle_fund(stk.Spec.m_warrantSpec)
                    ret_str = FormatReturnError(dc_type, ret_error)
                    ret_str = dc_type 
                    if ret_str ~= nil then
                        SendErrorMsg(ret_str)
                    end
					--print("warrantSpec")
					--print(stk.Spec.m_warrantSpec.m_cStyle)
					--print(stk.Spec.m_warrantSpec.m_cCP)
					--print(stk.Spec.m_warrantSpec.m_fStrikePrice)
				elseif(stk.m_cType == 4) then
					dc_type = "ge staticex bond\0"
                    ret_error = handle_fund(stk.Spec.m_bondSpec)
                    ret_str = FormatReturnError(dc_type, ret_error)
                    ret_str = dc_type 
                    if ret_str ~= nil then
                        SendErrorMsg(ret_str)
                    end

					--print("bondSpec")
					--print(stk.Spec.m_bondSpec.m_dwMaturityDate)
					--print(stk.Spec.m_bondSpec.m_dwIntAccruDate)
					--print(stk.Spec.m_bondSpec.m_fIssuePrice)
					--print(stk.Spec.m_bondSpec.m_fFaceValue)
				elseif(stk.m_cType == 5) then
					dc_type = "ge staticex Cnvt\0"
					ret_error = handle_fund(stk.Spec.m_CnvtSpec)
                    ret_str = FormatReturnError(dc_type, ret_error)
                    ret_str = dc_type
                    if ret_str ~= nil then
                        SendErrorMsg(ret_str)
                    end
				elseif(stk.m_cType == 6) then
					dc_type = "ge staticex future\0"
					ret_error = handle_future(stk.Spec.m_futureSpec)
					ret_str = FormatReturnError(dc_type,ret_error)
					ret_str = dc_type
					if ret_str ~= nil then
                        SendErrorMsg(ret_str)
                    end
				elseif(stk.m_cType == 7) then
              	    dc_type = "ge staticex trust\0"
                    ret_error = handle_future(stk.Spec.m_trustSpec)
                    ret_str = FormatReturnError(dc_type,ret_error)
                    ret_str = dc_type
                    if ret_str ~= nil then
                        SendErrorMsg(ret_str)
                    end
				else				 
					ret_str = nil
				end 
				stk = stk + 1
			elseif(intype == C.GE_HKDYNA) then
				stk = ffi_cast("STK_HKDYNA *",data)
				dc_type = "ge stk hkdyna\0"
				ret_error = handle_hkdyna(stk)
				ret_str = FormatReturnError(dc_type, ret_error)
				ret_str = dc_type 
				if ret_str ~= nil then
					SendErrorMsg(ret_str)
				end
				stk = stk + 1
			elseif(intype == C.GE_IOPV) then
				--stk = ffi_cast("IOPV *",data)
				--dc_type = "ge iopv"
				----ret_error = handle_iopv(stk.value)
				--ret_error = nil
				--ret_str = FormatReturnError(dc_type,ret_error) 
				--ret_str = dc_type
                --if ret_str ~= nil then
                --    SendErrorMsg(ret_str)
                --end
                --stk = stk + 1
			else 
				ret_str = nil
			end
		end
end

--TODO FIX
function process_shl2_queue(dctype, pdcdata)
	local stk
	if dctype == C.DCT_SHL2_QUEUE then
		stk = ffi_cast("SHL2_Queue *", pdcdata)
		local ret_error = handle_shl2_mmp(stk)
		dc_type = "shl2_queue"
		local ret_str = FormatReturnError(dc_type, ret_error)
	end
	return ret_str
end

