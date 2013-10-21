#include "stdafx.h"
#include "datacollect.h"
//#include "stkdatacps.h"
#include "extract_dc.h"
#include "datatype.h"
#include "generalcps.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//压缩扩展静态

static BITCODE seSubTypeCode[] = //子类型
{
	{0		,	1,	0,'E',		0,	0},			// 0			
	{4		,	3,	0,'E',	  'A',	0},			// 100			
	{5		,	3,	0,'E',	  'B',	0},			// 101			
	{0xC	,	4,	0,'E',	  'G',	0},			// 1100			
	{0xD	,	4,	0,'E',	  'O',	0},			// 1101			
	{0xE	,	4,	0,'E',	  'F',	0},			// 1110			
	{0xF	,	4,  8,'D',		0,	0},			// 1111	+ 8Bit
};

static BITCODE seStatusCode[] =
{
	{0		,	2,	0,'E',		'N',	0},			// 00
	{1		,	2,	0,'E',		'H',	0},			// 01
	{2		,	2,	0,'E',		'X',	0},			// 10
	{3		,	2,	8,'D',		  0,	0},			// 11
		
};

int CompressStaticEx(const STK_STATICEx* pStatic,int nNum,CBitStream& stream)
{
	int nRet = 0;
	int i,j;
	try
	{
		stream.Put(nNum,16);
		
		for(i=0;i<nNum;i++)
		{
			const STK_STATICEx* pCur = pStatic+i;
			stream.Put(pCur->m_cType,5);

			stream.SETBITCODE(seSubTypeCode);
			stream.EncodeData(pCur->m_cSubType);

			int nInd = 0x1F;
			switch(pCur->m_cType)
			{
			case STK_STATIC::STOCK:
			case STK_STATIC::OTHER_STOCK:
				stream.EncodeFloat(pCur->m_equitySpec.m_fFaceValue,TRUE);
				stream.EncodeFloat(pCur->m_equitySpec.m_fProfit,TRUE);
				stream.EncodeFloat(pCur->m_equitySpec.m_fCashDividend,TRUE);
				for(j=0;j<sizeof(industryClasify)/sizeof(industryClasify[0]);j++)
				{
					if(pCur->m_equitySpec.m_wIndustry==industryClasify[j].m_wCode)
					{
						nInd = j;
						break;
					}
				}
				stream.Put(nInd,5);
				stream.SETBITCODE(seStatusCode);
				stream.EncodeData(pCur->m_equitySpec.m_cTradeStatus);
				break;
			case STK_STATIC::ETF:
			case STK_STATIC::LOF:
			case STK_STATIC::FUND:
				stream.EncodeFloat(pCur->m_fundSpec.m_fFaceValue,TRUE);
				stream.EncodeFloat(pCur->m_fundSpec.m_fTotalIssued,TRUE);
				stream.EncodeFloat(pCur->m_fundSpec.m_fIOPV,TRUE);
				break;
			case STK_STATIC::OPTION:
			case STK_STATIC::WARRANT:
				stream.Put(pCur->m_warrantSpec.m_cStyle=='A' ? 1 : 0, 1);
				stream.Put(pCur->m_warrantSpec.m_cCP=='C' ? 1 : 0, 1);
				stream.EncodeFloat(pCur->m_warrantSpec.m_fCnvtRatio,TRUE);
				stream.EncodeFloat(pCur->m_warrantSpec.m_fStrikePrice,TRUE);
				stream.EncodeFloat(pCur->m_warrantSpec.m_fBalance,TRUE);
				stream.Put(pCur->m_warrantSpec.m_dwMaturityDate,32);
				for(j=0;j<12;j++)
				{
					stream.Put(pCur->m_warrantSpec.m_strUnderLine[j],8);
					if(pCur->m_warrantSpec.m_strUnderLine[j]==0)
						break;
				}
				break;
			case STK_STATIC::BOND:
				stream.Put(pCur->m_bondSpec.m_dwMaturityDate,32);
				stream.Put(pCur->m_bondSpec.m_dwIntAccruDate,32);
				stream.EncodeFloat(pCur->m_bondSpec.m_fIssuePrice,TRUE);
				stream.EncodeFloat(pCur->m_bondSpec.m_fCouponRate,TRUE);
				stream.EncodeFloat(pCur->m_bondSpec.m_fFaceValue,TRUE);
				stream.EncodeFloat(pCur->m_bondSpec.m_fAccruedInt,TRUE);
				break;
			case STK_STATIC::COV_BOND:
				stream.Put(pCur->m_CnvtSpec.m_cStyle=='A' ? 1 : 0, 1);
				stream.Put(pCur->m_CnvtSpec.m_cCP=='C' ? 1 : 0, 1);
				stream.EncodeFloat(pCur->m_CnvtSpec.m_fCnvtRatio,TRUE);
				stream.EncodeFloat(pCur->m_CnvtSpec.m_fStrikePrice,TRUE);
				stream.EncodeFloat(pCur->m_CnvtSpec.m_fAccruedInt,TRUE);
				stream.Put(pCur->m_CnvtSpec.m_dwMaturityDate,32);
				for(j=0;j<12;j++)
				{
					stream.Put(pCur->m_CnvtSpec.m_strUnderLine[j],8);
					if(pCur->m_CnvtSpec.m_strUnderLine[j]==0)
						break;
				}
				break;
			case STK_STATIC::FUTURE:
			case STK_STATIC::FTR_IDX:
			case STK_STATIC::COMM:
				stream.Put(pCur->m_futureSpec.m_dwLastDayOI,32);
				stream.EncodeFloat(pCur->m_futureSpec.m_fLastSettlePrice,TRUE);
				break;
			case STK_STATIC::TRUST:
				stream.EncodeFloat(pCur->m_trustSpec.m_dwfAsset,TRUE);
				stream.Put(pCur->m_trustSpec.m_dwAssetDate,32);
				break;
			case STK_STATIC::EXCHANGE:
				break;
			default:
				break;
			}
		}
		nRet = nNum;
	}
	catch(...)
	{
		nRet = 0;
	}
	return nRet;
}

int ExpandStaticEx(CBitStream& stream,STK_STATICEx* pBuf,int& nBufSize)
{
	int nRet = 0;
	int i,j;
	try
	{
		int nNum = stream.Get(16);

		if(nBufSize<nNum)
			nRet = -1;
		else
		{
			memset(pBuf,0,nNum*sizeof(STK_STATICEx));
			for(i=0;i<nNum;i++)
			{
				STK_STATICEx* pCur = pBuf+i;
				pCur->m_cType = (BYTE)stream.Get(5);

				stream.SETBITCODE(seSubTypeCode);
				pCur->m_cSubType = (BYTE)stream.DecodeData();
				
				switch(pCur->m_cType)
				{
				case STK_STATIC::STOCK:
				case STK_STATIC::OTHER_STOCK:
					pCur->m_equitySpec.m_fFaceValue = stream.DecodeFloat(TRUE);
					pCur->m_equitySpec.m_fProfit = stream.DecodeFloat(TRUE);
					pCur->m_equitySpec.m_fCashDividend = stream.DecodeFloat(TRUE);
					j = stream.Get(5);
					if(j<sizeof(industryClasify)/sizeof(industryClasify[0]))
					pCur->m_equitySpec.m_wIndustry = industryClasify[j].m_wCode;

					stream.SETBITCODE(seStatusCode);
					pCur->m_equitySpec.m_cTradeStatus = (BYTE)stream.DecodeData();
					break;
				case STK_STATIC::ETF:
				case STK_STATIC::LOF:
				case STK_STATIC::FUND:
					pCur->m_fundSpec.m_fFaceValue = stream.DecodeFloat(TRUE);
					pCur->m_fundSpec.m_fTotalIssued = stream.DecodeFloat(TRUE);
					pCur->m_fundSpec.m_fIOPV = stream.DecodeFloat(TRUE);
					break;
				case STK_STATIC::OPTION:
				case STK_STATIC::WARRANT:
					pCur->m_warrantSpec.m_cStyle = stream.Get(1) ? 'A' : 'E';
					pCur->m_warrantSpec.m_cCP = stream.Get(1) ? 'C' : 'P';
					pCur->m_warrantSpec.m_fCnvtRatio = stream.DecodeFloat(TRUE);
					pCur->m_warrantSpec.m_fStrikePrice = stream.DecodeFloat(TRUE);
					pCur->m_warrantSpec.m_fBalance = stream.DecodeFloat(TRUE);
					pCur->m_warrantSpec.m_dwMaturityDate = stream.Get(32);
					for(j=0;j<12;j++)
					{
						pCur->m_warrantSpec.m_strUnderLine[j] = (BYTE)stream.Get(8);
						if(pCur->m_warrantSpec.m_strUnderLine[j]==0)
							break;
					}
					break;
				case STK_STATIC::BOND:
					pCur->m_bondSpec.m_dwMaturityDate = stream.Get(32);
					pCur->m_bondSpec.m_dwIntAccruDate = stream.Get(32);
					pCur->m_bondSpec.m_fIssuePrice = stream.DecodeFloat(TRUE);
					pCur->m_bondSpec.m_fCouponRate = stream.DecodeFloat(TRUE);
					pCur->m_bondSpec.m_fFaceValue = stream.DecodeFloat(TRUE);
					pCur->m_bondSpec.m_fAccruedInt = stream.DecodeFloat(TRUE);
					break;
				case STK_STATIC::COV_BOND:
					pCur->m_CnvtSpec.m_cStyle = stream.Get(1) ? 'A' : 'E';
					pCur->m_CnvtSpec.m_cCP = stream.Get(1) ? 'C' : 'P';
					pCur->m_CnvtSpec.m_fCnvtRatio = stream.DecodeFloat(TRUE);
					pCur->m_CnvtSpec.m_fStrikePrice = stream.DecodeFloat(TRUE);
					pCur->m_CnvtSpec.m_fAccruedInt = stream.DecodeFloat(TRUE);
					pCur->m_CnvtSpec.m_dwMaturityDate = stream.Get(32);
					for(j=0;j<12;j++)
					{
						pCur->m_CnvtSpec.m_strUnderLine[j] = (BYTE)stream.Get(8);
						if(pCur->m_CnvtSpec.m_strUnderLine[j]==0)
							break;
					}
					break;
				case STK_STATIC::FUTURE:
				case STK_STATIC::FTR_IDX:
				case STK_STATIC::COMM:
					pCur->m_futureSpec.m_dwLastDayOI = stream.Get(32);
					pCur->m_futureSpec.m_fLastSettlePrice = stream.DecodeFloat(TRUE);
					break;
				case STK_STATIC::TRUST:
					pCur->m_trustSpec.m_dwfAsset = stream.DecodeFloat(TRUE);
					pCur->m_trustSpec.m_dwAssetDate = stream.Get(32);
					break;
				case STK_STATIC::EXCHANGE:
					break;
				default:
					break;
				}
			}
			nRet = nNum;
		}
		nBufSize = nNum;
	}
	catch(...)
	{
		nRet = 0;
	}
	return nRet;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////
//压缩港股动态
static BITCODE hkPriceCode[] =	//价格
{
	{0		,	2,  0,'E',		0,  0},			// 00			= 0
	{6		,	3,  8,'B',		8,  1},			// 110+8Bit		= 8Bit+1
	{1		,	2, 16,'B',	   16,257},			// 01+16Bit		= 16Bit+256+1
	{2		,	2, 24,'B',	   24,65793},		// 10+24Bit		= 24Bit+65536+256+1
	{7		,	3, 32,'D',	    0,	0},			// 111+32Bit	= 32Bit Org
};

extern BITCODE hkVolCode[6];

static BITCODE hkOrderNumCode[] =	//单量
{
	{0		,	2,  0,'E',		0,  0},			// 00			= 0
	{1		,	2,  6,'B',		6,  1},			// 01+6Bit		= 6Bit
	{2		,	2, 10,'B',	   10, 65},			// 10+10Bit		= 10Bit+64+1
	{3		,	2, 16,'D',	    0,	0},			// 11+16Bit		= 16Bit Org
};

static BITCODE hkBrokerTypeCode[] =	//经纪
{
	{0		,	1,  0,'E',	   'B',  0},		// 0			= 'B'
	{2		,	2,  0,'E',	   'R',  0},		// 10			= 'R'
	{6		,	3,  0,'E',	   'S',  0},		// 110			= 'S'
	{7		,	3,  8,'D',	    0,	0},			// 111			= 8Bit
};

static BITCODE hkBrokerCode[] =	//经纪
{
	{0		,	1, 11,'B',	   11,  0},			// 0+11Bit		= 11Bit
	{1		,	1, 16,'D',	    0,	0},			// 1+16Bit		= 16Bit Org
};

int CompressHKDyna(const STK_HKDYNA* pHKDyna,int nNum,CBitStream& stream)
{
	int nRet = 0;
	int i,j,nc,nb;
	try
	{
		stream.Put(nNum,16);

		for(i=0;i<nNum;i++)
		{
			stream.SETBITCODE(hkPriceCode);
			stream.EncodeData(pHKDyna[i].m_dwIEPrice);

			stream.SETBITCODE(hkVolCode);
			stream.EncodeData(pHKDyna[i].m_mIEVolume);

			stream.SETBITCODE(hkOrderNumCode);
			for(j=0;j<5;j++)
			{
				stream.EncodeData(pHKDyna[i].m_wBuyOrderNum[j]);
				stream.EncodeData(pHKDyna[i].m_wSellOrderNum[j]);
			}

			for(nc=0;nc<2;nc++)
			{
				const STK_HKDYNA::HK_BROKER& br = nc ? pHKDyna[i].m_sellBroker : pHKDyna[i].m_buyBroker;
				stream.Put(br.m_wNumBroker,6);

				stream.SETBITCODE(hkBrokerTypeCode);
				nb = Min((int)br.m_wNumBroker,40);
				for(j=0;j<nb;j++)
					stream.EncodeData(br.m_cType[j]);

				stream.SETBITCODE(hkBrokerCode);
				for(j=0;j<nb;j++)
					stream.EncodeData(br.m_wBrokerNo[j]);
			}
		}
		nRet = nNum;
	}
	catch(...)
	{
		nRet = 0;
	}
	return nRet;
}

int ExpandHKDyna(CBitStream& stream,STK_HKDYNA* pBuf,int& nBufSize)
{
	int nRet = 0;
	int i,j,nc;
	try
	{
		int nNum = stream.Get(16);
		if(nBufSize<nNum)
		{
			nRet = -1;
		}
		else
		{
			memset(pBuf,0,nNum*sizeof(STK_HKDYNA));
			for(i=0;i<nNum;i++)
			{
				stream.SETBITCODE(hkPriceCode);
				pBuf[i].m_dwIEPrice = stream.DecodeData();

				stream.SETBITCODE(hkVolCode);
				pBuf[i].m_mIEVolume = stream.DecodeData();

				stream.SETBITCODE(hkOrderNumCode);
				for(j=0;j<5;j++)
				{
					pBuf[i].m_wBuyOrderNum[j] = (WORD)stream.DecodeData();
					pBuf[i].m_wSellOrderNum[j] = (WORD)stream.DecodeData();
				}

				for(nc=0;nc<2;nc++)
				{
					STK_HKDYNA::HK_BROKER& br = nc ? pBuf[i].m_sellBroker : pBuf[i].m_buyBroker;
					br.m_wNumBroker = (WORD)stream.Get(6);

					stream.SETBITCODE(hkBrokerTypeCode);
					for(j=0;j<(int)br.m_wNumBroker;j++)
						br.m_cType[j] = (char)stream.DecodeData();

					stream.SETBITCODE(hkBrokerCode);
					for(j=0;j<(int)br.m_wNumBroker;j++)
						br.m_wBrokerNo[j] = (WORD)stream.DecodeData();
				}
			}
			nRet = nNum;
		}
		nBufSize = nNum;
	}
	catch(...)
	{
		nRet = 0;
	}
	return nRet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

static BITCODE geStkIDCode[] =	//StockID
{
	{0		,	1,	0,'E',		1,	0},			// 0				顺序
	{2		,	2,	4,'B',		4,	1},			// 10	+4Bit		顺序,1+4Bit
	{3		,	2, 16,'D',		0,	0},			// 11	+16Bit		直接存放wStockID
};

int CompressGeneralData(DC_GENERAL* pData,BYTE* pBuf,int nBufSize)
{
	int nRet = 0;
	WORD i;
	if(nBufSize>sizeof(CPS_GENERAL_HEAD))
	{
		memcpy(pBuf,pData,sizeof(CPS_GENERAL_HEAD));
		CBitStream stream(pBuf+sizeof(CPS_GENERAL_HEAD),nBufSize-sizeof(CPS_GENERAL_HEAD));
		try
		{
			DWORD dwLast = 0;
			stream.SETBITCODE(geStkIDCode);
			for(i=0;i<pData->m_wNum;i++)
			{
				stream.EncodeData(pData->m_wStock[i],&dwLast);
				dwLast = pData->m_wStock[i];
			}
		}
		catch(...)
		{
			nRet = -1;
		}
		if(nRet>=0)
		{
			int ndl = 0;
			switch(pData->m_wDataID)
			{
			case GE_STATIC_EX:
				ndl = CompressStaticEx((STK_STATICEx*)(pData->m_wStock+pData->m_wNum),pData->m_wNum,stream);
				break;
			case GE_HKDYNA:
				ndl = CompressHKDyna((STK_HKDYNA*)(pData->m_wStock+pData->m_wNum),pData->m_wNum,stream);
				break;
			default:
				break;
			}
			if(ndl>0)
				nRet = sizeof(CPS_GENERAL_HEAD) + (stream.GetCurPos()+7)/8;
			else
				nRet = pData->m_wNum>0 ? -1 : 0;
		}
	}
	else
		nRet = -1;
	return nRet;
}

int ExpandGeneralData(const BYTE* pCpsData,int nDataLen,DC_GENERAL* pData,int& nBufSize)
{
	int nRet = 0;
	WORD i;
	if(nDataLen>sizeof(CPS_GENERAL_HEAD))
	{
		CPS_GENERAL_HEAD* pCpsHead = (CPS_GENERAL_HEAD*)pCpsData;
		if((DWORD)nBufSize<pCpsHead->m_nTotalLen)
		{
			nRet = -1;
			nBufSize = pCpsHead->m_nTotalLen;
		}
		else if(!(pCpsHead->m_dwDataAttr&GENERAL_VAR) && pCpsHead->m_nDataSize>0)
		{
			memcpy(pData,pCpsData,sizeof(CPS_GENERAL_HEAD));
			int nDataNum = (nBufSize-sizeof(DC_GENERAL)-(pData->m_wNum-1)*sizeof(WORD))/pData->m_nDataSize;
			CBitStream stream((BYTE*)pCpsData+sizeof(CPS_GENERAL_HEAD),nDataLen-sizeof(CPS_GENERAL_HEAD));
			try
			{
				DWORD dwLast = 0;
				stream.SETBITCODE(geStkIDCode);
				for(i=0;i<pData->m_wNum;i++)
				{
					pData->m_wStock[i] = (WORD)stream.DecodeData(&dwLast);
					dwLast = pData->m_wStock[i];
				}
			}
			catch(...)
			{
				nRet = -1;
			}
			if(nRet>=0)
			{
				int ndl = 0;
				switch(pData->m_wDataID)
				{
				case GE_STATIC_EX:
					ndl = ExpandStaticEx(stream,(STK_STATICEx*)(pData->m_wStock+pData->m_wNum),nDataNum);
					break;
				case GE_HKDYNA:
					ndl = ExpandHKDyna(stream,(STK_HKDYNA*)(pData->m_wStock+pData->m_wNum),nDataNum);
					break;
				default:
					break;
				}
				if(ndl>0)
					nRet = sizeof(DC_GENERAL) + (pData->m_wNum-1)*sizeof(WORD) + ndl*pData->m_nDataSize;
				else
					nRet = 0;
			}
			nBufSize = ((CPS_GENERAL_HEAD*)pCpsData)->m_nTotalLen;
		}
	}
	else
		nRet = 0;
	return nRet;
}
