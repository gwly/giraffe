/********************************************************************
	created:	2013:1:31   9:14
	filename: 	public.cpp
	author:		fu.changyou
	
	purpose:	
*********************************************************************/
#include "stdafx.h"
#include "public.h"
#include "dscomapi.h"
#include <math.h>
#ifdef _MSC_VER
#include <io.h>
#else
#include <unistd.h>
#endif

//获取文件内容，采用malloc动态分配内存来保存文件内容，调用者需要自己释放该内存
char* GetFileContent(const char* pFile, int& iFileLen)
{
    iFileLen = 0;
    if (NULL == pFile || access(pFile, 0x0) != 0)
    {
        return NULL;
    }

    FILE* fp = fopen(pFile, "rb");
    if (NULL == fp)
    {// file can't open
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    iFileLen = ftell(fp);
    if (iFileLen <= 0)
    {//content is null
        fclose(fp);
        iFileLen = 0;
        return NULL;
    }

    char* pBuffer = (char*)malloc(iFileLen+1);
    if (NULL == pBuffer)
    {//no memory
        iFileLen = 0;
        fclose(fp);
        return NULL;
    }

    fseek(fp, 0, SEEK_SET);
    memset(pBuffer, 0x0, iFileLen+1);
    if (fread(pBuffer, iFileLen, 1, fp) != 1)
    {//read file failed
        iFileLen = 0;
        fclose(fp);
        free(pBuffer);
        return NULL;
    }

    //success
    fclose(fp);
    fp = NULL;
    return pBuffer;
}


//计算did模板文件的crc
unsigned int CalcFileCrc(const char* pFile)
{
    int iFileLen = 0;
    char* pContent = GetFileContent(pFile, iFileLen);
    if (NULL != pContent && iFileLen > 0)
    {
        unsigned int crc = CDidStructApi::ds_cal_crc(pContent, iFileLen);
        free(pContent);
        pContent = NULL;
        return crc;
    }
    return -1;
}

/*求比某一值大的最小素数的算法*/
int GetPrimeNumber(const int nBase)
{
    int PrimeNumber=0, i,nLimit,iSqrt;

    if(nBase <= 9)
        return 9;

    nLimit = nBase;
    if ( nBase % 2 == 0 )
        nLimit ++;

    for(PrimeNumber = nLimit; 1; PrimeNumber+=2)
    {
        iSqrt = (int)sqrt((double)PrimeNumber);
        for ( i = 3; i<= iSqrt; i+=2 )
        {
            if((0 == PrimeNumber % i))
                break;/*nPrimeNumber不是素数*/
        }

        if(i > iSqrt)
            break;/*素数已经找到,PrimeNumber肯定是素数*/
    }

    return PrimeNumber;
}

