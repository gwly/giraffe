#ifndef _SHL2_COMPRESS_INCLUDE
#define _SHL2_COMPRESS_INCLUDE

#include "datacollect.h"

#pragma pack(push,1)

//����ѹ���㷨������ѹ���˵�����������nBufSize����ʱ���뻺������������������ʱ�õ�ʵ����������������������������򷵻�-1������nBufSize������Ҫ������������
//ѹ����ʳɽ�
int CompressL2Report(WORD wMarket,const SH_L2_REPORT* pRep,int nNum,BYTE* pBuf,int& nBufSize);		
int ExpandL2Report(const BYTE* pData,int nDataLen,WORD& wMarket,SH_L2_REPORT* pRepBuf,int& nBufSize);

//ѹ����չ������
//int CompressL2MMPEx(WORD wMarket,const SH_L2_MMPEX* pMMPEx,int nNum,BYTE* pBuf,int& nBufSize);
//int ExpandL2MMPEx(const BYTE* pData,int nDataLen,WORD& wMarket,SH_L2_MMPEX* pMMPExBuf,int& nBufSize);

//ѹ�������̶���
//���������Ͷ��зֲ�ͬ�������洢
int CompressL2Queue(WORD wMarket,const DWORD* pQueue,int nQueueLen,BYTE* pBuf,int& nBufSize,BOOL bOrderQueue=FALSE);
int ExpandL2Queue(const BYTE* pData,int nDataLen,DWORD* pQueueBuf,int& nQueueBufSize,BOOL bOrderQueue=FALSE);

//ѹ����ʱ��󳷵�
int CompressL2BigWD(WORD wMarket,const DC_SHL2_BIG_WD* pWD,BYTE* pBuf,int& nBufSize);
int ExpandL2BigWD(const BYTE* pData,int nDataLen,WORD& wMarket,DC_SHL2_BIG_WD* pWDBuf,int& nBufSize);	//nBufSize��ʾ�ֽ���

//ѹ���ۼ���󳷵�
int CompressL2AccuWD(WORD wMarket,const DC_SHL2_ACCU_WD* pWD,BYTE* pBuf,int& nBufSize);
int ExpandL2AccuWD(const BYTE* pData,int nDataLen,WORD& wMarket,DC_SHL2_ACCU_WD* pWDBuf,int& nBufSize);	//nBufSize��ʾ�ֽ���

//��������������ʱ��󳷵�ѹ����һ��

//int CompressL2BigWD(WORD wMarket,const DC_SHL2_BIG_WD* pBuyWD,const DC_SHL2_BIG_WD* pSellWD,SVRNetHead* pBuf,int& nBufSize);
//int ExpandL2BigWD(const SVRNetHead* pData,WORD& wMarket,SH_L2_BIG_WD* pBuyBuf,int& nBuyBufSize,SH_L2_BIG_WD* pSellBuf,int& nSellBufSize);	//nBufBufSize, nSellBufSize��ʾ����
//int ExpandL2BigWD(const SVRNetHead* pHead,const BYTE* pData,WORD& wMarket,SH_L2_BIG_WD* pBuyBuf,int& nBuyBufSize,SH_L2_BIG_WD* pSellBuf,int& nSellBufSize);	//nBufBufSize, nSellBufSize��ʾ����
//
//int CompressL2AccuWD(WORD wMarket,const DC_SHL2_ACCU_WD* pBuyWD,const DC_SHL2_ACCU_WD* pSellWD,SVRNetHead* pBuf,int& nBufSize);
//int ExpandL2AccuWD(const SVRNetHead* pData,WORD& wMarket,SH_L2_ACCU_WD* pBuyBuf,int& nBuyBufSize,SH_L2_ACCU_WD* pSellBuf,int& nSellBufSize);	//nBuyBufSize,nSellBufSize��ʾ����
//int ExpandL2AccuWD(const SVRNetHead* pHead,const BYTE* pData,WORD& wMarket,SH_L2_ACCU_WD* pBuyBuf,int& nBuyBufSize,SH_L2_ACCU_WD* pSellBuf,int& nSellBufSize);	//nBuyBufSize,nSellBufSize��ʾ����

//ѹ�����ί��ͳ��
BOOL CompressL2OrderStat(WORD wMarket,const DC_SZL2_ORDER_STAT* pStat,BYTE* pBuf,int& nBufSize);				//nBufSize��ʾ�ֽ���,���غ��ʾѹ������ܳ���,ѹ���ɹ�����TRUE
int ExpandL2OrderStat(const BYTE* pData,int nDataLen,DC_SZL2_ORDER_STAT* pStatBuf,int& nBufSize);	//nBufSize��ʾ�ֽ���,���غ��ʾѹ������ܳ���,���ؽ�ѹ���ɹ�����

#pragma pack(pop)

#endif //_SHL2_COMPRESS_INCLUDE
