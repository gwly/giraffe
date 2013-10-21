#ifndef _GENERAL_COMPRESS_H_INCLUDE
#define _GENERAL_COMPRESS_H_INCLUDE

#include "datacollect.h"
#include "bitstream.h"

struct SVRNetHead;

#pragma pack(push,1)

//压缩扩展静态
int CompressStaticEx(const STK_STATICEx* pStatic,int nNum,CBitStream& stream);
int ExpandStaticEx(CBitStream& stream,STK_STATICEx* pBuf,int& nBufSize);		//nBufSize表示pBuf的个数,如果缓冲区不够，则返回-1，nBufSize带回需要的数量

//压缩港股动态
int CompressHKDyna(const STK_HKDYNA* pHKDyna,int nNum,CBitStream& stream);
int ExpandHKDyna(CBitStream& stream,STK_HKDYNA* pBuf,int& nBufSize);				//nBufSize表示pBuf的个数,如果缓冲区不够，则返回-1，nBufSize带回需要的数量

//压缩通用数据
struct CPS_GENERAL_HEAD
{
	WORD	m_wDataID;			//数据ID
	WORD	m_wMarket;			//市场
	DWORD	m_dwDataAttr;		//数据属性
	short	m_nDataSize;		//每个数据的长度,对于变长数据,表示每个数据的最大值(0表示没有最大值，任意长度)
	UINT24	m_nTotalLen;		//包含本数据头在内的数据总长度
	WORD	m_wNum;
};

int CompressGeneralData(DC_GENERAL* pData,BYTE* pBuf,int nBufSize);		//返回压缩数据长度，0表示不能压缩或压缩失败，-1表示缓冲区长度不够
int ExpandGeneralData(const BYTE* pCpsData,int nDataLen,DC_GENERAL* pBuf,int& nBufSize);		//nBufSize表示pBuf字节数，返回解压缩后的字节数，0表示失败，-1表示缓冲区不够,nBufSize带回需要的字节数

#pragma pack(pop)
#endif //_GENERAL_COMPRESS_H_INCLUDE
