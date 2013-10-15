/********************************************************************
	created:	2012:12:12   12:52
	filename: 	dzh_time_t.cpp
	author:		fu.changyou
	
	purpose:	大智慧时间的实现
*********************************************************************/
#include "stdafx.h"
#include "dzh_time_t.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#endif

#pragma warning(disable:4996)

#ifndef snprintf
#define snprintf _snprintf
#endif
#define SAFE_ATOI(x) ((x) == NULL ? 0 : atoi((x)))
#define SAFE_STRCPY(x, y) (((x) != NULL && (y) != NULL) ? strcpy((x), (y)) : "")
#define SAFE_STRNCPY(x, y, z) (((x) != NULL && (y) != NULL) ? strncpy((x), (y), (z)) : "")
#define SAFE_MEMCPY(x, y, z) (((x) != NULL && (y) != NULL) ? memcpy((x), (y), (z)) : "")

//计算两个数中有多少个数能被某一个数整除
int CalcDivisbleNum(int iStart, int iEnd, int iBase)
{
    int iDiff = iEnd - iStart;
    if (iDiff % iBase == 0)
    {
        return  iDiff / iBase;
    }
    else
    {
        return iStart%iBase > iEnd%iBase ? iDiff/iBase+1 : iDiff/iBase;
    }
}

static BYTE day_tab[2][12]={{31,28,31,30,31,30,31,31,30,31,30,31},{31,29,31,30,31,30,31,31,30,31,30,31}}; 
#define DZH_TIME_STR_LEN    20 //YYYY MM DD 24H MI SS + 6
#define MIN_DZH_TIME_YEAR   1900
#define MAX_DZH_TIME_YEAR   557844 
#define MAX_MILLISECOND     999
#define MAX_MICROSECOND     999
#define WEEK_DAY_1900_1_1       1               //1900年1月1日为星期一
#define VALID_MONTH(x)      ((x)>0 && (x)<13)   //[1,12]
#define VALID_HOUR(x)       ((x)>=0 && (x)<24)  //[0,23]
#define VALID_MINUTE(x)     ((x)>=0 && (x)<=59) //[0,59]
#define VALID_SECOND(x)     ((x)>=0 && (x)<=59) //[0,59]
#define IS_LEAP_YEAR(x)     (((x)%4==0 && (x)%100!=0) || (x)%400==0)
#define CALC_LEAP_YEAR_NUM(x, y) (CalcDivisbleNum((x), (y)-1, 4) - CalcDivisbleNum((x), (y)-1, 100) + CalcDivisbleNum((x), (y)-1, 400)) //计算两个年份之间有多少个闰年,不包含结束年份[x, y)

#ifdef _WIN32
#define YEAR_SECOND         31536000i64   //365*24*3600
#define DAY_SECOND          86400i64
#define NANO_SECOND_BIAS_1601_1900    94354848000000000i64  //Number of 100 nanosecond units from 1/1/1601 to 1/1/1900
#define SECOND_BIAS_1601_1970   11644473600i64  //seconds from 1601/1/1 to 1970/1/1
#define SECOND_BIAS_1900_1970   2208988800i64   //seconds from 1900/1/1 to 1970/1/1
#define DAY_BIAS_1601_1970      134774i64       //days from 1601/1/1 t0 1970/1/1
#define DAY_BIAS_1900_1970      25567i64        //days from 1900/1/1 to 1970/1/1
#else
#define YEAR_SECOND         31536000ll   //365*24*3600
#define DAY_SECOND          86400ll
#define NANO_SECOND_BIAS_1601_1900    94354848000000000ll  //Number of 100 nanosecond units from 1/1/1601 to 1/1/1900
#define SECOND_BIAS_1601_1970   11644473600ll  //seconds from 1601/1/1 to 1970/1/1
#define SECOND_BIAS_1900_1970   2208988800ll   //seconds from 1900/1/1 to 1970/1/1
#define DAY_BIAS_1601_1970      134774ll       //days from 1601/1/1 t0 1970/1/1
#define DAY_BIAS_1900_1970      25567ll        //days from 1900/1/1 to 1970/1/1
#endif
//FT nt_time;
//返回1601年1月1日至今的100纳秒数
//GetSystemTimeAsFileTime( &(nt_time.ft_struct) );


//锁
class CDzhTimeLock 
{
#ifdef _WIN32
public:
    CDzhTimeLock(){InitializeCriticalSection(&m_Mutex);}
    ~CDzhTimeLock(){DeleteCriticalSection(&m_Mutex);}
    void Lock(){EnterCriticalSection(&m_Mutex);};
    void UnLock(){LeaveCriticalSection(&m_Mutex);};
private:
    CRITICAL_SECTION m_Mutex;
#else
public:
    CDzhTimeLock(){pthread_mutex_init(&m_Mutex, NULL);}
    ~CDzhTimeLock(){pthread_mutex_destroy(&m_Mutex);}
    void Lock(){pthread_mutex_lock(&m_Mutex);}
    void UnLock(){pthread_mutex_unlock(&m_Mutex);}
private:
    pthread_mutex_t m_Mutex;
#endif
};
CDzhTimeLock g_DzhTimeLockXXX;  //大智慧时间锁

struct TDzhTimeStrStruct 
{
    char sYear[5];
    char sMonth[3];
    char sDay[3];
    char sHour[3];
    char sMinute[3];
    char sSecond[3];
    char sMsec[4];
    char sUsec[4];
    TDzhTimeStrStruct(){memset(this, 0x0, sizeof(TDzhTimeStrStruct));}
};

//判断是否为数字
bool IsNumberStrForTime(const char* pStr)
{
    if (NULL == pStr)
    {
        return true;
    }

    char* p = (char*)pStr;
    while (*p != '\0')
    {
        if (*p < '0' || *p > '9')
        {
            return false;
        }
        ++p;
    }
    return true;
}

//判断是否为数字
bool IsDzhTimeStr(const char* pStr, TDzhTimeNumStruct& tNumDzhTime)
{
    char* p = (char*)pStr;
    int iLen = 0;
    while (*p != '\0')
    {
        if (*p < '0' || *p > '9')
        {
            return false;
        }
        ++p;
        ++iLen;
    }

    if (iLen != DZH_TIME_STR_LEN)
    {
        return false;
    }
    TDzhTimeStrStruct tDzhTime;
    SAFE_STRNCPY(tDzhTime.sYear, pStr, 4);
    SAFE_STRNCPY(tDzhTime.sMonth, pStr+4, 2);
    SAFE_STRNCPY(tDzhTime.sDay, pStr+6, 2);
    SAFE_STRNCPY(tDzhTime.sHour, pStr+8, 2);
    SAFE_STRNCPY(tDzhTime.sMinute, pStr+10, 2);
    SAFE_STRNCPY(tDzhTime.sSecond, pStr+12, 2);
    SAFE_STRNCPY(tDzhTime.sMsec, pStr+14, 3);
    SAFE_STRNCPY(tDzhTime.sUsec, pStr+17, 3);

    TDzhTimeNumStruct tTmpTime;
    tTmpTime.uYear = SAFE_ATOI(tDzhTime.sYear);
    tTmpTime.uMonth = SAFE_ATOI(tDzhTime.sMonth);
    tTmpTime.uDay = SAFE_ATOI(tDzhTime.sDay);
    tTmpTime.uHour = SAFE_ATOI(tDzhTime.sHour);
    tTmpTime.uMinute = SAFE_ATOI(tDzhTime.sMinute);
    tTmpTime.uSecond = SAFE_ATOI(tDzhTime.sSecond);
    tTmpTime.uMsec = SAFE_ATOI(tDzhTime.sMsec);
    tTmpTime.uUsec = SAFE_ATOI(tDzhTime.sUsec);

    bool bLeapYear = IS_LEAP_YEAR(tTmpTime.uYear);
    if (!VALID_MONTH(tTmpTime.uMonth) 
        || !VALID_HOUR(tTmpTime.uHour)
        || !VALID_MINUTE(tTmpTime.uMinute)
        || !VALID_SECOND(tTmpTime.uSecond)
        || tTmpTime.uMsec > MAX_MILLISECOND
        || tTmpTime.uUsec > MAX_MICROSECOND)
    {
        return false;
    }

    //判断天
    if (bLeapYear)
    {
        if (tTmpTime.uDay<=0 || tTmpTime.uDay > day_tab[1][tTmpTime.uMonth-1])
        {
            return false;
        }
    }
    else
    {
        if (tTmpTime.uDay<=0 || tTmpTime.uDay > day_tab[0][tTmpTime.uMonth-1])
        {
            return false;
        }
    }

    SAFE_MEMCPY(&tNumDzhTime, &tTmpTime, sizeof(TDzhTimeNumStruct));

    return true;
}

//大智慧TM时间转换成大智慧时间
dzh_time_t dzh_time_t::DzhTimeNumStruct2DzhTime(const struct TDzhTimeNumStruct& tTimeStruct)
{
    dzh_time_t dzhTime;

    bool bLeapYear = IS_LEAP_YEAR(tTimeStruct.uYear);
    if (tTimeStruct.uYear < MIN_DZH_TIME_YEAR
        || tTimeStruct.uYear > MAX_DZH_TIME_YEAR
        || !VALID_MONTH(tTimeStruct.uMonth) 
        || !VALID_HOUR(tTimeStruct.uHour)
        || !VALID_MINUTE(tTimeStruct.uMinute)
        || !VALID_SECOND(tTimeStruct.uSecond)
        || tTimeStruct.uMsec > MAX_MILLISECOND
        || tTimeStruct.uUsec > MAX_MICROSECOND)
    {
        return dzhTime;
    }

    //判断天
    if (bLeapYear)
    {
        if (tTimeStruct.uDay<=0 || tTimeStruct.uDay > day_tab[1][tTimeStruct.uMonth-1])
        {
            return dzhTime;
        }
    }
    else
    {
        if (tTimeStruct.uDay<=0 || tTimeStruct.uDay > day_tab[0][tTimeStruct.uMonth-1])
        {
            return dzhTime;
        }
    }
    int iDiffYear = tTimeStruct.uYear - MIN_DZH_TIME_YEAR;
    int iDays = 0;
    for (int i = 0; i < tTimeStruct.uMonth-1; i++)
    {
        iDays += (bLeapYear ? day_tab[1][i] : day_tab[0][i]);
    }
    iDays += tTimeStruct.uDay - 1;

    dzhTime.SetSecond(iDiffYear * YEAR_SECOND 
        + (CALC_LEAP_YEAR_NUM(MIN_DZH_TIME_YEAR, tTimeStruct.uYear) + iDays) * DAY_SECOND
        + tTimeStruct.uHour * 3600
        + tTimeStruct.uMinute * 60
        + tTimeStruct.uSecond);
    dzhTime.SetMilliSecond(tTimeStruct.uMsec);
    dzhTime.SetMicroSecond(tTimeStruct.uUsec);

    return dzhTime;
}

//传入一个dzh_time_t
dzh_time_t::dzh_time_t(const dzh_time_t& uDzhTime)
{
    this->_value = uDzhTime._value;
}

//构造一个空的大智慧时间
dzh_time_t::dzh_time_t()
{
    _value = 0;
}

//传入一个UINT64
dzh_time_t::dzh_time_t(UINT64 uDzhTime)
{
    _value = 0;
    int i = 0x01020304;
    char* p = (char*)&i;
    if (p[0] == 0x01)
    {
        SetSecond(uDzhTime>>20);
        SetMicroSecond(uDzhTime&0xFFFFF);
    }
    else
    {
        SetSecond(uDzhTime&0xFFFFFFFFFFF);
        SetMilliSecond((uDzhTime>>44)&0x3FF);
        SetMicroSecond((uDzhTime>>54));
    }
}

//分别传入整形表示的日期和时间，如dzh_time_t(20121221,235959),表示2012年12月21日23点59分59秒
dzh_time_t::dzh_time_t(UINT32 uDate, UINT32 uTime)
{
    _value = 0;
    TDzhTimeNumStruct tNumStruct;
    tNumStruct.uYear  = uDate / 10000;
    uDate %= 10000;
    tNumStruct.uMonth = uDate / 100;
    tNumStruct.uDay   = uDate % 100;
    tNumStruct.uHour  = uTime / 10000;
    uTime %= 10000;
    tNumStruct.uMinute = uTime / 100;
    tNumStruct.uSecond = uTime % 100;
    *this = DzhTimeNumStruct2DzhTime(tNumStruct);
}

//传入大智慧的秒、毫秒、微妙
dzh_time_t::dzh_time_t(UINT64 uSecFrom1900, WORD uMsec, WORD uUsec)
{
    _value = 0;
    if (uSecFrom1900 > MAX_DZH_TIME_SECOND)
    {
        _value = INVALID_DZH_TIME;
    }
    else
    {
        this->secs = uSecFrom1900;
        SetMicroSecond(uUsec);
        SetMilliSecond(uMsec);
    }
}

/*传入精确到微妙的时间，NULL表示获取当前时间, 
    *格式为YYYYMMDD24HMISS+3字节毫秒+3字节微妙,
    *如20121212124039020099，表示2012年12月12日 12时40分39秒 20毫秒 99微妙*/
dzh_time_t::dzh_time_t(const char* pTime/*=NULL*/, short iZoneMinutes/*=0*/)
{
    _value = 0;
    if (NULL == pTime)
    {
        _GetCurrentTime();
    }
    else 
    {
        _StringToDzhTime(pTime);
        SubSecond(iZoneMinutes*60);
    }
}

//传入一个time_t时间
dzh_time_t::dzh_time_t(time_t &tTime)
{
    _value = 0;
    Time2DzhTime(tTime);
}

//传入一个tm结构，为NULL表示获取当前时间
dzh_time_t::dzh_time_t(struct tm* pTime, short iZoneMinutes/*=0*/)
{
    _value = 0;
    TM2DzhTime(pTime);
    SubSecond(iZoneMinutes*60);
}

//分别传入年月日时分秒及毫秒和微妙，小时均为24小时制
dzh_time_t::dzh_time_t(const char* pYear, const char* pMonth, const char* pDay
    , const char* pHour, const char* pMinute, const char* pSecond
    , const char* pMsec/*=NULL*/, const char* pUsec/*=NULL*/, short iZoneMinutes/*=0*/)
{
    _value = 0;
    if (!IsNumberStrForTime(pYear)
        || !IsNumberStrForTime(pMonth)
        || !IsNumberStrForTime(pDay)
        || !IsNumberStrForTime(pHour)
        || !IsNumberStrForTime(pMinute)
        || !IsNumberStrForTime(pSecond)
        || !IsNumberStrForTime(pMsec)
        || !IsNumberStrForTime(pUsec))
    {
        _value = INVALID_DZH_TIME;
    }
    else
    {
        TDzhTimeNumStruct tTmpTime;
        tTmpTime.uYear = SAFE_ATOI(pYear);
        tTmpTime.uMonth = SAFE_ATOI(pMonth);
        tTmpTime.uDay = SAFE_ATOI(pDay);
        tTmpTime.uHour = SAFE_ATOI(pHour);
        tTmpTime.uMinute = SAFE_ATOI(pMinute);
        tTmpTime.uSecond = SAFE_ATOI(pSecond);
        tTmpTime.uMsec = SAFE_ATOI(pMsec);
        tTmpTime.uUsec = SAFE_ATOI(pUsec);
        *this = DzhTimeNumStruct2DzhTime(tTmpTime);
    }

    SubSecond(iZoneMinutes*60);
}

//分别传入年月日时分秒及毫秒和微妙，小时均为24小时制
dzh_time_t::dzh_time_t(int y, int m, int d, int h, int minute, int s, int ms/*=0*/, int us/*=0*/, short iZoneMinutes/*=0*/)
{
    _value = 0;
    TDzhTimeNumStruct tTmpTime;
    tTmpTime.uYear = y;
    tTmpTime.uMonth = m;
    tTmpTime.uDay = d;
    tTmpTime.uHour = h;
    tTmpTime.uMinute = minute;
    tTmpTime.uSecond = s;
    tTmpTime.uMsec = ms;
    tTmpTime.uUsec = us;

    *this = DzhTimeNumStruct2DzhTime(tTmpTime);
    SubSecond(iZoneMinutes*60);
}


//格式为YYYYMMDD24HMISS+3字节毫秒+3字节微妙
void dzh_time_t::_StringToDzhTime(const char* pStr, short iZoneMinutes/*=0*/)
{
    if (NULL == pStr)
    {
        _value = INVALID_DZH_TIME;
        return;
    }

    TDzhTimeNumStruct tNumDzhTime;
    if (!IsDzhTimeStr(pStr, tNumDzhTime) || tNumDzhTime.uYear < MIN_DZH_TIME_YEAR)
    {
        _value = INVALID_DZH_TIME;
        return;
    }

    *this = DzhTimeNumStruct2DzhTime(tNumDzhTime);
    SubSecond(iZoneMinutes*60);
}

//获取当前时间,windows精确到毫秒，linux精确到微秒
void dzh_time_t::_GetCurrentTime()
{
    TDzhTimeNumStruct tTimeStruct;
#ifdef _WIN32
    SYSTEMTIME tSysTime;
    GetSystemTime(&tSysTime);
    tTimeStruct.uYear   = tSysTime.wYear;
    tTimeStruct.uMonth  = tSysTime.wMonth;
    tTimeStruct.uDay    = tSysTime.wDay;
    tTimeStruct.uHour   = tSysTime.wHour;
    tTimeStruct.uMinute = tSysTime.wMinute;
    tTimeStruct.uSecond = tSysTime.wSecond;
    tTimeStruct.uMsec   = tSysTime.wMilliseconds;
#else
    struct timeval nowtimeval;
    time_t tTime = time(NULL);
    struct tm* tmTime = gmtime(&tTime);
    gettimeofday(&nowtimeval,0);
    tTimeStruct.uYear   = tmTime->tm_year + 1900;
    tTimeStruct.uMonth  = tmTime->tm_mon + 1;
    tTimeStruct.uDay    = tmTime->tm_mday;
    tTimeStruct.uHour   = tmTime->tm_hour;
    tTimeStruct.uMinute = tmTime->tm_min;
    tTimeStruct.uSecond = tmTime->tm_sec;
    tTimeStruct.uMsec   = nowtimeval.tv_usec / 1000;
    tTimeStruct.uUsec   = nowtimeval.tv_usec % 1000;
#endif
    *this = DzhTimeNumStruct2DzhTime(tTimeStruct);
}

//将time_t转换成大智慧时间
void dzh_time_t::Time2DzhTime(time_t &tTime)
{
    if (tTime < 0)
    {
        _value = INVALID_DZH_TIME;
    }
    else
    {
        secs = SECOND_BIAS_1900_1970 + tTime;
        msecs = 0;
        usecs = 0;
    }
}

//将tm时间转换成大智慧时间
void dzh_time_t::TM2DzhTime(struct tm* tmTime)    
{
    if (NULL == tmTime)
    {
        _value = INVALID_DZH_TIME;
        return;
    }

    msecs = usecs = 0;
	SetSecond(tmTime->tm_year * YEAR_SECOND 
		+ (CALC_LEAP_YEAR_NUM(MIN_DZH_TIME_YEAR, tmTime->tm_year+1900)+tmTime->tm_yday) * DAY_SECOND 
		+ tmTime->tm_hour*3600 
		+ tmTime->tm_min * 60
		+ tmTime->tm_sec);

    if (tmTime->tm_isdst > 0)
    {//夏令时
        SubSecond(3600);
    }
}

//将大智慧时间按照某种格式转换成字符串时间，同时支持时区计算，计算方法是在大智慧时间上加上时区偏移量
const char* dzh_time_t::_FormatTimeToString(dzh_time_t& tTime
    , const char* fmt/* =STR_TIME_FORMAT_1 */
    , char* pOutBuffer/* = NULL*/
    , int iBufferLen/* = 0*/
    , short iZoneMinutes/* =0 */)
{
    dzh_time_t t = tTime;
    if (t._value == INVALID_DZH_TIME)
    {
        if (NULL != pOutBuffer && iBufferLen > 0)
        {
            memset(pOutBuffer, 0x0, iBufferLen);
        }
        return NULL;
    }

    //先做时区偏移计算
    t.secs += iZoneMinutes * 60;
    TDzhTimeStrStruct sTime;
    int iDays = (int)(t.secs / DAY_SECOND);  
    int iSec = t.secs % DAY_SECOND;
    int iYear = 1900;
    int iMonth = 1;
    int iHour = iSec / 3600;
    iSec %= 3600;
    int iMinute = iSec / 60;
    iSec %= 60;
    char sUsec[7] = {0};
    sprintf(sUsec, "%03llu%03llu", t.msecs, t.usecs);

    //计算年分
    int iYearDays = 365;
    while (1)
    {
        iYearDays = 365;
        if (IS_LEAP_YEAR(iYear))
        {
            iYearDays++;
        }

        if (iDays >= iYearDays)
        {
            iYear++;
            iDays -= iYearDays;
            continue;
        }
        break;
    }

    //计算月份
    int iTmp = IS_LEAP_YEAR(iYear) ? 1 : 0;
    while (1)
    {
        if (day_tab[iTmp][iMonth-1] >= iDays)
        {
            break;
        }

        iDays -= day_tab[iTmp][iMonth-1];
        iMonth++;
    }
    iDays++;

    char* pFormat = (char*)fmt;
    bool bTagFind = false;
    bool b12HourFormat = false;
    bool bSuccess = true;
    char* pFormatStart = NULL;

    static char sStrTime[40];
    int iFreeSpace = sizeof(sStrTime)-1;
    char* pCurr = sStrTime;
    char* pRet = sStrTime;
    if (NULL != pOutBuffer && iBufferLen > 0)
    {
        memset(pOutBuffer, 0x0, iBufferLen);
        iFreeSpace = iBufferLen - 1;
        pCurr = pOutBuffer;
        pRet = pOutBuffer;
    }
    else
    {
        //加锁，线程安全
        g_DzhTimeLockXXX.Lock();
        memset(sStrTime, 0x0, sizeof(sStrTime));
    }
    char sSuffix[3] = {0}; //时间后缀，AM或者PM

    //格式化时间
    while (*pFormat != '\0')
    {
        switch (*pFormat)
        {
        case '%':
            {
                if (bTagFind)
                {//格式错误
                    bSuccess = false;
                    goto FORMAT_TIME_END;
                }
                bTagFind = true;
                pFormatStart = pFormat;
                break;
            }
        case 'Y':
            {
                if (!bTagFind || iFreeSpace < 4)
                {//格式错误
                    bSuccess = false;
                    goto FORMAT_TIME_END;
                }
                sprintf(pCurr, "%04d", iYear);
                pCurr += 4;
                iFreeSpace -= 4;
                bTagFind = false;
                break;
            }
        case 'y':
            {
                if (!bTagFind || iFreeSpace < 2)
                {//格式错误
                    bSuccess = false;
                    goto FORMAT_TIME_END;
                }
                char sTmp[5] = {0};
                sprintf(sTmp, "%04d", iYear);
                SAFE_STRNCPY(pCurr, sTmp+2, 2);
                pCurr += 2;
                iFreeSpace -= 2;
                bTagFind = false;
                break;
            }
        case 'm':
            {
                if (!bTagFind || iFreeSpace < 2)
                {//格式错误
                    bSuccess = false;
                    goto FORMAT_TIME_END;
                }
                sprintf(pCurr, "%02d", iMonth);
                pCurr += 2;
                iFreeSpace -= 2;
                bTagFind = false;
                break;
            }
        case 'd':
            {
                if (!bTagFind || iFreeSpace < 2)
                {//格式错误
                    bSuccess = false;
                    goto FORMAT_TIME_END;
                }
                sprintf(pCurr, "%02d", iDays);
                pCurr += 2;
                iFreeSpace -= 2;
                bTagFind = false;
                break;
            }
        case 'H':
            {
                if (!bTagFind || iFreeSpace < 2)
                {//格式错误
                    bSuccess = false;
                    goto FORMAT_TIME_END;
                }
                sprintf(pCurr, "%02d", iHour);
                pCurr += 2;
                iFreeSpace -= 2;
                bTagFind = false;
                break;
            }
        case 'h':
            {
                if (!bTagFind || iFreeSpace < 4)
                {//格式错误
                    bSuccess = false;
                    goto FORMAT_TIME_END;
                }
                b12HourFormat = true;
                sprintf(pCurr, "%02d", (iHour >= 12 ? iHour-12 : iHour));
                pCurr += 2;
                iFreeSpace -= 4;
                bTagFind = false;
                if (iHour >= 12)
                {
                    strcpy(sSuffix, "PM");
                }
                else
                {
                    strcpy(sSuffix, "AM");
                }
                break;
            }
        case 'M':
            {
                if (!bTagFind || iFreeSpace < 2)
                {//格式错误
                    bSuccess = false;
                    goto FORMAT_TIME_END;
                }
                sprintf(pCurr, "%02d", iMinute);
                pCurr += 2;
                iFreeSpace -= 2;
                bTagFind = false;
                break;
            }
        case 'S':
            {
                if (!bTagFind || iFreeSpace < 2)
                {//格式错误
                    bSuccess = false;
                    goto FORMAT_TIME_END;
                }
                sprintf(pCurr, "%02d", iSec);
                pCurr += 2;
                iFreeSpace -= 2;
                bTagFind = false;
                break;
            }
        case 'u':
            {
                int iLen = pFormat - pFormatStart;

                if (iLen != 3 && iLen != 1)
                {//格式错误
                    bSuccess = false;
                    goto FORMAT_TIME_END;
                }

                int iDigit = SAFE_ATOI(pFormatStart+1);
                if (iLen == 1)
                {//%u的形式
                    iDigit = 6;
                }
                if (iDigit > 6 || iDigit <= 0)
                {//格式错误
                    bSuccess = false;
                    goto FORMAT_TIME_END;
                }

                if (iFreeSpace < iDigit)
                {//格式错误
                    bSuccess = false;
                    goto FORMAT_TIME_END;
                }
                pCurr -= (iLen - 1);
                SAFE_STRNCPY(pCurr, sUsec, iDigit);
                pCurr += iDigit;
                iFreeSpace -= iDigit;
                bTagFind = false;
                break;
            }
        default:
            {
                if ((bTagFind && (strlen(pFormat) < 3 || pFormat[2] != 'u')) || iFreeSpace < 1)
                {//格式错误
                    bSuccess = false;
                    goto FORMAT_TIME_END;
                }
                bTagFind = false;
                *pCurr++ = *pFormat;
                iFreeSpace -= 1;
                break;
            }
        }
        pFormat++;
    }

FORMAT_TIME_END:
    if (NULL == pOutBuffer || iBufferLen <= 0)
    {
        strcat(pRet, sSuffix);
        g_DzhTimeLockXXX.UnLock();
    }
    else
    {
        strcat(pRet, sSuffix);
    }

    if (bSuccess)
    {
        return pRet;
    }
    return NULL;
}

//将字符串转换成大智慧时间，指定格式，并进行时区计算
dzh_time_t dzh_time_t::StringToTime(const char* pStr, const char* fmt, short iZoneMinutes/* = 0*/)
{
    dzh_time_t t;
    if (NULL == pStr)
    {
        return t;
    }

    char* pTime = (char*)pStr;
    char* pFmt = (char*)fmt;
    char* pStart = NULL;
    bool bTagFind = false;
    bool bSuccess = true;
    TDzhTimeStrStruct tTimeStruct;
    int iUnDealLen = strlen(pTime);
    while (*pFmt != '\0' && iUnDealLen > 0)
    {
        switch (*pFmt)
        {
        case '%':
            {
                if (bTagFind)
                {//格式错误
                    bSuccess = false;
                    goto FORMAT_TIME_END;
                }
                pStart = pFmt;
                bTagFind = true;
                break;
            }
            case 'Y':
                {
                    if (!bTagFind || iUnDealLen < 4 || tTimeStruct.sYear[0] != '\0')
                    {//格式错误
                        bSuccess = false;
                        goto FORMAT_TIME_END;
                    }
                    SAFE_STRNCPY(tTimeStruct.sYear, pTime, 4);
                    if (!IsNumberStrForTime(tTimeStruct.sYear))
                    {//格式错误
                        bSuccess = false;
                        goto FORMAT_TIME_END;
                    }
                    pTime += 4;
                    iUnDealLen -= 4;
                    bTagFind = false;
                    break;
                }
            case 'y':
                {
                    if (!bTagFind || iUnDealLen < 2 || tTimeStruct.sYear[0] != '\0')
                    {//格式错误
                        bSuccess = false;
                        goto FORMAT_TIME_END;
                    }
                    SAFE_STRCPY(tTimeStruct.sYear, "20");
                    SAFE_STRNCPY(tTimeStruct.sYear+2,  pTime, 2);
                    if (!IsNumberStrForTime(tTimeStruct.sYear))
                    {//格式错误
                        bSuccess = false;
                        goto FORMAT_TIME_END;
                    }
                    pTime += 2;
                    iUnDealLen -= 2;
                    bTagFind = false;
                    break;
                }
            case 'm':
                {
                    if (!bTagFind || iUnDealLen < 2 || tTimeStruct.sMonth[0] != '\0')
                    {//格式错误
                        bSuccess = false;
                        goto FORMAT_TIME_END;
                    }
                    SAFE_STRNCPY(tTimeStruct.sMonth,  pTime, 2);
                    if (!IsNumberStrForTime(tTimeStruct.sMonth))
                    {//格式错误
                        bSuccess = false;
                        goto FORMAT_TIME_END;
                    }
                    pTime += 2;
                    iUnDealLen -= 2;
                    bTagFind = false;
                    break;
                }
            case 'd':
                {
                    if (!bTagFind || iUnDealLen < 2 || tTimeStruct.sDay[0] != '\0')
                    {//格式错误
                        bSuccess = false;
                        goto FORMAT_TIME_END;
                    }
                    SAFE_STRNCPY(tTimeStruct.sDay,  pTime, 2);
                    if (!IsNumberStrForTime(tTimeStruct.sDay))
                    {//格式错误
                        bSuccess = false;
                        goto FORMAT_TIME_END;
                    }
                    pTime += 2;
                    iUnDealLen -= 2;
                    bTagFind = false;
                    break;
                }
            case 'H':
                {
                    if (!bTagFind || iUnDealLen < 2 || tTimeStruct.sHour[0] != '\0')
                    {//格式错误
                        bSuccess = false;
                        goto FORMAT_TIME_END;
                    }
                    SAFE_STRNCPY(tTimeStruct.sHour,  pTime, 2);
                    if (!IsNumberStrForTime(tTimeStruct.sHour))
                    {//格式错误
                        bSuccess = false;
                        goto FORMAT_TIME_END;
                    }
                    pTime += 2;
                    iUnDealLen -= 2;
                    bTagFind = false;
                    break;
                }
            case 'h':
                {
                    if (!bTagFind || iUnDealLen < 2 || tTimeStruct.sHour[0] != '\0')
                    {//格式错误
                        bSuccess = false;
                        goto FORMAT_TIME_END;
                    }
                    char sTmp[3] = {0};
                    SAFE_STRNCPY(sTmp,  pTime, 2);
                    if (!IsNumberStrForTime(sTmp) || SAFE_ATOI(sTmp) > 11)
                    {//格式错误
                        bSuccess = false;
                        goto FORMAT_TIME_END;
                    }
                    if (strcmp(pTime+iUnDealLen-2, "AM") == 0)
                    {
                        SAFE_STRCPY(tTimeStruct.sHour, sTmp);
                    }
                    else if (strcmp(pTime+iUnDealLen-2, "PM") == 0)
                    {
                        sprintf(tTimeStruct.sHour, "%02d", SAFE_ATOI(sTmp) + 12);
                    }
                    else
                    {//格式错误
                        bSuccess = false;
                        goto FORMAT_TIME_END;
                    }
                    pTime += 2;
                    iUnDealLen -= 4;
                    bTagFind = false;
                    break;
                }
            case 'M':
                {
                    if (!bTagFind || iUnDealLen < 2 || tTimeStruct.sMinute[0] != '\0')
                    {//格式错误
                        bSuccess = false;
                        goto FORMAT_TIME_END;
                    }
                    SAFE_STRNCPY(tTimeStruct.sMinute,  pTime, 2);
                    if (!IsNumberStrForTime(tTimeStruct.sMinute))
                    {//格式错误
                        bSuccess = false;
                        goto FORMAT_TIME_END;
                    }
                    pTime += 2;
                    iUnDealLen -= 2;
                    bTagFind = false;
                    break;
                }
            case 'S':
                {
                    if (!bTagFind || iUnDealLen < 2 || tTimeStruct.sSecond[0] != '\0')
                    {//格式错误
                        bSuccess = false;
                        goto FORMAT_TIME_END;
                    }
                    SAFE_STRNCPY(tTimeStruct.sSecond,  pTime, 2);
                    if (!IsNumberStrForTime(tTimeStruct.sSecond))
                    {//格式错误
                        bSuccess = false;
                        goto FORMAT_TIME_END;
                    }
                    pTime += 2;
                    iUnDealLen -= 2;
                    bTagFind = false;
                    break;
                }
            case 'u':
                {
                    int iLen = pFmt - pStart;
                    if ((iLen != 3 && iLen != 1) || tTimeStruct.sUsec[0] != '\0' || tTimeStruct.sMsec[0] != '\0')
                    {//格式错误
                        bSuccess = false;
                        goto FORMAT_TIME_END;
                    }

                    char sTmp[7] = {0};
                    if (iLen == 1)
                    {//%u的情况
                        for (iLen = 0; iLen < 6 && iLen < iUnDealLen; iLen++)
                        {
                            if (pTime[iLen] >= '0' && pTime[iLen]<='9')
                            {
                                sTmp[iLen] = pTime[iLen];
                            }
                        }
                    }
                    else
                    {//%0[1-6]u的情况
                        char sLen[3] = {0};
                        SAFE_STRNCPY(sLen, pStart+1, 2);
                        if (!IsNumberStrForTime(sLen))
                        {//格式错误
                            bSuccess = false;
                            goto FORMAT_TIME_END;
                        }
                        iLen = SAFE_ATOI(sLen);
                        if (iLen < 1 || iLen > 6)
                        {//格式错误
                            bSuccess = false;
                            goto FORMAT_TIME_END;
                        }

                        pTime -= 2;
                        iUnDealLen += 2;
                        SAFE_STRNCPY(sTmp, pTime, iLen);
                        if (!IsNumberStrForTime(sTmp))
                        {//格式错误
                            bSuccess = false;
                            goto FORMAT_TIME_END;
                        }
                    }

                    pTime += iLen;
                    iUnDealLen -= iLen;
                    int us = SAFE_ATOI(sTmp);
                    for (; iLen < 6; iLen++)
                    {
                        us *= 10;
                    }
                    sprintf(tTimeStruct.sMsec, "%03d", us/1000);
                    sprintf(tTimeStruct.sUsec, "%03d", us%1000);
                    bTagFind = false;
                    break;
                }
            default:
                {
                    if (bTagFind && (iUnDealLen <3 || pFmt[2] != 'u'))
                    {//格式错误
                        bSuccess = false;
                        goto FORMAT_TIME_END;
                    }
                    bTagFind = false;
                    pTime++;
                    iUnDealLen--;
                    break;
                }
        }
		pFmt++;
    }

FORMAT_TIME_END:
    if (!bSuccess)
    {
        t._value = INVALID_DZH_TIME;
        return t;
    }

    TDzhTimeNumStruct tTmpTime;
    tTmpTime.uYear = SAFE_ATOI(tTimeStruct.sYear);
    tTmpTime.uMonth = SAFE_ATOI(tTimeStruct.sMonth);
    tTmpTime.uDay = SAFE_ATOI(tTimeStruct.sDay);
    tTmpTime.uHour = SAFE_ATOI(tTimeStruct.sHour);
    tTmpTime.uMinute = SAFE_ATOI(tTimeStruct.sMinute);
    tTmpTime.uSecond = SAFE_ATOI(tTimeStruct.sSecond);
    tTmpTime.uMsec = SAFE_ATOI(tTimeStruct.sMsec);
    tTmpTime.uUsec = SAFE_ATOI(tTimeStruct.sUsec);

    return DzhTimeNumStruct2DzhTime(tTmpTime).SubSecond(iZoneMinutes * 60);
}

//将大智慧时间转换成TDzhTimeNumStruct
struct TDzhTimeNumStruct dzh_time_t::DzhTM(dzh_time_t& t)
{
    return t.DzhTM();
}

//转换成time_t格式，会丢失毫秒和微秒
time_t dzh_time_t::ToTime()
{
    if (_value == INVALID_DZH_TIME)
    {
        return -1;
    }

	return secs - SECOND_BIAS_1900_1970; 
}

//转换成GMT的tm格式， tm_yday存放的是毫秒和微秒值（毫秒自动转换成微秒）
struct tm* dzh_time_t::ToGMTime(struct tm& tmTime)
{
	memset(&tmTime, 0x0, sizeof(tm));
    if (_value == INVALID_DZH_TIME)
    {
        return &tmTime;
    }

	TDzhTimeNumStruct t = DzhTM();
	tmTime.tm_year = t.uYear - 1900;
	tmTime.tm_mon  = t.uMonth - 1;
	tmTime.tm_mday = t.uDay;
	tmTime.tm_hour = t.uHour;
	tmTime.tm_min  = t.uMinute;
	tmTime.tm_sec  = t.uSecond;
	tmTime.tm_yday = t.uMsec * 1000 + t.uUsec;
    tmTime.tm_wday = t.uWeekDay;
	return &tmTime;
}

//转换成本地时间，tm_yday存放的是毫秒和微秒值（毫秒自动转换成微秒）,忽略夏令时
struct tm* dzh_time_t::ToLocalTime(struct tm& tmTime, short iZoneMinutes/*=0*/)
{
	memset(&tmTime, 0x0, sizeof(tm));
    if (_value == INVALID_DZH_TIME)
    {
        return &tmTime;
    }
	secs += iZoneMinutes * 60;
	TDzhTimeNumStruct t = DzhTM();
	tmTime.tm_year = t.uYear - 1900;
	tmTime.tm_mon  = t.uMonth - 1;
	tmTime.tm_mday = t.uDay;
	tmTime.tm_hour = t.uHour;
	tmTime.tm_min  = t.uMinute;
	tmTime.tm_sec  = t.uSecond;
    tmTime.tm_yday = t.uMsec * 1000 + t.uUsec;
    tmTime.tm_wday = t.uWeekDay;
	return &tmTime;
}

//将大智慧时间转换成TDzhTimeNumStruct
struct TDzhTimeNumStruct dzh_time_t::DzhTM()
{
    TDzhTimeNumStruct tNumStruct;
    if (_value == INVALID_DZH_TIME)
    {
        return tNumStruct;
    }

    int iDays = (int)(secs / DAY_SECOND);  
    int iSec = secs % DAY_SECOND;
    int iYear = 1900;
    int iMonth = 1;
    int iHour = iSec / 3600;
    iSec %= 3600;
    int iMinute = iSec / 60;
    iSec %= 60;
    tNumStruct.uWeekDay = (iDays + WEEK_DAY_1900_1_1) % 7;

    //计算年分
    int iYearDays = 365;
    while (1)
    {
        iYearDays = 365;
        if (IS_LEAP_YEAR(iYear))
        {
            iYearDays++;
        }

        if (iDays >= iYearDays)
        {
            iYear++;
            iDays -= iYearDays;
            continue;
        }
        break;
    }

    //计算月份
    int iTmp = IS_LEAP_YEAR(iYear) ? 1 : 0;
    while (1)
    {
        if (day_tab[iTmp][iMonth-1] >= iDays)
        {
            break;
        }

        iDays -= day_tab[iTmp][iMonth-1];
        iMonth++;
    }
    iDays++;

    tNumStruct.uYear = iYear;
    tNumStruct.uMonth = iMonth;
    tNumStruct.uDay = iDays;
    tNumStruct.uHour = iHour;
    tNumStruct.uMinute = iMinute;
    tNumStruct.uSecond = iSec;
    tNumStruct.uMsec = msecs;
    tNumStruct.uUsec = usecs;

    return tNumStruct;
}

dzh_time_t&   dzh_time_t::SetSecond(UINT64 v)
{
    if (v > MAX_DZH_TIME_SECOND)
    {
        _value = INVALID_DZH_TIME;
    }
    else
    {
        secs = v;
    }
    return *this;
}
dzh_time_t&   dzh_time_t::SetMilliSecond(WORD v)
{
    msecs = v % 1000;
    SetSecond(secs + v/1000);
    return *this;
}
dzh_time_t&   dzh_time_t::SetMicroSecond(UINT32 v)
{
    usecs = v % 1000;
    SetMilliSecond((WORD)(v/1000 + msecs));
    return *this;
}

dzh_time_t& dzh_time_t::AddSecond(UINT64 v)
{
    if (_value == INVALID_DZH_TIME)
    {
        _value = 0;
        return SetSecond(v);
    }
    SetSecond(secs + v);
    return *this;
}

dzh_time_t& dzh_time_t::AddMilliSecond(WORD v)
{
    if (_value == INVALID_DZH_TIME)
    {
        _value = 0;
        return SetMilliSecond(v);
    }

    SetMilliSecond(msecs + v);
    return *this;
}

dzh_time_t& dzh_time_t::AddMicroSecond(UINT32 v)
{
    if (_value == INVALID_DZH_TIME)
    {
        _value = 0;
        return SetMicroSecond(v);
    }
    SetMicroSecond(usecs + v);
    return *this;
}

dzh_time_t& dzh_time_t::SubSecond(UINT64 v)
{
    if (_value == INVALID_DZH_TIME)
    {
        return *this;
    }

    if (secs < v)
    {
        _value = INVALID_DZH_TIME;
    }
	else
	{
		secs -= v;
	}
    return *this;
}

dzh_time_t& dzh_time_t::SubMilliSecond(WORD v)
{
    if (_value == INVALID_DZH_TIME)
    {
        return *this;
    }

    if (msecs < v%1000)
    {
        msecs -= (v%1000) - 1000;
        SubSecond(1 + v/1000);
    }
    else
    {
        msecs -= v % 1000;
        SubSecond(v/1000);
    }
    return *this;
}

dzh_time_t& dzh_time_t::SubMicroSecond(UINT32 v)
{
    if (_value == INVALID_DZH_TIME)
    {
        return *this;
    }

    if (usecs < v%1000)
    {
        usecs -= (v%1000) - 1000;
        SubMilliSecond(1 + v/1000);
    }
    else
    {
        usecs -= v % 1000;
        SubMilliSecond(v/1000);
    }
    return *this;
}

dzh_time_t dzh_time_t::operator+(const dzh_time_t& t)
{
    if (t == INVALID_DZH_TIME)
    {
        return *this;
    }
    
    if (*this == INVALID_DZH_TIME)
    {
        return t;
    }

	dzh_time_t t1(*this);
    t1.AddMicroSecond(t.usecs);
    if (t1._value != INVALID_DZH_TIME)
    {
        t1.AddMilliSecond(t.msecs);
        if (t1._value != INVALID_DZH_TIME)
        {
            t1.AddSecond(t.secs);
        }
    }
    return t1;
}

dzh_time_t dzh_time_t::operator-(const dzh_time_t& t)
{
    if (t == INVALID_DZH_TIME || *this == INVALID_DZH_TIME)
    {
        return *this;
    }

	dzh_time_t t1(*this);
    t1.SubMicroSecond(t.usecs);
    if (t1._value != INVALID_DZH_TIME)
    {
        t1.SubMilliSecond(t.msecs);
        if (t1._value != INVALID_DZH_TIME)
        {
            t1.SubSecond(t.secs);
        }
    }
    return t1;
}

dzh_time_t dzh_time_t::operator+(const UINT64& t)
{
	dzh_time_t tTime = t;
    return *this + tTime;
}

dzh_time_t dzh_time_t::operator-(const UINT64& t)
{
    dzh_time_t tTime = t;
    return *this - tTime;
}

const dzh_time_t& dzh_time_t::operator+=(const dzh_time_t& t)
{
    *this = *this + t;
    return *this;
}

const dzh_time_t& dzh_time_t::operator-=(const dzh_time_t& t)
{
    *this = *this - t;
    return *this;
}

const dzh_time_t& dzh_time_t::operator+=(const UINT64& t)
{
    *this = *this + t;
    return *this;
}

const dzh_time_t& dzh_time_t::operator-=(const UINT64& t)
{
    *this = *this - t;
    return *this;
}

const dzh_time_t& dzh_time_t::operator=(const dzh_time_t& t) const
{
    _value = t._value;
    return *this;
}

const dzh_time_t& dzh_time_t::operator=(const UINT64& t) const
{
    dzh_time_t tTime(t);
    *this = tTime;
    return *this;
}

bool dzh_time_t::operator==(const dzh_time_t& t) const
{
    return this->_value == t._value;
}

bool dzh_time_t::operator!=(const dzh_time_t& t) const
{
    return this->_value != t._value;
}

bool dzh_time_t::operator>=(const dzh_time_t& t) const
{
    return *this > t || *this == t;
}
bool dzh_time_t::operator<=(const dzh_time_t& t) const
{
    return *this < t || *this == t;
}

bool dzh_time_t::operator>(const dzh_time_t& t) const
{
    //无效时间跟有效时间比较，有效时间为大
    if (this->_value == INVALID_DZH_TIME && t._value != INVALID_DZH_TIME)
    {
        return false;
    }

    if (this->_value != INVALID_DZH_TIME && t._value == INVALID_DZH_TIME)
    {
        return true;
    }

    if (this->secs == t.secs)
    {
        if (this->msecs == t.msecs)
        {
            return this->usecs > t.usecs;
        }
        return this->msecs > t.msecs;
    }
    return this->secs > t.secs;
}

bool dzh_time_t::operator<(const dzh_time_t& t) const
{
    //无效时间跟有效时间比较，有效时间为大
    if (this->_value == INVALID_DZH_TIME && t._value != INVALID_DZH_TIME)
    {
        return true;
    }

    if (this->_value != INVALID_DZH_TIME && t._value == INVALID_DZH_TIME)
    {
        return false;
    }

    if (this->secs == t.secs)
    {
        if (this->msecs == t.msecs)
        {
            return this->usecs < t.usecs;
        }
        return this->msecs < t.msecs;
    }
    return this->secs < t.secs;
}

bool dzh_time_t::operator==(const UINT64& t) const
{
    return *this == dzh_time_t(t);
}

bool dzh_time_t::operator!=(const UINT64& t) const
{
    return *this != dzh_time_t(t);
}

bool dzh_time_t::operator>(const UINT64& t) const
{
    return *this > dzh_time_t(t);
}

bool dzh_time_t::operator<(const UINT64& t) const
{
    return *this < dzh_time_t(t);
}

bool dzh_time_t::operator>=(const UINT64& t) const
{
    return *this > dzh_time_t(t) || *this == dzh_time_t(t);
}

bool dzh_time_t::operator<=(const UINT64& t) const
{
    return *this < dzh_time_t(t) || *this == dzh_time_t(t);
}