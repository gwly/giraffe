#ifndef _SHL2_COMPRESS_INCLUDE
#define _SHL2_COMPRESS_INCLUDE

#include "datacollect.h"

#pragma pack(push,1)

//以下压缩算法均返回压缩了的数据数量，nBufSize调用时带入缓冲区数据容量，返回时得到实际数据数量，如果缓冲区不够，则返回-1，并用nBufSize带回需要的容量缓冲区
//压缩逐笔成交
int CompressL2Report(WORD wMarket,const SH_L2_REPORT* pRep,int nNum,BYTE* pBuf,int& nBufSize);		
int ExpandL2Report(const BYTE* pData,int nDataLen,WORD& wMarket,SH_L2_REPORT* pRepBuf,int& nBufSize);

//压缩扩展买卖盘
//int CompressL2MMPEx(WORD wMarket,const SH_L2_MMPEX* pMMPEx,int nNum,BYTE* pBuf,int& nBufSize);
//int ExpandL2MMPEx(const BYTE* pData,int nDataLen,WORD& wMarket,SH_L2_MMPEX* pMMPExBuf,int& nBufSize);

//压缩买卖盘队列
//队列索引和队列分不同缓冲区存储
int CompressL2Queue(WORD wMarket,const DWORD* pQueue,int nQueueLen,BYTE* pBuf,int& nBufSize,BOOL bOrderQueue=FALSE);
int ExpandL2Queue(const BYTE* pData,int nDataLen,DWORD* pQueueBuf,int& nQueueBufSize,BOOL bOrderQueue=FALSE);

//压缩即时最大撤单
int CompressL2BigWD(WORD wMarket,const DC_SHL2_BIG_WD* pWD,BYTE* pBuf,int& nBufSize);
int ExpandL2BigWD(const BYTE* pData,int nDataLen,WORD& wMarket,DC_SHL2_BIG_WD* pWDBuf,int& nBufSize);	//nBufSize表示字节数

//压缩累计最大撤单
int CompressL2AccuWD(WORD wMarket,const DC_SHL2_ACCU_WD* pWD,BYTE* pBuf,int& nBufSize);
int ExpandL2AccuWD(const BYTE* pData,int nDataLen,WORD& wMarket,DC_SHL2_ACCU_WD* pWDBuf,int& nBufSize);	//nBufSize表示字节数

//用于买卖两个即时最大撤单压缩到一起

//int CompressL2BigWD(WORD wMarket,const DC_SHL2_BIG_WD* pBuyWD,const DC_SHL2_BIG_WD* pSellWD,SVRNetHead* pBuf,int& nBufSize);
//int ExpandL2BigWD(const SVRNetHead* pData,WORD& wMarket,SH_L2_BIG_WD* pBuyBuf,int& nBuyBufSize,SH_L2_BIG_WD* pSellBuf,int& nSellBufSize);	//nBufBufSize, nSellBufSize表示数量
//int ExpandL2BigWD(const SVRNetHead* pHead,const BYTE* pData,WORD& wMarket,SH_L2_BIG_WD* pBuyBuf,int& nBuyBufSize,SH_L2_BIG_WD* pSellBuf,int& nSellBufSize);	//nBufBufSize, nSellBufSize表示数量
//
//int CompressL2AccuWD(WORD wMarket,const DC_SHL2_ACCU_WD* pBuyWD,const DC_SHL2_ACCU_WD* pSellWD,SVRNetHead* pBuf,int& nBufSize);
//int ExpandL2AccuWD(const SVRNetHead* pData,WORD& wMarket,SH_L2_ACCU_WD* pBuyBuf,int& nBuyBufSize,SH_L2_ACCU_WD* pSellBuf,int& nSellBufSize);	//nBuyBufSize,nSellBufSize表示数量
//int ExpandL2AccuWD(const SVRNetHead* pHead,const BYTE* pData,WORD& wMarket,SH_L2_ACCU_WD* pBuyBuf,int& nBuyBufSize,SH_L2_ACCU_WD* pSellBuf,int& nSellBufSize);	//nBuyBufSize,nSellBufSize表示数量

//压缩逐笔委托统计
BOOL CompressL2OrderStat(WORD wMarket,const DC_SZL2_ORDER_STAT* pStat,BYTE* pBuf,int& nBufSize);				//nBufSize表示字节数,返回后表示压缩后的总长度,压缩成功返回TRUE
int ExpandL2OrderStat(const BYTE* pData,int nDataLen,DC_SZL2_ORDER_STAT* pStatBuf,int& nBufSize);	//nBufSize表示字节数,返回后表示压缩后的总长度,返回解压缩成功数量

#pragma pack(pop)

#endif //_SHL2_COMPRESS_INCLUDE
