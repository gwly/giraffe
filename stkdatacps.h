#ifndef _STKDATACPS_H_INCLUDE
#define _STKDATACPS_H_INCLUDE

#include "flags.h"
#include "structs.h"
#include "bitstream.h"

#pragma pack(push,1)

//��Ҫʵ�ֵ�2������
const struct STK_DYNA* GetDynDatById(WORD wMarket,WORD index);
const struct STK_STATIC* GetStaticByID(WORD wMarket,WORD index);
//ÿ���������г��п�����Ҫ�޸ĵĺ���
BYTE	GetMarketDynaAttr(WORD wMarket);
BYTE	myGetMarketDynaAttr(WORD wMarket);

//���������ͻ�������ͷ
struct SVRNetHead
{
	unsigned short m_type;//��������
	DWORD	m_nLen;	//������
	WORD	m_wAttrib;	//����
};
//������������������ͷ�� DC_HEAD


//���º��������ڽ�����ͷ�����ݱ������ģ����������ΪNULL�����Ǳ�ʾ���������������ͷ���棬nBufSizeͳһ��ʾ��������ͷ��С���ڵĻ�������С

int CompressDataPack(WORD wMarket,BOOL bSeq,const DC_HEAD* pOrgPack,DC_HEAD* pBuf,int nBufSize);
int CompressDataPack(WORD wMarket,BOOL bSeq,const SVRNetHead* pOrgPack,SVRNetHead* pBuf,int nBufSize);
int CompressDataPack(WORD wMarket,BOOL bSeq,const DC_HEAD* pOrgHead,const BYTE* pOrgData,DC_HEAD* pHeadBuf,BYTE* pDataBuf,int nBufSize);
int CompressDataPack(WORD wMarket,BOOL bSeq,const SVRNetHead* pOrgHead,const BYTE* pOrgData,SVRNetHead* pHeadBuf,BYTE* pDataBuf,int nBufSize);

int ExpandDataPack(const DC_HEAD* pOrgPackHead,DC_HEAD* pBuf,int nBufSize,WORD* pwMarketBuf=NULL);		//pwMarketBuf���ڽ��ոð����г���Ϣ
int ExpandDataPack(const SVRNetHead* pOrgPack,SVRNetHead* pBuf,int nBufSize,WORD* pwMarketBuf=NULL);
int ExpandDataPack(const DC_HEAD* pOrgHead,const BYTE* pOrgData,DC_HEAD* pHeadBuf,BYTE* pDataBuf,int nBufSize,WORD* pwMarketBuf=NULL);
int ExpandDataPack(const SVRNetHead* pOrgHead,const BYTE* pOrgData,SVRNetHead* pHeadBuf,BYTE* pDataBuf,int nBufSize,WORD* pwMarketBuf=NULL);

int CompressRawData(WORD wMarket,BOOL bSeq,int nType,const BYTE* pOrgData,int nOrgDataLen,BYTE* pBuf,int nBufSize);		//ѹ��һ����,����ѹ����ĳ���,����0��ʾѹ�����ɹ�
int ExpandRawData(int nType,const BYTE* pCpsedPack,int nPackLen,WORD& wMarket,BYTE* pBuf,int nBufSize);							//��ѹ��һ���������ؽ�ѹ����ĳ���

//���ܺ���
BOOL EncryptDataPack(DC_ENCY_TYPE encType,DC_HEAD* pData);
BOOL EncryptDataPack(DC_ENCY_TYPE encType,DC_HEAD* pHead,BYTE* pRawData);	//����ͷ�����ݷ���
BOOL DecryptDataPack(DC_HEAD* pData);
BOOL DecryptDataPack(DC_HEAD* pHead,BYTE* pData);
BOOL EncryptDataPack(DC_ENCY_TYPE encType,SVRNetHead* pData);
BOOL EncryptDataPack(DC_ENCY_TYPE encType,SVRNetHead* pHead,BYTE* pData);
BOOL DecryptDataPack(SVRNetHead* pData);
BOOL DecryptDataPack(SVRNetHead* pHead,BYTE* pData);

#pragma pack(pop)

#endif //_STKDATACPS_H_INCLUDE
