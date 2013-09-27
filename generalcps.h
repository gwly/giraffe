#ifndef _GENERAL_COMPRESS_H_INCLUDE
#define _GENERAL_COMPRESS_H_INCLUDE

#include "datacollect.h"
#include "bitstream.h"

struct SVRNetHead;

#pragma pack(push,1)

//ѹ����չ��̬
int CompressStaticEx(const STK_STATICEx* pStatic,int nNum,CBitStream& stream);
int ExpandStaticEx(CBitStream& stream,STK_STATICEx* pBuf,int& nBufSize);		//nBufSize��ʾpBuf�ĸ���,����������������򷵻�-1��nBufSize������Ҫ������

//ѹ���۹ɶ�̬
int CompressHKDyna(const STK_HKDYNA* pHKDyna,int nNum,CBitStream& stream);
int ExpandHKDyna(CBitStream& stream,STK_HKDYNA* pBuf,int& nBufSize);				//nBufSize��ʾpBuf�ĸ���,����������������򷵻�-1��nBufSize������Ҫ������

//ѹ��ͨ������
struct CPS_GENERAL_HEAD
{
	WORD	m_wDataID;			//����ID
	WORD	m_wMarket;			//�г�
	DWORD	m_dwDataAttr;		//��������
	short	m_nDataSize;		//ÿ�����ݵĳ���,���ڱ䳤����,��ʾÿ�����ݵ����ֵ(0��ʾû�����ֵ�����ⳤ��)
	UINT24	m_nTotalLen;		//����������ͷ���ڵ������ܳ���
	WORD	m_wNum;
};

int CompressGeneralData(DC_GENERAL* pData,BYTE* pBuf,int nBufSize);		//����ѹ�����ݳ��ȣ�0��ʾ����ѹ����ѹ��ʧ�ܣ�-1��ʾ���������Ȳ���
int ExpandGeneralData(const BYTE* pCpsData,int nDataLen,DC_GENERAL* pBuf,int& nBufSize);		//nBufSize��ʾpBuf�ֽ��������ؽ�ѹ������ֽ�����0��ʾʧ�ܣ�-1��ʾ����������,nBufSize������Ҫ���ֽ���

#pragma pack(pop)
#endif //_GENERAL_COMPRESS_H_INCLUDE
