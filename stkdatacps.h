#ifndef _STKDATACPS_H_INCLUDE
#define _STKDATACPS_H_INCLUDE

#include "flags.h"
#include "structs.h"
#include "bitstream.h"

#pragma pack(push,1)

//需要实现的2个函数
const struct STK_DYNA* GetDynDatById(WORD wMarket,WORD index);
const struct STK_STATIC* GetStaticByID(WORD wMarket,WORD index);
//每个新增的市场有可能需要修改的函数
BYTE	GetMarketDynaAttr(WORD wMarket);
BYTE	myGetMarketDynaAttr(WORD wMarket);

//服务器到客户端数据头
struct SVRNetHead
{
	unsigned short m_type;//数据类型
	DWORD	m_nLen;	//包长度
	WORD	m_wAttrib;	//属性
};
//采数机到服务器数据头用 DC_HEAD


//以下函数，对于将数据头和数据本身分离的，数据体可以为NULL，这是表示数据体紧跟在数据头后面，nBufSize统一表示包含数据头大小在内的缓冲区大小

int CompressDataPack(WORD wMarket,BOOL bSeq,const DC_HEAD* pOrgPack,DC_HEAD* pBuf,int nBufSize);
int CompressDataPack(WORD wMarket,BOOL bSeq,const SVRNetHead* pOrgPack,SVRNetHead* pBuf,int nBufSize);
int CompressDataPack(WORD wMarket,BOOL bSeq,const DC_HEAD* pOrgHead,const BYTE* pOrgData,DC_HEAD* pHeadBuf,BYTE* pDataBuf,int nBufSize);
int CompressDataPack(WORD wMarket,BOOL bSeq,const SVRNetHead* pOrgHead,const BYTE* pOrgData,SVRNetHead* pHeadBuf,BYTE* pDataBuf,int nBufSize);

int ExpandDataPack(const DC_HEAD* pOrgPackHead,DC_HEAD* pBuf,int nBufSize,WORD* pwMarketBuf=NULL);		//pwMarketBuf用于接收该包的市场信息
int ExpandDataPack(const SVRNetHead* pOrgPack,SVRNetHead* pBuf,int nBufSize,WORD* pwMarketBuf=NULL);
int ExpandDataPack(const DC_HEAD* pOrgHead,const BYTE* pOrgData,DC_HEAD* pHeadBuf,BYTE* pDataBuf,int nBufSize,WORD* pwMarketBuf=NULL);
int ExpandDataPack(const SVRNetHead* pOrgHead,const BYTE* pOrgData,SVRNetHead* pHeadBuf,BYTE* pDataBuf,int nBufSize,WORD* pwMarketBuf=NULL);

int CompressRawData(WORD wMarket,BOOL bSeq,int nType,const BYTE* pOrgData,int nOrgDataLen,BYTE* pBuf,int nBufSize);		//压缩一个包,返回压缩后的长度,返回0表示压缩不成功
int ExpandRawData(int nType,const BYTE* pCpsedPack,int nPackLen,WORD& wMarket,BYTE* pBuf,int nBufSize);							//解压缩一个包，返回解压缩后的长度

//加密函数
BOOL EncryptDataPack(DC_ENCY_TYPE encType,DC_HEAD* pData);
BOOL EncryptDataPack(DC_ENCY_TYPE encType,DC_HEAD* pHead,BYTE* pRawData);	//数据头，数据分离
BOOL DecryptDataPack(DC_HEAD* pData);
BOOL DecryptDataPack(DC_HEAD* pHead,BYTE* pData);
BOOL EncryptDataPack(DC_ENCY_TYPE encType,SVRNetHead* pData);
BOOL EncryptDataPack(DC_ENCY_TYPE encType,SVRNetHead* pHead,BYTE* pData);
BOOL DecryptDataPack(SVRNetHead* pData);
BOOL DecryptDataPack(SVRNetHead* pHead,BYTE* pData);

#pragma pack(pop)

#endif //_STKDATACPS_H_INCLUDE
