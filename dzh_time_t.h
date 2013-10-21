/********************************************************************
	created:	2012:12:12   12:40
	filename: 	dzh_time_t.h
	author:		fu.changyou

	purpose:	大智慧时间类定义
    attention:  大智慧时间是GM时间，即格林威治时间，因此在构造时需要传入该时间所对应的时区信息
*********************************************************************/
#ifndef _DZH_TIME_T_H_
#define _DZH_TIME_T_H_
#include <time.h>
#include <iostream>

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

#ifdef __linux 
#define MAX_DZH_TIME_SECOND     17592186044415ll  //大智慧时间的最大值
#define INVALID_DZH_TIME        0xFFFFFFFFFFFFFFFFll  //无效的大智慧时间
#else
#define MAX_DZH_TIME_SECOND     17592186044415i64   //大智慧时间的最大值
#define INVALID_DZH_TIME        0xFFFFFFFFFFFFFFFFi64  //无效的大智慧时间
#endif

/*时间相关格式代码为：
 *%Y（四位年份）%y（两位年份）
 *%m（月份)
 *%d (日期)
 *%H（小时，24小时），%h（小时，12小时 + AM/PM）
 *%M（分钟）
 *%S（秒）
 *%<digit>u （分数秒，精度<digit>[01-06]）*/
#define DZH_TIME_FORMAT_4YEAR   "%Y"    //四位年份
#define DZH_TIME_FORMAT_2YEAR   "%y"    //两位年份 前面两位省略
#define DZH_TIME_FORMAT_MONTH   "%m"    //月份
#define DZH_TIME_FORMAT_DAY     "%d"    //日期
#define DZH_TIME_FORMAT_24HOUR  "%H"    //24小时
#define DZH_TIME_FORMAT_12HOUR  "%h"    //12小时 + AM/PM
#define DZH_TIME_FORMAT_MINUTE  "%M"    //分钟
#define DZH_TIME_FORMAT_SECOND  "%S"    //秒
#define STR_TIME_FORMAT_1       "%Y%m%d%H%M%S%06u" //精确到微妙的YYYYMMDD24HMISSUUUUUU
#define STR_TIME_FORMAT_2       "%Y-%m-%d %H:%M:%S %06u" //精确到微妙的YYYY-MM-DD 24H:MI:SS UUUUUU
#define STR_TIME_FORMAT_3       "%Y%m%d%H%M%S"      //精确到微妙的YYYYMMDD24HMISS
#define STR_TIME_FORMAT_4       "%Y-%m-%d %H:%M:%S" //精确到微妙的YYYY-MM-DD 24H:MI:SS

//世界时区相对于标准时间的偏移分钟数定义
#define ZONE_MINUTE_UTCN1200 -720  //
#define ZONE_MINUTE_UTCN1100 -660  //
#define ZONE_MINUTE_UTCN1000 -600  //℉ST - 夏威夷－阿留申标准时间）
#define ZONE_MINUTE_UTCN930  -570  //
#define ZONE_MINUTE_UTCN900  -540  //（AKST - 阿拉斯加标准时间）
#define ZONE_MINUTE_UTCN800  -480  //（PST - 太平洋标准时间）
#define ZONE_MINUTE_UTCN700  -420  //（MST - 北美山区标准时间）
#define ZONE_MINUTE_UTCN600  -360  //（CST - 北美中部标准时间）
#define ZONE_MINUTE_UTCN500  -300  //（EST - 北美东部标准时间）
#define ZONE_MINUTE_UTCN430  -270  //
#define ZONE_MINUTE_UTCN400  -240  //（AST - 大西洋标准时间）
#define ZONE_MINUTE_UTCN330  -210  //（NST - 纽芬兰岛标准时间）
#define ZONE_MINUTE_UTCN300  -180  //
#define ZONE_MINUTE_UTCN200  -120  //
#define ZONE_MINUTE_UTCN100  -60   //
#define ZONE_MINUTE_UTC000    0    //（WET - 欧洲西部时区，GMT - 格林尼治标准时间）
#define ZONE_MINUTE_UTCP100   60   //（CET - 欧洲中部时区）
#define ZONE_MINUTE_UTCP200   120  //（EET - 欧洲东部时区）
#define ZONE_MINUTE_UTCP300   180  //（MSK - 莫斯科时区）
#define ZONE_MINUTE_UTCP330   210  //
#define ZONE_MINUTE_UTCP400   240  //
#define ZONE_MINUTE_UTCP430   270  //
#define ZONE_MINUTE_UTCP500   300  //
#define ZONE_MINUTE_UTCP530   330  //
#define ZONE_MINUTE_UTCP545   345  //
#define ZONE_MINUTE_UTCP600   360  //
#define ZONE_MINUTE_UTCP630   390  //
#define ZONE_MINUTE_UTCP700   420  //
#define ZONE_MINUTE_UTCP800   480  //（CST - 中国标准时间）
#define ZONE_MINUTE_UTCP900   540  //（KST- 韩国标准时间）
#define ZONE_MINUTE_UTCP930   570  //（ACST - 澳洲中部标准时间）
#define ZONE_MINUTE_UTCP1000  600  //（AEST - 澳洲东部标准时间）
#define ZONE_MINUTE_UTCP1030  630  //
#define ZONE_MINUTE_UTCP1100  660  //
#define ZONE_MINUTE_UTCP1130  690  //
#define ZONE_MINUTE_UTCP1200  720  //
#define ZONE_MINUTE_UTCP1245  765  //
#define ZONE_MINUTE_UTCP1300  780  //
#define ZONE_MINUTE_UTCP1400  840  //

/*大智慧TM时间结构*/
struct TDzhTimeNumStruct
{
    WORD uYear;     /*year, from 0*/
    BYTE uMonth;    /*month from 1-12*/
    BYTE uDay;      /*day from 1-31*/
    BYTE uHour;     /*hour from 0-23*/
    BYTE uMinute;   /*minute from 0-59*/
    BYTE uSecond;   /*second from 0-59*/
    BYTE uWeekDay;  /*week days since Sunday - [0,6]*/
    WORD uMsec;     /*millisecond from 0-999*/
    WORD uUsec;     /*microsecond from 0-999*/
    TDzhTimeNumStruct()
    {
        uYear = uMsec = uUsec = 0;
        uMonth = uDay = uHour = uMinute = uSecond = uWeekDay = 0;
    }
    TDzhTimeNumStruct(const TDzhTimeNumStruct& t)
    {
        uYear   = t.uYear;     /*year, from 0*/
        uMonth  = t.uMonth;    /*month from 1-12*/
        uDay    = t.uDay;      /*day from 1-31*/
        uHour   = t.uHour;     /*hour from 0-23*/
        uMinute = t.uMinute;   /*minute from 0-59*/
        uSecond = t.uSecond;   /*second from 0-59*/
        uMsec   = t.uMsec;     /*millisecond from 0-999*/
        uUsec   = t.uUsec;     /*microsecond from 0-999*/
        uWeekDay = t.uWeekDay; /*week days since Sunday - [0,6]*/
    }
};

#ifdef DLL_FILE
class _declspec(dllexport) dzh_time_t //导出类
#elif __linux 
class dzh_time_t
#else
class _declspec(dllimport) dzh_time_t
#endif
{
public:
    //构造一个空的大智慧时间
    dzh_time_t();

    //传入一个dzh_time_t
    dzh_time_t(const dzh_time_t& uDzhTime);

    //传入一个UINT64
    dzh_time_t(UINT64 uDzhTime);

    //分别传入整形表示的日期和时间，如dzh_time_t(20121221,235959),表示2012年12月21日23点59分59秒
    dzh_time_t(UINT32 uDate, UINT32 uTime);

    //传入大智慧的秒、毫秒、微妙
    dzh_time_t(UINT64 uSecFrom1900, WORD uMsec, WORD uUsec);

    /*传入精确到微妙的时间，NULL表示获取当前时间,
     *格式为YYYYMMDD24HMISS+3字节毫秒+3字节微妙,
     *如20121212124039020099，表示2012年12月12日 12时40分39秒 20毫秒 99微妙*/
    dzh_time_t(const char* pTime, short iZoneMinutes=0);

    //传入一个格林威治的time_t时间
    dzh_time_t(time_t &tTime);

    //传入一个tm结构，为NULL表示获取当前时间
    dzh_time_t(struct tm* pTime, short iZoneMinutes=0);

    //分别传入年月日时分秒及毫秒和微妙，小时均为24小时制
    dzh_time_t(const char* pYear, const char* pMonth, const char* pDay
        , const char* pHour, const char* pMinute, const char* pSecond
        , const char* pMsec=NULL, const char* pUsec=NULL, short iZoneMinutes=0);

    //分别传入年月日时分秒及毫秒和微妙，小时均为24小时制
    dzh_time_t(int y, int m, int d, int h, int minute, int s, int ms=0, int us=0, short iZoneMinutes=0);

    /*将大智慧时间按照要求格式化成字符串，同时支持时区计算
     *tTime：要格式化的大智慧时间，fmt：格式化参数，iZoneMinutes：时区信息
     *返回的是tTime+iZoneMinutes之后的时间字符串
     */
    const char* FormatTimeToString(const char* fmt=STR_TIME_FORMAT_1
        , char* pOutBuffer = NULL
        , int iBufferLen = 0
        , short iZoneMinutes=0)
    {
        return _FormatTimeToString(*this, fmt, pOutBuffer, iBufferLen, iZoneMinutes);
    }

    //将大智慧时间转换成TDzhTimeNumStruct
    struct TDzhTimeNumStruct DzhTM();

public:
    dzh_time_t operator+(const dzh_time_t& t);
    dzh_time_t operator-(const dzh_time_t& t);

    dzh_time_t operator+(const UINT64& t);
    dzh_time_t operator-(const UINT64& t);

    const dzh_time_t& operator+=(const dzh_time_t& t);
    const dzh_time_t& operator-=(const dzh_time_t& t);

    const dzh_time_t& operator+=(const UINT64& t);
    const dzh_time_t& operator-=(const UINT64& t);

    const dzh_time_t& operator=(const dzh_time_t& t) const;
    const dzh_time_t& operator=(const UINT64& t) const;

    bool operator==(const dzh_time_t& t) const;
    bool operator!=(const dzh_time_t& t) const;
    bool operator>(const dzh_time_t& t) const;
    bool operator<(const dzh_time_t& t) const;
    bool operator>=(const dzh_time_t& t) const;
    bool operator<=(const dzh_time_t& t) const;

    bool operator==(const UINT64& t) const;
    bool operator!=(const UINT64& t) const;
    bool operator>(const UINT64& t) const;
    bool operator<(const UINT64& t) const;
    bool operator>=(const UINT64& t) const;
    bool operator<=(const UINT64& t) const;

	time_t ToTime();		//转换成time_t格式，会丢失毫秒和微秒
	struct tm* ToGMTime(struct tm& tmTime);			//转换成GMT的tm格式，tm_yday存放的是毫秒和微秒值（毫秒自动转换成微秒）
	struct tm* ToLocalTime(struct tm& tmTime, short iZoneMinutes=0);	//转换成本地时间，tm_yday存放的是毫秒和微秒值（毫秒自动转换成微秒），tm_isdst值为0
    UINT64 To64(){return _value;}               //UINT64内存块
    UINT64 Second(){return secs;}               //获取秒 from 1900
    WORD   MilliSecond(){return msecs;}         //获取毫秒
    WORD   MicroSecond(){return usecs;}         //获取微秒
    dzh_time_t&   SetValue(UINT64 v){_value = v; return *this;} //设置值
    dzh_time_t&   SetSecond(UINT64 v);          //设置秒 from 1900
    dzh_time_t&   SetMilliSecond(WORD v);       //设置毫秒
    dzh_time_t&   SetMicroSecond(UINT32 v);     //设置微秒
    dzh_time_t&   AddSecond(UINT64 v);          //添加秒
    dzh_time_t&   AddMilliSecond(WORD v);       //添加毫秒
    dzh_time_t&   AddMicroSecond(UINT32 v);     //添加微秒
    dzh_time_t&   SubSecond(UINT64 v);          //减去秒
    dzh_time_t&   SubMilliSecond(WORD v);       //减去毫秒
    dzh_time_t&   SubMicroSecond(UINT32 v);     //减去微秒

public:
    //将字符串转换成大智慧时间，格式为YYYYMMDD24HMISS+3字节毫秒+3字节微妙
    static dzh_time_t StringToTime(const char* pStr, short iZoneMinutes=0)
    {
        dzh_time_t t;
        t._StringToDzhTime(pStr, iZoneMinutes);
        return t;
    }

    //将字符串转换成大智慧时间，指定格式，并进行时区计算
    static dzh_time_t StringToTime(const char* pStr, const char* fmt, short iZoneMinutes = 0);

    //获取当前时间，该时间是格林威治时间，经过了时区转换的,windows精确到毫秒，linux精确到微秒
    static dzh_time_t GetCurrentTime()
    {
        dzh_time_t t;
        t._GetCurrentTime();
        return t;
    }

    /*将大智慧时间转换成字符串, 转换后返回的是经过iZoneMinutes偏移的本地时间
    * 如大智慧时间是2012年12月20日 15点20分30秒 100毫秒 888微妙，iZoneMinutes是0分钟
    * 返回的结果是大智慧时间+iZoneMinutes，为20121220232030100888，即2012年12月20日 23点20分30秒 100毫秒 888微妙
    */
    static const char* TimeToString(dzh_time_t tTime
        , char* pOutBuffer = NULL
        , int iBufferLen = 0
        , short iZoneMinutes=0)
    {
        return tTime._TimeToString(tTime, pOutBuffer, iBufferLen, iZoneMinutes);
    }

    /*将大智慧时间按照要求格式化成字符串，同时支持时区计算
     *tTime：要格式化的大智慧时间，fmt：格式化参数，iZoneMinutes：时区信息
     *返回的是tTime+iZoneMinutes之后的时间字符串
     */
    static const char* FormatTimeToString(dzh_time_t& tTime
        , const char* fmt=STR_TIME_FORMAT_1
        , char* pOutBuffer = NULL
        , int iBufferLen = 0
        , short iZoneMinutes=0)
    {
        return tTime._FormatTimeToString(tTime, fmt, pOutBuffer, iBufferLen, iZoneMinutes);
    }

    /*将大智慧时间偏移某个时区得到格林威治的大智慧时间
    * tTime：需要进行偏移的大智慧时间
    * iZoneMinutes：需要进行偏移的分钟数
    * 返回偏移后的时间，计算方法是tTime-iZoneMinutes
    * 如北京时间
    */
    //static dzh_time_t GMTDzhTime(dzh_time_t& tTime, short iZoneMinutes);

    //将大智慧时间转换成TDzhTimeNumStruct
    static struct TDzhTimeNumStruct DzhTM(dzh_time_t& t);

    //大智慧TM时间转换成大智慧时间
    static dzh_time_t DzhTimeNumStruct2DzhTime(const struct TDzhTimeNumStruct& tTimeStruct);

private:
    dzh_time_t operator*(const dzh_time_t& t) const{return INVALID_DZH_TIME;};
    dzh_time_t operator/(const dzh_time_t& t) const{return INVALID_DZH_TIME;};
    dzh_time_t operator*(const UINT64& t) const{return INVALID_DZH_TIME;};
    dzh_time_t operator/(const UINT64& t) const{return INVALID_DZH_TIME;};
    dzh_time_t operator*=(const dzh_time_t& t) const{return INVALID_DZH_TIME;};
    dzh_time_t operator/=(const dzh_time_t& t) const{return INVALID_DZH_TIME;};
    dzh_time_t operator*=(const UINT64& t) const{return INVALID_DZH_TIME;};
    dzh_time_t operator/=(const UINT64& t) const{return INVALID_DZH_TIME;};
private:
    //格式为YYYYMMDD24HMISS+3字节毫秒+3字节微妙
    void _StringToDzhTime(const char* pStr, short iZoneMinutes=0);

    //获取当前时间
    void _GetCurrentTime();

    //将大智慧时间转换成字符串
    const char* _TimeToString(dzh_time_t& tTime
        , char* pOutBuffer = NULL
        , int iBufferLen = 0
        , short iZoneMinutes=0)
	{
		return _FormatTimeToString(tTime, STR_TIME_FORMAT_1, pOutBuffer, iBufferLen, iZoneMinutes);
	}

    //将time_t转换成大智慧时间
    void Time2DzhTime(time_t &tTime);

    //将tm时间转换成大智慧时间
    void TM2DzhTime(struct tm* tmTime);

    //将大智慧时间按照某种格式转换成字符串时间，同时支持时区计算，计算方法是在大智慧时间上加上时区偏移量
    const char* _FormatTimeToString(dzh_time_t& tTime
        , const char* fmt=STR_TIME_FORMAT_1
        , char* pOutBuffer = NULL
        , int iBufferLen = 0
        , short iZoneMinutes=0);
private:
    union
    {
        struct
        {
            mutable UINT64 secs:44;   //1900年1月1日00:00:00到当前时刻的秒数,time_t
            mutable UINT64 msecs:10;  //毫秒数
            mutable UINT64 usecs:10;  //微秒数
        };
        mutable UINT64 _value;
    };
    friend std::ostream& operator<<(std::ostream& os, const dzh_time_t& t);

};

inline std::ostream& operator<<(std::ostream& os, dzh_time_t& t)
{
    const char* pStr = t.FormatTimeToString(STR_TIME_FORMAT_2);
    if (NULL == pStr)
    {
        os << "(null)";
    }
    else
    {
        os << pStr;
    }
    return os;
}

#endif
