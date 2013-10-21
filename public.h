/********************************************************************
	created:	2013:1:31   9:12
	filename: 	public.h
	author:		fu.changyou
	
	purpose:	公共定义
*********************************************************************/
#ifndef _WDC_PUBLIC_H_
#define _WDC_PUBLIC_H_
#ifdef _MSC_VER
#include <Windows.h>
#else 
#include <pthread.h>
#endif

#if defined(_MSC_VER)
#define PATH_DELIMITATED_CHAR '\\'
#define PATH_DELIMITATED      "\\"
#else
#define PATH_DELIMITATED_CHAR '/'
#define PATH_DELIMITATED      "/"
#endif 

//获取文件内容，采用malloc动态分配内存来保存文件内容，调用者需要自己释放该内存
char* GetFileContent(const char* pFile, int& iFileLen);

//计算did模板文件的crc
unsigned int CalcFileCrc(const char* pFile);

/*求比某一值大的最小素数的算法*/
int GetPrimeNumber(const int nBase);

//分割字符串
struct CSplitString 
{
private:
    char* pBuffer;
    char  cSplit;
    char* pNow;
    char* pNext;
public:
    CSplitString(const char* pStr, char split)
    {
        if (NULL != pStr)
        {
            int iLen = strlen(pStr)+1;
            pBuffer = (char*)malloc(iLen);
            memcpy(pBuffer, pStr, iLen-1);
            pBuffer[iLen-1] = 0;
            pNow = pBuffer;
            pNext = strchr(pNow, split);
            if (NULL != pNext)
            {
                *pNext = '\0';
                ++pNext;
            }
        }
        else
        {
            pNow = NULL;
            pNext = NULL;
        }
        cSplit = split;
    }

    ~CSplitString()
    {
        if (NULL != pBuffer)
        {
            free(pBuffer);
            pBuffer = NULL;
        }
    }

    char* Next()
    {
        char* pRet = pNow;
        pNow = pNext;
        if (NULL != pNow)
        {
            pNext = strchr(pNow, cSplit);
            if (NULL != pNext)
            {
                *pNext = '\0';
                ++pNext;
            }
        }

        return pRet;
    }
};


//锁
class CMyLock 
{
#ifdef _MSC_VER
public:
    CMyLock(){InitializeCriticalSection(&m_Mutex);}
    ~CMyLock(){DeleteCriticalSection(&m_Mutex);}
    void Lock(){EnterCriticalSection(&m_Mutex);};
    void UnLock(){LeaveCriticalSection(&m_Mutex);};
private:
    CRITICAL_SECTION m_Mutex;
#else
public:
    CMyLock(){pthread_mutex_init(&m_Mutex, NULL);}
    ~CMyLock(){pthread_mutex_destroy(&m_Mutex);}
    void Lock(){pthread_mutex_lock(&m_Mutex);}
    void UnLock(){pthread_mutex_unlock(&m_Mutex);}
private:
    pthread_mutex_t m_Mutex;
#endif
};

#endif
