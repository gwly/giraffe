/********************************************************************
	created:	2012:12:12   12:40
	filename: 	dzh_time_t.h
	author:		fu.changyou

	purpose:	���ǻ�ʱ���ඨ��
    attention:  ���ǻ�ʱ����GMʱ�䣬����������ʱ�䣬����ڹ���ʱ��Ҫ�����ʱ������Ӧ��ʱ����Ϣ
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
#define MAX_DZH_TIME_SECOND     17592186044415ll  //���ǻ�ʱ������ֵ
#define INVALID_DZH_TIME        0xFFFFFFFFFFFFFFFFll  //��Ч�Ĵ��ǻ�ʱ��
#else
#define MAX_DZH_TIME_SECOND     17592186044415i64   //���ǻ�ʱ������ֵ
#define INVALID_DZH_TIME        0xFFFFFFFFFFFFFFFFi64  //��Ч�Ĵ��ǻ�ʱ��
#endif

/*ʱ����ظ�ʽ����Ϊ��
 *%Y����λ��ݣ�%y����λ��ݣ�
 *%m���·�)
 *%d (����)
 *%H��Сʱ��24Сʱ����%h��Сʱ��12Сʱ + AM/PM��
 *%M�����ӣ�
 *%S���룩
 *%<digit>u �������룬����<digit>[01-06]��*/
#define DZH_TIME_FORMAT_4YEAR   "%Y"    //��λ���
#define DZH_TIME_FORMAT_2YEAR   "%y"    //��λ��� ǰ����λʡ��
#define DZH_TIME_FORMAT_MONTH   "%m"    //�·�
#define DZH_TIME_FORMAT_DAY     "%d"    //����
#define DZH_TIME_FORMAT_24HOUR  "%H"    //24Сʱ
#define DZH_TIME_FORMAT_12HOUR  "%h"    //12Сʱ + AM/PM
#define DZH_TIME_FORMAT_MINUTE  "%M"    //����
#define DZH_TIME_FORMAT_SECOND  "%S"    //��
#define STR_TIME_FORMAT_1       "%Y%m%d%H%M%S%06u" //��ȷ��΢���YYYYMMDD24HMISSUUUUUU
#define STR_TIME_FORMAT_2       "%Y-%m-%d %H:%M:%S %06u" //��ȷ��΢���YYYY-MM-DD 24H:MI:SS UUUUUU
#define STR_TIME_FORMAT_3       "%Y%m%d%H%M%S"      //��ȷ��΢���YYYYMMDD24HMISS
#define STR_TIME_FORMAT_4       "%Y-%m-%d %H:%M:%S" //��ȷ��΢���YYYY-MM-DD 24H:MI:SS

//����ʱ������ڱ�׼ʱ���ƫ�Ʒ���������
#define ZONE_MINUTE_UTCN1200 -720  //
#define ZONE_MINUTE_UTCN1100 -660  //
#define ZONE_MINUTE_UTCN1000 -600  //�HST - �����ģ��������׼ʱ�䣩
#define ZONE_MINUTE_UTCN930  -570  //
#define ZONE_MINUTE_UTCN900  -540  //��AKST - ����˹�ӱ�׼ʱ�䣩
#define ZONE_MINUTE_UTCN800  -480  //��PST - ̫ƽ���׼ʱ�䣩
#define ZONE_MINUTE_UTCN700  -420  //��MST - ����ɽ����׼ʱ�䣩
#define ZONE_MINUTE_UTCN600  -360  //��CST - �����в���׼ʱ�䣩
#define ZONE_MINUTE_UTCN500  -300  //��EST - ����������׼ʱ�䣩
#define ZONE_MINUTE_UTCN430  -270  //
#define ZONE_MINUTE_UTCN400  -240  //��AST - �������׼ʱ�䣩
#define ZONE_MINUTE_UTCN330  -210  //��NST - Ŧ��������׼ʱ�䣩
#define ZONE_MINUTE_UTCN300  -180  //
#define ZONE_MINUTE_UTCN200  -120  //
#define ZONE_MINUTE_UTCN100  -60   //
#define ZONE_MINUTE_UTC000    0    //��WET - ŷ������ʱ����GMT - �������α�׼ʱ�䣩
#define ZONE_MINUTE_UTCP100   60   //��CET - ŷ���в�ʱ����
#define ZONE_MINUTE_UTCP200   120  //��EET - ŷ�޶���ʱ����
#define ZONE_MINUTE_UTCP300   180  //��MSK - Ī˹��ʱ����
#define ZONE_MINUTE_UTCP330   210  //
#define ZONE_MINUTE_UTCP400   240  //
#define ZONE_MINUTE_UTCP430   270  //
#define ZONE_MINUTE_UTCP500   300  //
#define ZONE_MINUTE_UTCP530   330  //
#define ZONE_MINUTE_UTCP545   345  //
#define ZONE_MINUTE_UTCP600   360  //
#define ZONE_MINUTE_UTCP630   390  //
#define ZONE_MINUTE_UTCP700   420  //
#define ZONE_MINUTE_UTCP800   480  //��CST - �й���׼ʱ�䣩
#define ZONE_MINUTE_UTCP900   540  //��KST- ������׼ʱ�䣩
#define ZONE_MINUTE_UTCP930   570  //��ACST - �����в���׼ʱ�䣩
#define ZONE_MINUTE_UTCP1000  600  //��AEST - ���޶�����׼ʱ�䣩
#define ZONE_MINUTE_UTCP1030  630  //
#define ZONE_MINUTE_UTCP1100  660  //
#define ZONE_MINUTE_UTCP1130  690  //
#define ZONE_MINUTE_UTCP1200  720  //
#define ZONE_MINUTE_UTCP1245  765  //
#define ZONE_MINUTE_UTCP1300  780  //
#define ZONE_MINUTE_UTCP1400  840  //

/*���ǻ�TMʱ��ṹ*/
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
class _declspec(dllexport) dzh_time_t //������
#elif __linux 
class dzh_time_t
#else
class _declspec(dllimport) dzh_time_t
#endif
{
public:
    //����һ���յĴ��ǻ�ʱ��
    dzh_time_t();

    //����һ��dzh_time_t
    dzh_time_t(const dzh_time_t& uDzhTime);

    //����һ��UINT64
    dzh_time_t(UINT64 uDzhTime);

    //�ֱ������α�ʾ�����ں�ʱ�䣬��dzh_time_t(20121221,235959),��ʾ2012��12��21��23��59��59��
    dzh_time_t(UINT32 uDate, UINT32 uTime);

    //������ǻ۵��롢���롢΢��
    dzh_time_t(UINT64 uSecFrom1900, WORD uMsec, WORD uUsec);

    /*���뾫ȷ��΢���ʱ�䣬NULL��ʾ��ȡ��ǰʱ��,
     *��ʽΪYYYYMMDD24HMISS+3�ֽں���+3�ֽ�΢��,
     *��20121212124039020099����ʾ2012��12��12�� 12ʱ40��39�� 20���� 99΢��*/
    dzh_time_t(const char* pTime, short iZoneMinutes=0);

    //����һ���������ε�time_tʱ��
    dzh_time_t(time_t &tTime);

    //����һ��tm�ṹ��ΪNULL��ʾ��ȡ��ǰʱ��
    dzh_time_t(struct tm* pTime, short iZoneMinutes=0);

    //�ֱ���������ʱ���뼰�����΢�Сʱ��Ϊ24Сʱ��
    dzh_time_t(const char* pYear, const char* pMonth, const char* pDay
        , const char* pHour, const char* pMinute, const char* pSecond
        , const char* pMsec=NULL, const char* pUsec=NULL, short iZoneMinutes=0);

    //�ֱ���������ʱ���뼰�����΢�Сʱ��Ϊ24Сʱ��
    dzh_time_t(int y, int m, int d, int h, int minute, int s, int ms=0, int us=0, short iZoneMinutes=0);

    /*�����ǻ�ʱ�䰴��Ҫ���ʽ�����ַ�����ͬʱ֧��ʱ������
     *tTime��Ҫ��ʽ���Ĵ��ǻ�ʱ�䣬fmt����ʽ��������iZoneMinutes��ʱ����Ϣ
     *���ص���tTime+iZoneMinutes֮���ʱ���ַ���
     */
    const char* FormatTimeToString(const char* fmt=STR_TIME_FORMAT_1
        , char* pOutBuffer = NULL
        , int iBufferLen = 0
        , short iZoneMinutes=0)
    {
        return _FormatTimeToString(*this, fmt, pOutBuffer, iBufferLen, iZoneMinutes);
    }

    //�����ǻ�ʱ��ת����TDzhTimeNumStruct
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

	time_t ToTime();		//ת����time_t��ʽ���ᶪʧ�����΢��
	struct tm* ToGMTime(struct tm& tmTime);			//ת����GMT��tm��ʽ��tm_yday��ŵ��Ǻ����΢��ֵ�������Զ�ת����΢�룩
	struct tm* ToLocalTime(struct tm& tmTime, short iZoneMinutes=0);	//ת���ɱ���ʱ�䣬tm_yday��ŵ��Ǻ����΢��ֵ�������Զ�ת����΢�룩��tm_isdstֵΪ0
    UINT64 To64(){return _value;}               //UINT64�ڴ��
    UINT64 Second(){return secs;}               //��ȡ�� from 1900
    WORD   MilliSecond(){return msecs;}         //��ȡ����
    WORD   MicroSecond(){return usecs;}         //��ȡ΢��
    dzh_time_t&   SetValue(UINT64 v){_value = v; return *this;} //����ֵ
    dzh_time_t&   SetSecond(UINT64 v);          //������ from 1900
    dzh_time_t&   SetMilliSecond(WORD v);       //���ú���
    dzh_time_t&   SetMicroSecond(UINT32 v);     //����΢��
    dzh_time_t&   AddSecond(UINT64 v);          //�����
    dzh_time_t&   AddMilliSecond(WORD v);       //��Ӻ���
    dzh_time_t&   AddMicroSecond(UINT32 v);     //���΢��
    dzh_time_t&   SubSecond(UINT64 v);          //��ȥ��
    dzh_time_t&   SubMilliSecond(WORD v);       //��ȥ����
    dzh_time_t&   SubMicroSecond(UINT32 v);     //��ȥ΢��

public:
    //���ַ���ת���ɴ��ǻ�ʱ�䣬��ʽΪYYYYMMDD24HMISS+3�ֽں���+3�ֽ�΢��
    static dzh_time_t StringToTime(const char* pStr, short iZoneMinutes=0)
    {
        dzh_time_t t;
        t._StringToDzhTime(pStr, iZoneMinutes);
        return t;
    }

    //���ַ���ת���ɴ��ǻ�ʱ�䣬ָ����ʽ��������ʱ������
    static dzh_time_t StringToTime(const char* pStr, const char* fmt, short iZoneMinutes = 0);

    //��ȡ��ǰʱ�䣬��ʱ���Ǹ�������ʱ�䣬������ʱ��ת����,windows��ȷ�����룬linux��ȷ��΢��
    static dzh_time_t GetCurrentTime()
    {
        dzh_time_t t;
        t._GetCurrentTime();
        return t;
    }

    /*�����ǻ�ʱ��ת�����ַ���, ת���󷵻ص��Ǿ���iZoneMinutesƫ�Ƶı���ʱ��
    * ����ǻ�ʱ����2012��12��20�� 15��20��30�� 100���� 888΢�iZoneMinutes��0����
    * ���صĽ���Ǵ��ǻ�ʱ��+iZoneMinutes��Ϊ20121220232030100888����2012��12��20�� 23��20��30�� 100���� 888΢��
    */
    static const char* TimeToString(dzh_time_t tTime
        , char* pOutBuffer = NULL
        , int iBufferLen = 0
        , short iZoneMinutes=0)
    {
        return tTime._TimeToString(tTime, pOutBuffer, iBufferLen, iZoneMinutes);
    }

    /*�����ǻ�ʱ�䰴��Ҫ���ʽ�����ַ�����ͬʱ֧��ʱ������
     *tTime��Ҫ��ʽ���Ĵ��ǻ�ʱ�䣬fmt����ʽ��������iZoneMinutes��ʱ����Ϣ
     *���ص���tTime+iZoneMinutes֮���ʱ���ַ���
     */
    static const char* FormatTimeToString(dzh_time_t& tTime
        , const char* fmt=STR_TIME_FORMAT_1
        , char* pOutBuffer = NULL
        , int iBufferLen = 0
        , short iZoneMinutes=0)
    {
        return tTime._FormatTimeToString(tTime, fmt, pOutBuffer, iBufferLen, iZoneMinutes);
    }

    /*�����ǻ�ʱ��ƫ��ĳ��ʱ���õ��������εĴ��ǻ�ʱ��
    * tTime����Ҫ����ƫ�ƵĴ��ǻ�ʱ��
    * iZoneMinutes����Ҫ����ƫ�Ƶķ�����
    * ����ƫ�ƺ��ʱ�䣬���㷽����tTime-iZoneMinutes
    * �籱��ʱ��
    */
    //static dzh_time_t GMTDzhTime(dzh_time_t& tTime, short iZoneMinutes);

    //�����ǻ�ʱ��ת����TDzhTimeNumStruct
    static struct TDzhTimeNumStruct DzhTM(dzh_time_t& t);

    //���ǻ�TMʱ��ת���ɴ��ǻ�ʱ��
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
    //��ʽΪYYYYMMDD24HMISS+3�ֽں���+3�ֽ�΢��
    void _StringToDzhTime(const char* pStr, short iZoneMinutes=0);

    //��ȡ��ǰʱ��
    void _GetCurrentTime();

    //�����ǻ�ʱ��ת�����ַ���
    const char* _TimeToString(dzh_time_t& tTime
        , char* pOutBuffer = NULL
        , int iBufferLen = 0
        , short iZoneMinutes=0)
	{
		return _FormatTimeToString(tTime, STR_TIME_FORMAT_1, pOutBuffer, iBufferLen, iZoneMinutes);
	}

    //��time_tת���ɴ��ǻ�ʱ��
    void Time2DzhTime(time_t &tTime);

    //��tmʱ��ת���ɴ��ǻ�ʱ��
    void TM2DzhTime(struct tm* tmTime);

    //�����ǻ�ʱ�䰴��ĳ�ָ�ʽת�����ַ���ʱ�䣬ͬʱ֧��ʱ�����㣬���㷽�����ڴ��ǻ�ʱ���ϼ���ʱ��ƫ����
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
            mutable UINT64 secs:44;   //1900��1��1��00:00:00����ǰʱ�̵�����,time_t
            mutable UINT64 msecs:10;  //������
            mutable UINT64 usecs:10;  //΢����
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
