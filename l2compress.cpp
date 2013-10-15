#include "stdafx.h"
#include "datacollect.h"
#include "stkdatacps.h"
#include "datatype.h"
#include "l2compress.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//逐笔成交

static BITCODE repNumRepCode[] = //连续数量
{
	{0		,	1,	0,'E',		1,	0},			// 0				1个
	{0x2	,	2,	3,'B',		3,	2},			// 10	+3Bit		3Bit+2个
	{0x6	,	3,	8,'B',		8, 10},			// 110	+8Bit		8Bit+10个
	{0x7	,	3, 16,'D',		0,	0},			// 111	+16Bit		16Bit
};

static BITCODE repStkIDCode[] =	//StockID
{
	{0		,	1,	0,'E',		1,	0},			// 0				顺序
	{0x2	,	2,	6,'B',		6,	2},			// 10	+6Bit		顺序,6Bit+2
	{0x3	,	2, 16,'D',		0,	0},			// 11	+16Bit		直接存放wStockID
};

static BITCODE repTimeCode[] =
{
	{0		,	1,	0,'E',		0,  0},			//0					= 基准
	{2		,	2,	3,'B',		3,  1},			//10	+3Bit		= 基准 + 3Bit + 1
	{6		,	3,  6,'B',	    6,  9},			//110	+6Bit		= 基准 + 6Bit + 9
	{0xE	,	4, 16,'B',	   16, 41},			//1110  +16Bit		= 基准 + 16Bit + 32+9; todo Change to 64+9
	{0xF	,	4, 32,'D',	    0,	0},			//1111	+32Bit		= 32Bit org
};

static BITCODE repPriceCode[] = //首记录
{
	{0x2	,	2,	8,'B',		8, 0},			//10   +8Bit	= 8Bit
	{0x0	,	1, 12,'B',	   12,256},			//0   +12Bit	= 12Bit+256
	{0x6	,	3, 16,'B',	   16,4352},		//110 +16Bit	= 16Bit+4096+256
	{0x7	,	3, 32,'D',		0, 0},			//111 +32Bit	= DWORD
};

static BITCODE repPriceDiffCode[] = //之后以前一个价格为基准
{
	{0		,	1,	0,'E',		0, 0},			//0				= 基准
	{2		,	2,	2,'b',		2, 0},			//10			= 基准+2Bit; todo change to 100 +1, 101 -1
	{0x6	,	3,	4,'b',		4, 1},			//110	+4Bit	= 基准+4Bit+1
	{0xE	,	4,	8,'b',		8, 9},			//1110  +8Bit	= 基准+8Bit+9
	{0x1E	,	5, 16,'b',	   16,137},			//11110 +16Bit	= 基准+16Bit+128+9
	{0x1F	,	5, 32,'D',		0, 0},			//11111 +32Bit	= DWORD
};

static BITCODE repVolumeCode[] =	//量; todo change to B
{
	{0x0	,	1,  6,'b',	   6,	0},			// 0+6Bit			= 6Bit
	{0x5	,	3,  4,'Z',	   2,	1},			// 101+2Bit+2Bit	= 2Bit*10^2Bit
	{0xE	,	4,  6,'Z',	   4,	5},			// 1110+4Bit+2Bit	= (4Bit+4)*10^2Bit
	{0x4	,	3,  8,'b',	   8,  32},			// 100+8Bit			= 8Bit+32
	{0x6	,	3, 12,'b',	  12, 160},			// 110+12Bit		= 12Bit+128+32
	{0x1E	,	5, 16,'b',	  16,2208},			// 11110+16Bit		= 16Bit+2048+128+32
	{0x1F	,	5, 32,'D',	   0,	0},			// 11111+32Bit		= 32Bit DWORD
};


int CompressL2Report(WORD wMarket,const SH_L2_REPORT* pRep,int nNum,BYTE* pBuf,int& nBufSize)
{
	int nRet = 0;
	int i,j;
	time_t tBase = 0x7FFFFFFF;
	for(i=0;i<nNum;i++)
	{
		if(pRep[i].m_time<tBase)
			tBase = pRep[i].m_time;
	}
	CBitStream stream(pBuf,nBufSize);

	try
	{
		stream.Put(nNum,16);
		stream.Put(wMarket,16);
		stream.Put(tBase,32);

		int nSeqLen = 0;
		DWORD dwLastStkID = 0;
		DWORD dwLastPrice = 0;
		DWORD tLast = 0;
		for(i=0;i<nNum;i++)
		{
			const SH_L2_REPORT* pCur = pRep+i;

			if(nSeqLen==0)
			{
				nSeqLen = 1;
				for(j=i+1;j<nNum;j++)
				{
					if(pRep[j].m_wStkID==pRep[i].m_wStkID)
						nSeqLen++;
					else
						break;
				}
				stream.SETBITCODE(repNumRepCode);
				stream.EncodeData(nSeqLen);

				stream.SETBITCODE(repStkIDCode);
				stream.EncodeData(pCur->m_wStkID,&dwLastStkID);

				stream.SETBITCODE(repTimeCode);
				stream.EncodeData(pCur->m_time,(DWORD*)&tBase);

				stream.SETBITCODE(repPriceCode);
				stream.EncodeData(pCur->m_dwPrice);
			}
			else
			{
				stream.SETBITCODE(repTimeCode);
				stream.EncodeData(pCur->m_time,&tLast);

				stream.SETBITCODE(repPriceDiffCode);
				stream.EncodeData(pCur->m_dwPrice,&dwLastPrice);
			}

			stream.SETBITCODE(repVolumeCode);
			stream.EncodeData(pCur->m_dwVolume);
			
			nSeqLen--;
			if(nSeqLen==0)
				dwLastStkID = pCur->m_wStkID;
			tLast = pCur->m_time;
			dwLastPrice = pCur->m_dwPrice;
		}
		nBufSize = (stream.GetCurPos()+7)/8;
		nRet = nNum;
	}
	catch(...)
	{
		nRet = 0;
	}
	return nRet;
}

int ExpandL2Report(const BYTE* pData,int nDataLen,WORD& wMarket,SH_L2_REPORT* pRepBuf,int& nBufSize)
{
	int nRet = 0;
	int i;

	CBitStream stream((BYTE*)pData,nDataLen);

	try
	{
		int nNum = stream.Get(16);
		wMarket = (WORD)stream.Get(16);
		time_t tBase = stream.Get(32);

		if(nNum<=nBufSize)
		{
			int nSeqLen = 0;
			DWORD dwLastStkID = 0;
			DWORD dwLastPrice = 0;
			DWORD tLast = 0;
			WORD wStkID = 0;
			for(i=0;i<nNum;i++)
			{
				SH_L2_REPORT* pCur = pRepBuf+i;

				if(nSeqLen==0)
				{
					stream.SETBITCODE(repNumRepCode);
					nSeqLen = stream.DecodeData();

					stream.SETBITCODE(repStkIDCode);
					pCur->m_wStkID = (WORD)stream.DecodeData(&dwLastStkID);
					wStkID = pCur->m_wStkID;

					stream.SETBITCODE(repTimeCode);
					pCur->m_time = stream.DecodeData((DWORD*)&tBase);

					stream.SETBITCODE(repPriceCode);
					pCur->m_dwPrice = stream.DecodeData();
				}
				else
				{
					pCur->m_wStkID = wStkID;

					stream.SETBITCODE(repTimeCode);
					pCur->m_time = stream.DecodeData(&tLast);

					stream.SETBITCODE(repPriceDiffCode);
					pCur->m_dwPrice = stream.DecodeData(&dwLastPrice);
				}

				stream.SETBITCODE(repVolumeCode);
				pCur->m_dwVolume = stream.DecodeData();
				
				nSeqLen--;
				if(nSeqLen==0)
					dwLastStkID = wStkID;
				tLast = pCur->m_time;
				dwLastPrice = pCur->m_dwPrice;
			}
			nRet = nNum;
		}
		else
			nRet = -1;
		nBufSize = nNum;
	}
	catch(...)
	{
		nRet = 0;
	}
	return nRet;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//买卖盘队列


static BITCODE queueTypeCode[] =
{
	{0		,	1,	0,'E',		QUEUE_ORDER,		0},			// 0
	{0x2	,	2,	0,'E',		QUEUE_BUYPRICE,		0},			// 10
	{0x6	,	3,	0,'E',		QUEUE_SELLPRICE,	0},			// 110
	{0x7	,	3,  0,'E',		QUEUE_STOCKID,		0},			// 111
};

static BITCODE queueVolCode[] =
{
	{0x0	,	1,  6,'B',	   6,	0},			// 0+6Bit			= 6Bit
	{0x5	,	3,  4,'Z',	   2,	1},			// 101+2Bit+2Bit	= 2Bit*10^2Bit
	{0x6	,	3,  6,'Z',	   4,	5},			// 110+4Bit+2Bit	= (4Bit+4)*10^2Bit
	{0x4	,	3,  8,'B',	   8,  64},			// 100+8Bit			= 8Bit+64
	{0xE	,	4, 16,'B',	  16, 320},			// 1110+16Bit		= 16Bit+256+64
	{0xF	,	4, 30,'D',	   0,	0},			// 1111+30Bit		= 30Bit DWORD
};


static BITCODE queuePriceCode[] =	
{
	{2	,	2,  8,'B',		8,	  0},			// 10	+8Bit		= 8Bit
	{0	,	1, 12,'B',	   12,  256},			// 0	+12Bit		= 12Bit+256
	{6	,	3, 16,'B',	   16, 4352},			// 110	+16Bit		= 16Bit+4096+256
	{0xE,	4, 24,'B',	   24,69888},			// 1110	+24Bit		= 24Bit+65536+4096+256
	{0xF,	4, 30,'D',	    0,	  0},			// 1111	+30Bit		= 30Bit Org
};

static BITCODE queueNumberCode[] =	
{
	{0	,	2,  4,'B',		4,	  0},			// 00	+4Bit		= 4Bit
	{1	,	2,  8,'B',		8,	 16},			// 01	+8Bit		= 8Bit+16
	{2	,	2, 12,'B',	   12,  272},			// 10	+12Bit		= 12Bit+256+16
	{6	,	3, 16,'B',	   16, 4368},			// 110	+16Bit		= 16Bit+4096+256+16
	{7,		3, 30,'D',	    0,	  0},			// 111	+30Bit		= 30Bit Org
};



int CompressL2Queue(WORD wMarket,const DWORD* pQueue,int nQueueLen,BYTE* pBuf,int& nBufSize,BOOL bOrderQueue)
{
	int nRet = 0;
	int i;

	CBitStream stream;
	stream.SetBuffer(pBuf,nBufSize);

	try
	{
		stream.Put(wMarket,16);
		stream.Put(nQueueLen,16);

		BOOL bError = FALSE;
		int nStatus = 0;	//0:begin, 1:found stock id, 2:found price, 3:order vol
		for(i=0;i<nQueueLen && !bError;i++)
		{
			DWORD dw = pQueue[i]&(~QUEUE_TYPE_MASK);
			DWORD dwType = pQueue[i]&QUEUE_TYPE_MASK;

			stream.SETBITCODE(queueTypeCode);
			stream.EncodeData(dwType);

			switch(dwType)
			{
			case QUEUE_ORDER:			// 编码 0
				if(nStatus==2 || nStatus==3)
				{
					stream.SETBITCODE(queueVolCode);
					stream.EncodeData(dw);
					nStatus = 3;
				}
				else
					bError = TRUE;
				break;
			case QUEUE_BUYPRICE:		//编码 10
			case QUEUE_SELLPRICE:		//编码 110
				if(nStatus)
				{
					stream.SETBITCODE(queuePriceCode);
					stream.EncodeData(dw);
					if(i<nQueueLen-1)
					{
						stream.SETBITCODE(queueNumberCode);
						stream.EncodeData(pQueue[i+1]&(~QUEUE_TYPE_MASK));
						i++;
						nStatus = 2;
					}
					else
						bError = TRUE;
				}
				else
					bError = TRUE;
				break;
			case QUEUE_STOCKID:			//编码 111
				stream.Put(dw,16);
				nStatus = 1;
				break;
			}
		}
		if(!bError)
		{
			nRet = nQueueLen;
			nBufSize = (stream.GetCurPos()+7)/8;
		}
	}
	catch(...)
	{
		nRet = 0;
	}
	return nRet;
}

int ExpandL2Queue(const BYTE* pData,int nDataLen,DWORD* pQueueBuf,int& nQueueBufSize,BOOL bOrderQueue)
{
	int nRet = 0;
	int i;

	CBitStream stream;
	stream.SetBuffer((BYTE*)pData,nDataLen);

	try
	{
		unsigned short wMarket = (WORD)stream.Get(16);
		int nQueueLen = stream.Get(16);

		if(nQueueLen<=nQueueBufSize)
		{
			BOOL bError = FALSE;
			int nStatus = 0;	//0:begin, 1:found stock id, 2:found price, 3:order vol
			for(i=0;i<nQueueLen && !bError;i++)
			{
				stream.SETBITCODE(queueTypeCode);
				DWORD dwType = stream.DecodeData();
				DWORD dw = 0;
				switch(dwType)
				{
				case QUEUE_ORDER:			// 编码 0
					if(nStatus==2 || nStatus==3)
					{
						stream.SETBITCODE(queueVolCode);
						dw = stream.DecodeData();
						nStatus = 3;
					}
					else
						bError = TRUE;
					break;
				case QUEUE_BUYPRICE:		//编码 10
				case QUEUE_SELLPRICE:		//编码 110
					if(nStatus)
					{
						stream.SETBITCODE(queuePriceCode);
						dw = stream.DecodeData();
						nStatus = 2;
					}
					else
						bError = TRUE;
					break;
				case QUEUE_STOCKID:			//编码 111
					dw = stream.Get(16);
					nStatus = 1;
					break;
				default:
					bError = TRUE;
				}
				if(!bError)
				{
					pQueueBuf[i] = dw | dwType;
					if(nStatus==2)	//价格
					{
						if(i<nQueueLen-1)
						{
							stream.SETBITCODE(queueNumberCode);
							pQueueBuf[i+1] = stream.DecodeData();
							i++;
						}
						else
							bError = TRUE;
					}
				}
			}
			if(!bError)
			{
				nRet = nQueueLen;
			}
		}
		else
			nRet = -1;
		nQueueBufSize = nQueueLen;
	}
	catch(...)
	{
		nRet = 0;
	}

	return nRet;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//压缩即时最大撤单
static BITCODE numWDCode[] = 
{
	{0		,	1,	0,'E',	   10,	0},			// 0				1个
	{0x2	,	2,	8,'B',		8,	0},			// 10	+8Bit		8Bit
	{0x6	,	3, 16,'B',	   16, 256},		// 110	+16Bit		16Bit+256
	{0x7	,	3, 32,'D',		0,	0},			// 111	+16Bit		32Bit
};

static BITCODE wdTimeCode[] =
{
	{0		,	1,  0,'E',	    0,  0},			//0					= 基准
	{2		,	2,  4,'B',	    4,  1},			//10	+4Bit		= 基准 + 4Bit +1
	{6		,	3,  8,'B',	    8, 17},			//110	+8Bit		= 基准 + 8Bit + 16 + 1
	{0xE	,	4, 16,'B',	   16,273},			//1110	+16Bit		= 基准 + 8Bit + 256 + 16 + 1
	{0xF	,	4, 32,'D',	    0,	0},			//1111	+32Bit		= 32Bit org
};

static BITCODE wdSbumitTimeCode[] =
{
	{2		,	2,  8,'B',	    8,  0},			//10	+8Bit		= 基准 + 8Bit 
	{0		,	1, 16,'B',	   16,256},			//0		+16Bit		= 基准 + 16Bit + 256
	{3		,	2, 32,'D',	    0,	0},			//11	+32Bit		= 32Bit org
};

static BITCODE wdPriceCode[] =
{
	{2		,	2,	8,'B',		8,	0},			//10	+8Bit		= 8Bit
	{0		,	1, 16,'B',	   16,256},			//0		+16Bit		= 16Bit+256
	{0x6	,	3, 24,'B',	   24,65792},		//110	+24Bit		= 24Bit+65536+256
	{0x7	,	3, 32,'D',	   0,	0},			//111	+32Bit		= 32Bit Org
};

static BITCODE wdVolumeCode[] =	//量
{
	{0x0	,	2,  4,'Z',	   2,	1},			// 00 +2Bit+2Bit	= 2Bit*10^2Bit
	{0x1	,	2,  6,'Z',	   4,	5},			// 01 +4Bit+2Bit	= (4Bit+4)*10^2Bit
	{0x2	,	2,  8,'B',	   8,   0},			// 10 +8Bit			= 8Bit
	{0x6	,	3, 12,'B',	  12, 256},			// 110+12Bit		= 12Bit+256
	{0xE	,	4, 16,'B',	  16,4352},			// 1110+16Bit		= 16Bit+4096+256
	{0xF	,	4, 32,'D',	   0,	0},			// 1111+32Bit		= 32Bit DWORD
};


int CompressL2BigWD(WORD wMarket,const DC_SHL2_BIG_WD* pWD,BYTE* pBuf,int& nBufSize)
{
	int nRet = 0;
	int i;

	if(pWD && pBuf && pWD->m_nNum)
	{
		CBitStream stream;
		stream.SetBuffer(pBuf,nBufSize);

		time_t tBase = 0x7FFFFFFF;
		for(i=0;i<pWD->m_nNum;i++)
			tBase = Min(tBase,pWD->m_data[i].m_time);

		try
		{
			stream.Put(wMarket,16);
			i = pWD->m_cBidOrAsk=='B' ? 1 : 0;
			stream.Put(i,1);
			stream.Put(tBase,32);

			stream.SETBITCODE(numWDCode);
			stream.EncodeData(pWD->m_nNum);

			for(i=0;i<pWD->m_nNum;i++)
			{
				const SH_L2_BIG_WD& wd = pWD->m_data[i];
				stream.Put(wd.m_wStkID,16);

				stream.SETBITCODE(wdTimeCode);
				stream.EncodeData(wd.m_time,(DWORD*)&tBase);
				
				stream.SETBITCODE(wdSbumitTimeCode);
				stream.EncodeData(wd.m_submitTime,(DWORD*)&wd.m_time,TRUE);

				stream.SETBITCODE(wdPriceCode);
				stream.EncodeData(wd.m_dwPrice);

				stream.SETBITCODE(wdVolumeCode);
				stream.EncodeData(wd.m_dwWidthdrawVol);
			}
			nBufSize = (stream.GetCurPos()+7)/8;
			nRet = 1;
		}
		catch(...)
		{
		}
	}
	return nRet;
}

int ExpandL2BigWD(const BYTE* pData,int nDataLen,WORD& wMarket,DC_SHL2_BIG_WD* pWDBuf,int& nBufSize)
{
	int nRet = 0;
	int i;

	CBitStream stream;
	stream.SetBuffer((BYTE*)pData,nDataLen);

	if(pWDBuf)
	{
		try
		{
			wMarket = (WORD)stream.Get(16);
			pWDBuf->m_cBidOrAsk = stream.Get(1) ? 'B' : 'A';
			time_t tBase = stream.Get(32);

			stream.SETBITCODE(numWDCode);
			pWDBuf->m_nNum = (short)stream.DecodeData();

			if(nBufSize>=(int)(sizeof(DC_SHL2_BIG_WD)+(pWDBuf->m_nNum-1)*sizeof(SH_L2_BIG_WD))) 
			{
				for(i=0;i<pWDBuf->m_nNum;i++)
				{
					SH_L2_BIG_WD& wd = pWDBuf->m_data[i];
					wd.m_wStkID = (WORD)stream.Get(16);

					stream.SETBITCODE(wdTimeCode);
					wd.m_time = stream.DecodeData((DWORD*)&tBase);
					
					stream.SETBITCODE(wdSbumitTimeCode);
					wd.m_submitTime = stream.DecodeData((DWORD*)&wd.m_time,TRUE);

					stream.SETBITCODE(wdPriceCode);
					wd.m_dwPrice = stream.DecodeData();

					stream.SETBITCODE(wdVolumeCode);
					wd.m_dwWidthdrawVol = stream.DecodeData();
				}
				nRet = sizeof(DC_SHL2_BIG_WD)+(pWDBuf->m_nNum-1)*sizeof(SH_L2_BIG_WD);
			}
			else
				nRet = -1;
			nBufSize = sizeof(DC_SHL2_BIG_WD)+(pWDBuf->m_nNum-1)*sizeof(SH_L2_BIG_WD);
		}
		catch(...)
		{
			nRet = 0;
		}
	}
	return nRet;
}

//买卖两个即时最大撤单压缩到一起
int CompressL2BigWD(WORD wMarket,const DC_SHL2_BIG_WD* pBuyWD,const DC_SHL2_BIG_WD* pSellWD,SVRNetHead* pBuf,int& nBufSize)
{
	int nRet = 0;
	int i;

	if(pBuyWD && pSellWD && pBuf && (pBuyWD->m_nNum || pSellWD->m_nNum))
	{
		CBitStream stream;
		stream.SetBuffer((BYTE*)(pBuf+1),nBufSize);

		time_t tBase = 0x7FFFFFFF;
		for(i=0;i<pBuyWD->m_nNum;i++)
			tBase = Min(tBase,pBuyWD->m_data[i].m_time);
		for(i=0;i<pSellWD->m_nNum;i++)
			tBase = Min(tBase,pSellWD->m_data[i].m_time);

		try
		{
			stream.Put(wMarket,16);
			stream.Put(tBase,32);

			stream.SETBITCODE(numWDCode);
			stream.EncodeData(pBuyWD->m_nNum);
			stream.EncodeData(pSellWD->m_nNum);

			for(int j=0;j<2;j++)
			{
				const DC_SHL2_BIG_WD* pWD = j ? pSellWD : pBuyWD;
				for(i=0;i<pWD->m_nNum;i++)
				{
					const SH_L2_BIG_WD& wd = pWD->m_data[i];
					stream.Put(wd.m_wStkID,16);

					stream.SETBITCODE(wdTimeCode);
					stream.EncodeData(wd.m_time,(DWORD*)&tBase);
					
					stream.SETBITCODE(wdSbumitTimeCode);
					stream.EncodeData(wd.m_submitTime,(DWORD*)&wd.m_time,TRUE);

					stream.SETBITCODE(wdPriceCode);
					stream.EncodeData(wd.m_dwPrice);

					stream.SETBITCODE(wdVolumeCode);
					stream.EncodeData(wd.m_dwWidthdrawVol);
				}
			}
			nBufSize = (stream.GetCurPos()+7)/8;
			nRet = 1;
			pBuf->m_type = CLNT_L2_WD;
			pBuf->m_nLen = nBufSize;
			pBuf->m_wAttrib = DC_STD_CPS;
			nBufSize += sizeof(SVRNetHead);
		}
		catch(...)
		{
		}
	}
	return nRet;
}


//int ExpandL2BigWD(const SVRNetHead* pData,WORD& wMarket,SH_L2_BIG_WD* pBuyBuf,int& nBuyBufSize,SH_L2_BIG_WD* pSellBuf,int& nSellBufSize)
//{
//	return ExpandL2BigWD(pData,(BYTE*)(pData+1),wMarket,pBuyBuf,nBuyBufSize,pSellBuf,nSellBufSize);
//}

//int ExpandL2BigWD(const SVRNetHead* pHead,const BYTE* pData,WORD& wMarket,SH_L2_BIG_WD* pBuyBuf,int& nBuyBufSize,SH_L2_BIG_WD* pSellBuf,int& nSellBufSize)
//{
//	int nRet = 0;
//	int i;
//
//	if(pHead->m_type==CLNT_L2_WD && (pHead->m_wAttrib&DC_CPS_MASK)==DC_STD_CPS && pBuyBuf && pSellBuf)
//	{
//		CBitStream stream;
//		stream.SetBuffer((BYTE*)pData,pHead->m_nLen);
//
//		try
//		{
//			wMarket = (WORD)stream.Get(16);
//			time_t tBase = stream.Get(32);
//
//			stream.SETBITCODE(numWDCode);
//			int nNumBuy = stream.DecodeData();
//			int nNumSell = stream.DecodeData();
//
//			for(int j=0;j<2;j++)
//			{
//				SH_L2_BIG_WD* pWDBuf = j ? pSellBuf : pBuyBuf;
//				int nBufSize = j ? nSellBufSize : nBuyBufSize;
//				int nNum = j ? nNumSell : nNumBuy;
//				if(nBufSize>=nNum) 
//				{
//					for(i=0;i<nNum;i++)
//					{
//						SH_L2_BIG_WD& wd = pWDBuf[i];
//						wd.m_wStkID = (WORD)stream.Get(16);
//
//						stream.SETBITCODE(wdTimeCode);
//						wd.m_time = stream.DecodeData((DWORD*)&tBase);
//						
//						stream.SETBITCODE(wdSbumitTimeCode);
//						wd.m_submitTime = stream.DecodeData((DWORD*)&wd.m_time,TRUE);
//
//						stream.SETBITCODE(wdPriceCode);
//						wd.m_dwPrice = stream.DecodeData();
//
//						stream.SETBITCODE(wdVolumeCode);
//						wd.m_dwWidthdrawVol = stream.DecodeData();
//					}
//				}
//				else
//					nRet = -1;
//				if(j)
//					nSellBufSize = nNum;
//				else
//					nBuyBufSize = nNum;
//			}
//			if(nRet==0)
//			{
//				nRet = 1;
//			}
//		}
//		catch(...)
//		{
//		}
//	}
//	return nRet;
//}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//压缩累计最大撤单
int CompressL2AccuWD(WORD wMarket,const DC_SHL2_ACCU_WD* pWD,BYTE* pBuf,int& nBufSize)
{
	int nRet = 0;
	int i;

	if(pWD && pBuf && pWD->m_nNum)
	{
		CBitStream stream;
		stream.SetBuffer(pBuf,nBufSize);

		time_t tBase = 0x7FFFFFFF;
		for(i=0;i<pWD->m_nNum;i++)
			tBase = Min(tBase,pWD->m_data[i].m_time);

		try
		{
			stream.Put(wMarket,16);
			i = pWD->m_cBidOrAsk=='B' ? 1 : 0;
			stream.Put(i,1);
			stream.Put(tBase,32);

			stream.SETBITCODE(numWDCode);
			stream.EncodeData(pWD->m_nNum);

			for(i=0;i<pWD->m_nNum;i++)
			{
				const SH_L2_ACCU_WD& wd = pWD->m_data[i];
				stream.Put(wd.m_wStkID,16);

				stream.SETBITCODE(wdTimeCode);
				stream.EncodeData(wd.m_time,(DWORD*)&tBase);

				stream.SETBITCODE(wdVolumeCode);
				stream.EncodeData(wd.m_dwWidthdrawVol);
			}
			nBufSize = (stream.GetCurPos()+7)/8;
			nRet = 1;
		}
		catch(...)
		{
		}
	}
	return nRet;
}

int ExpandL2AccuWD(const BYTE* pData,int nDataLen,WORD& wMarket,DC_SHL2_ACCU_WD* pWDBuf,int& nBufSize)
{
	int nRet = 0;
	int i;

	CBitStream stream;
	stream.SetBuffer((BYTE*)pData,nDataLen);

	if(pWDBuf)
	{
		try
		{
			wMarket = (WORD)stream.Get(16);
			pWDBuf->m_cBidOrAsk = stream.Get(1) ? 'B' : 'A';
			time_t tBase = stream.Get(32);

			stream.SETBITCODE(numWDCode);
			pWDBuf->m_nNum = (short)stream.DecodeData();

			if(nBufSize>=(int)(sizeof(DC_SHL2_ACCU_WD)+(pWDBuf->m_nNum-1)*sizeof(SH_L2_ACCU_WD))) 
			{
				for(i=0;i<pWDBuf->m_nNum;i++)
				{
					SH_L2_ACCU_WD& wd = pWDBuf->m_data[i];
					wd.m_wStkID = (WORD)stream.Get(16);

					stream.SETBITCODE(wdTimeCode);
					wd.m_time = stream.DecodeData((DWORD*)&tBase);
					
					stream.SETBITCODE(wdVolumeCode);
					wd.m_dwWidthdrawVol = stream.DecodeData();
				}
				nRet = sizeof(DC_SHL2_ACCU_WD)+(pWDBuf->m_nNum-1)*sizeof(SH_L2_ACCU_WD);
			}
			else
				nRet = -1;
			nBufSize = sizeof(DC_SHL2_ACCU_WD)+(pWDBuf->m_nNum-1)*sizeof(SH_L2_ACCU_WD);
		}
		catch(...)
		{
			nRet = 0;
		}
	}
	return nRet;
}

int CompressL2AccuWD(WORD wMarket,const DC_SHL2_ACCU_WD* pBuyWD,const DC_SHL2_ACCU_WD* pSellWD,SVRNetHead* pBuf,int& nBufSize)
{
	int nRet = 0;
	int i;

	if(pBuyWD && pSellWD && pBuf && (pBuyWD->m_nNum || pSellWD->m_nNum))
	{
		CBitStream stream;
		stream.SetBuffer((BYTE*)(pBuf+1),nBufSize);

		time_t tBase = 0x7FFFFFFF;
		for(i=0;i<pBuyWD->m_nNum;i++)
			tBase = Min(tBase,pBuyWD->m_data[i].m_time);
		for(i=0;i<pSellWD->m_nNum;i++)
			tBase = Min(tBase,pSellWD->m_data[i].m_time);

		try
		{
			stream.Put(wMarket,16);
			stream.Put(tBase,32);

			stream.SETBITCODE(numWDCode);
			stream.EncodeData(pBuyWD->m_nNum);
			stream.EncodeData(pSellWD->m_nNum);

			for(int j=0;j<2;j++)
			{
				const DC_SHL2_ACCU_WD* pWD = j ? pSellWD : pBuyWD;
				for(i=0;i<pWD->m_nNum;i++)
				{
					const SH_L2_ACCU_WD& wd = pWD->m_data[i];
					stream.Put(wd.m_wStkID,16);

					stream.SETBITCODE(wdTimeCode);
					stream.EncodeData(wd.m_time,(DWORD*)&tBase);
					
					stream.SETBITCODE(wdVolumeCode);
					stream.EncodeData(wd.m_dwWidthdrawVol);
				}
			}
			nBufSize = (stream.GetCurPos()+7)/8;
			nRet = 1;
			pBuf->m_type = CLNT_L2_ACCU_WD;
			pBuf->m_nLen = nBufSize;
			pBuf->m_wAttrib = DC_STD_CPS;
			nBufSize += sizeof(SVRNetHead);
		}
		catch(...)
		{
		}
	}
	return nRet;
}

//int ExpandL2AccuWD(const SVRNetHead* pData,WORD& wMarket,SH_L2_ACCU_WD* pBuyBuf,int& nBuyBufSize,SH_L2_ACCU_WD* pSellBuf,int& nSellBufSize)
//{
//	return ExpandL2AccuWD(pData,(BYTE*)(pData+1),wMarket,pBuyBuf,nBuyBufSize,pSellBuf,nSellBufSize);
//}

//int ExpandL2AccuWD(const SVRNetHead* pHead,const BYTE* pData,WORD& wMarket,SH_L2_ACCU_WD* pBuyBuf,int& nBuyBufSize,SH_L2_ACCU_WD* pSellBuf,int& nSellBufSize)
//{
//	int nRet = 0;
//	int i;
//
//	if(pHead->m_type==CLNT_L2_ACCU_WD && (pHead->m_wAttrib&DC_CPS_MASK)==DC_STD_CPS && pBuyBuf && pSellBuf)
//	{
//		CBitStream stream;
//		stream.SetBuffer((BYTE*)pData,pHead->m_nLen);
//
//		try
//		{
//			wMarket = (WORD)stream.Get(16);
//			time_t tBase = stream.Get(32);
//
//			stream.SETBITCODE(numWDCode);
//			int nNumBuy = stream.DecodeData();
//			int nNumSell = stream.DecodeData();
//
//			for(int j=0;j<2;j++)
//			{
//				SH_L2_ACCU_WD* pWDBuf = j ? pSellBuf : pBuyBuf;
//				int nBufSize = j ? nSellBufSize : nBuyBufSize;
//				int nNum = j ? nNumSell : nNumBuy;
//				if(nBufSize>=nNum)
//				{
//					for(i=0;i<nNum;i++)
//					{
//						SH_L2_ACCU_WD& wd = pWDBuf[i];
//						wd.m_wStkID = (WORD)stream.Get(16);
//
//						stream.SETBITCODE(wdTimeCode);
//						wd.m_time = stream.DecodeData((DWORD*)&tBase);
//						
//						stream.SETBITCODE(wdVolumeCode);
//						wd.m_dwWidthdrawVol = stream.DecodeData();
//					}
//				}
//				else
//					nRet = -1;
//				if(j)
//					nSellBufSize = nNum;
//				else
//					nBuyBufSize = nNum;
//			}
//			if(nRet==0)
//			{
//				nRet = 1;
//			}
//		}
//		catch(...)
//		{
//		}
//	}
//	return nRet;
//}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//SZ_L2_ORDER_STAT

void SZ_L2_ORDER_STAT::Init(WORD wStkID)
{
	memset(this,0,sizeof(SZ_L2_ORDER_STAT));
	m_wStkID = wStkID;
}

//新增委托单时调用，nPriceDigit引用 STK_STATIC::m_nPriceDigit
void SZ_L2_ORDER_STAT::AddNewOrder(int nDir,DWORD dwPrice,DWORD dwVol,BYTE nPriceDigit)
{
	static DWORD dwVolLimit[] = {0,20000,100000,500000};
	static DWORD dwAmountLimit[] = {0,40000,100000,1000000};

	int i;
	DWORD dwAmount = dwPrice*dwVol;
	while(nPriceDigit--)
		dwAmount /= 10;
	for(i=3;i>=0;i--)
	{
		if(dwVol>=dwVolLimit[i] || dwAmount>=dwAmountLimit[i])
		{
			if(nDir>=0)
			{
				m_nBuyOrderVol[i] += dwVol;
				m_dwOrderNum[0] = m_dwOrderNum[0]+1;
			}
			else
			{
				m_nSellOrderVol[i] += dwVol;
				m_dwOrderNum[1] = m_dwOrderNum[1]+1;
			}
			break;
		}
	}
}

//新增成功撤单时调用，nDir=0表示不明方向，1表示买入撤单，-1表示卖出撤单,dwPrice不知道就填写0
void SZ_L2_ORDER_STAT::AddNewWithdarw(int nDir,DWORD dwVol)
{
	if(nDir)
		m_nWDVol[nDir>0 ? 0 : 1] += dwVol;
	else
	{
		m_nWDVol[0] += dwVol/2;
		m_nWDVol[1] += dwVol/2;
	}
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static BITCODE osIDCode[] =	//StockID
{
	{0		,	1,	0,'E',		1,	0},			// 0				顺序
	{0x2	,	2,	6,'B',		6,	2},			// 10	+6Bit		顺序,6Bit+2
	{0x3	,	2, 16,'D',		0,	0},			// 11	+16Bit		直接存放wStockID
};

static BITCODE osVolCode[] =	//量
{
	{0x0	,	2, 0, 'E',	   2,	0},			// 00				= 0
	{0x1	,	2, 12,'B',	   12,	0},			// 01	+12Bit		= 基准+12Bit
	{0x2	,	2, 16,'B',	   16,4096},		// 10	+16Bit		= 基准+16Bit+4096
	{0x6	,	3, 24,'B',	   24,69632},		// 110	+24Bit		= 基准+24Bit+65536+4096
	{0x7	,	3, 32,'M',	    0,	0},			// 111  +32Bit		= 32Bit Org
};

static BITCODE osCountCode[] =	//单数
{
	{0x0	,	1, 12,'B',	   12,	0},			// 0	+12Bit		= 基准+12Bit
	{0x2	,	2, 16,'B',	   16,4096},		// 10	+16Bit		= 基准+16Bit+4096
	{0x3	,	2, 24,'D',	    0,  0},			// 11	+24Bit		= 24Bit Org
};


//压缩逐笔委托统计
BOOL CompressL2OrderStat(WORD wMarket,const DC_SZL2_ORDER_STAT* pStat,BYTE* pBuf,int& nBufSize)
{
	BOOL bRet = FALSE;

	CBitStream	stream(pBuf,nBufSize);

	try
	{
		stream.Put(wMarket,16);
		stream.Put(pStat->m_nNum,16);
		stream.Put(pStat->m_time,32);

		int i,j,nNum;
		nNum = pStat->m_nNum;
		BOOL bHasID = FALSE;
		for(i=0;i<nNum;i++)
		{
			if(pStat->m_data[i].m_wStkID!=(WORD)i)
			{
				bHasID = TRUE;
				break;
			}
		}
		stream.Put(bHasID,1);


		DWORD	dwLastID = 0;
		for(i=0;i<nNum;i++)
		{
			SZ_L2_ORDER_STAT* pCur = (SZ_L2_ORDER_STAT*)pStat->m_data+i;

			if(bHasID)
			{
				stream.SETBITCODE(osIDCode);
				stream.EncodeData(pCur->m_wStkID,&dwLastID);
				dwLastID = pCur->m_wStkID;
			}

			stream.SETBITCODE(osVolCode);

			for(j=0;j<4;j++)
			{
				stream.EncodeData(pCur->m_nBuyOrderVol[j]);
				stream.EncodeData(pCur->m_nSellOrderVol[j]);
				if(j<2)
					stream.EncodeData(pCur->m_nWDVol[j]);
			}

			stream.SETBITCODE(osCountCode);

			for(j=0;j<2;j++)
				stream.EncodeData(pCur->m_dwOrderNum[j]);
		}
		nBufSize = stream.GetByteLength();
		bRet = TRUE;
	}
	catch(...)
	{
		bRet = FALSE;
	}

	return bRet;

}

int ExpandL2OrderStat(const BYTE* pData,int nDataLen,DC_SZL2_ORDER_STAT* pStatBuf,int& nBufSize)
{
	int nRet = 0;

	BOOL bRet = FALSE;

	CBitStream	stream((BYTE*)pData,nDataLen);

	try
	{
		stream.Get(16);
		pStatBuf->m_nNum = (short)stream.Get(16);
		pStatBuf->m_time = stream.Get(32);

		if(nBufSize>=(int)((pStatBuf->m_nNum-1)*sizeof(SZ_L2_ORDER_STAT)+sizeof(DC_SZL2_ORDER_STAT)))
		{
			int i,j,nNum;
			nNum = pStatBuf->m_nNum;
			BOOL bHasID = stream.Get(1);


			DWORD	dwLastID = 0;
			for(i=0;i<nNum;i++)
			{
				SZ_L2_ORDER_STAT* pCur = pStatBuf->m_data+i;

				if(bHasID)
				{
					stream.SETBITCODE(osIDCode);
					pCur->m_wStkID = (WORD)stream.DecodeData(&dwLastID);
					dwLastID = pCur->m_wStkID;
				}
				else
					pCur->m_wStkID = (WORD)i;

				stream.SETBITCODE(osVolCode);

				for(j=0;j<4;j++)
				{
					pCur->m_nBuyOrderVol[j] = stream.DecodeMWordData();
					pCur->m_nSellOrderVol[j] = stream.DecodeMWordData();
					if(j<2)
						pCur->m_nWDVol[j] = stream.DecodeMWordData();
				}

				stream.SETBITCODE(osCountCode);

				for(j=0;j<2;j++)
					pCur->m_dwOrderNum[j] = stream.DecodeData();
			}
			nBufSize = (nNum-1)*sizeof(SZ_L2_ORDER_STAT)+sizeof(DC_SZL2_ORDER_STAT);
			nRet = nNum;
		}
	}
	catch(...)
	{
		nRet = 0;
	}
	return nRet;
}
