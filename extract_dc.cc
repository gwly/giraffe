#include "extract_dc.h"
#include "l2compress.h"
#include "generalcps.h"

int ExtractDC::ExtractData(int dc_type, const unsigned char *pbufsrc, int bufsizesrc, unsigned char * pbufdes, int bufsizedes, STK_DYNA *pStkDyna, STK_STATIC *pStkStatic)
{
 	int nRet = 0;
	int nNum = 0;
	int nExtraHeadLen = 0;
	int nSize;
	//cout<<"dc_type:::::"<<dc_type<<endl;
	switch(dc_type)
	{
	case DCT_STKSTATIC:
	{
		static_tag = false;
	   /* cout<<"DCT_STKSTATIC"<<endl;*/

		DC_STKSTATIC* pOrgStatic = (DC_STKSTATIC*)pbufsrc;
		DC_STKSTATIC* pDestStatic = (DC_STKSTATIC*)pbufdes;
		pDestStatic->m_dwVersion = pOrgStatic->m_dwVersion;
		pDestStatic->m_wAttrib = pOrgStatic->m_wAttrib;
		pDestStatic->m_nDay = pOrgStatic->m_nDay;
//			cout<<"dct static attrib:"<<pOrgStatic->m_wAttrib<<endl;
//			cout<<"dct static version:"<<pOrgStatic->m_dwVersion<<endl;
//			cout<<"dct static day:"<<pOrgStatic->m_nDay<<endl;
		nExtraHeadLen = sizeof(DC_STKSTATIC) - sizeof(WORD);
		nSize = (bufsizedes - nExtraHeadLen)/sizeof(STK_STATIC);
		nNum = ExtractStaticData((BYTE*)&(pOrgStatic->m_nNum),bufsizesrc-nExtraHeadLen,pDestStatic->m_data,nSize);
		
		if(nNum>0)
		{
			nRet = sizeof(DC_STKSTATIC)+(nNum-1)*sizeof(STK_STATIC);
			pDestStatic->m_nNum = nNum;
		}
		break;
	}
	case DCT_STKDYNA:
	{
		DC_STKDYNA* pOrgDyna = (DC_STKDYNA*)pbufsrc;
		DC_STKDYNA* pDestDyna = (DC_STKDYNA*)pbufdes;

		if(static_before_dyna_tag)
		{
			/*cout<<"DCT_STKDYNA"<<endl;*/
		
			pDestDyna->m_wDynaSeq = pOrgDyna->m_wDynaSeq;
			m_compressType_ = DYNA;
			nExtraHeadLen = sizeof(WORD);
			nSize = (bufsizedes - nExtraHeadLen)/sizeof(STK_DYNA);
			nNum = ExtractDynaData((BYTE*)&pOrgDyna->m_nNum,bufsizesrc-nExtraHeadLen,pDestDyna->m_data,nSize,pStkDyna,pStkStatic);
			static_tag = true;
		}
		else
		{
			nNum = 0;
			nRet = 0;
		}
		if(nNum>0)
		{
			nRet = sizeof(DC_STKDYNA)+(nNum-1)*sizeof(STK_DYNA);
			pDestDyna->m_nNum = nNum;
		}
		else if(nNum==-3)	//校验错误
		{
			nRet = -3;
		}
		break;
	}
	case DCT_SHL2_MMPEx:
	{
		DC_SHL2_MMPEx* pMMPEx = (DC_SHL2_MMPEx*)pbufdes;
		nSize = (bufsizedes-sizeof(DC_SHL2_MMPEx)+sizeof(pMMPEx->m_data))/sizeof(DC_SHL2_MMPEx);
		if(ExpandL2MMPEx(pbufsrc,bufsizesrc,pMMPEx->m_data,nSize)>0)
		{
			pMMPEx->m_nNum = nSize;
			nRet = sizeof(DC_SHL2_MMPEx) + (nSize-1)*sizeof(SH_L2_MMPEX);
		}
		break;
	}
	//case DCT_SHL2_REPORT:
	//	{
//			DC_SHL2_REPORT* pL2Rep = (DC_SHL2_REPORT*)pbufdes;
//			nSize = (bufsizedes-sizeof(DC_SHL2_REPORT)+sizeof(SH_L2_REPORT))/sizeof(SH_L2_REPORT);
//			nNum = ExpandL2Report(pbufsrc,bufsizesrc,wMarket,pL2Rep->m_data,nSize);
//			if(nNum>0)
//			{
//				pL2Rep->m_nNum = nNum;
//				nRet = sizeof(DC_SHL2_REPORT)+(nNum-1)*sizeof(SH_L2_REPORT);
//			}
//			else
//				nRet = 0;
	//	}
	//	break;
	case DCT_SHL2_QUEUE:
	case DCT_SZL2_ORDER_QUEUE:
	{
		nSize = bufsizedes/sizeof(DWORD);
		if(ExpandL2Queue(pbufsrc,bufsizesrc,(DWORD*)pbufdes,nSize,dc_type==DCT_SZL2_ORDER_QUEUE)>0)
			nRet = nSize*sizeof(DWORD);

		break;
	}
	//case DCT_SHL2_BIG_WD:
	//	{
//			nSize = bufsizedes;
//			if(ExpandL2BigWD(pbufsrc,bufsizesrc,wMarket,(DC_SHL2_BIG_WD*)pbufdes,nSize)>0)
//				nRet = nSize;
	//	}
	//	break;
	//case DCT_SHL2_ACCU_WD:
	//	{
//			nSize = bufsizedes;
//			if(ExpandL2AccuWD(pbufsrc,bufsizesrc,wMarket,(DC_SHL2_ACCU_WD*)pbufdes,nSize)>0)
//				nRet = nSize;
	//	}
	//	break;
	case DCT_GENERAL:
		{
			nSize = bufsizedes;
			nRet = ExpandGeneralData(pbufsrc,bufsizesrc,(DC_GENERAL*)pbufdes,nSize);
			if(nRet<0)
				nRet = 0;
		}
		break;
	case DCT_SZL2_ORDER_STAT:
		{
			DC_SZL2_ORDER_STAT* pStat = (DC_SZL2_ORDER_STAT*)pbufdes;
			nSize = bufsizedes;
			nNum = ExpandL2OrderStat(pbufsrc,bufsizesrc,pStat,nSize);
			if(nNum>0)
				nRet = nSize;
			else
				nRet = 0;
		}
		break;
	default:
		break;
	}
	return nRet;
}

int ExtractDC::ExtractStaticData(const BYTE* pData,int nDataLen,STK_STATIC* pStaticBuf,int& nBufSize)
{
	int nRet = 0;

    m_pStaticHead = (CPS_STATIC_HEAD*)pData;
	m_wMarket = m_pStaticHead->m_wMarket;
	int nNum = (int)m_pStaticHead->m_wNum;
//	cout<<"static compress head market:"<<m_wMarket<<endl;
//	cout<<"static  compress head num"<<nNum<<endl;
//	cout<<"static  compress head attrib:"<<(int)m_pStaticHead->m_cAttrib<<endl;
//	cout<<"static compress head addr attrib:"<<&(m_pStaticHead->m_cAttrib)<<endl;
	if(nNum<=nBufSize)
	{
		m_stream.SetBuffer((BYTE*)pData+sizeof(CPS_STATIC_HEAD),nDataLen-sizeof(CPS_STATIC_HEAD));

		m_bFullData = m_pStaticHead->m_cAttrib&CPS_STATIC_HEAD::FULLDATA;
		WORD wStockID = m_pStaticHead->m_cAttrib&CPS_STATIC_HEAD::SEQ_ID ? (WORD)-1 : 0;
		DWORD dwLastLabel = 0;
		int i;
		for(i=0;i<nNum;i++)
		{
			if(!ExtractStaticStream(pStaticBuf+i,wStockID,dwLastLabel))
				break;
		}
		nRet = i;
	}
	else
		nRet = -1;
	nBufSize = nNum;
	return nRet;
}

int ExtractDC::ExtractDynaData(const BYTE* pData,int nDataLen,STK_DYNA* pDynaBuf,int& nBufSize,STK_DYNA *pStkDyna,STK_STATIC *pStkStatic)
{
 	int nRet = 0;

	CPS_DYNA_HEAD* pDynaHead = (CPS_DYNA_HEAD*)pData;
//	cout<<"getCRC16 len:"<<nDataLen<<endl;
	WORD wCRC = GetCrc16(pData+2,nDataLen-2);
	//cout<<"wCRC:"<<(int)((BYTE)wCRC)<<endl;
	//cout<<"pDynahead->m_cCRC:"<<(int)(pDynaHead->m_cCRC)<<endl;
    m_wMarket = pDynaHead->m_wMarket;
	if((BYTE)wCRC==pDynaHead->m_cCRC)
	{
		m_cHeadAttr_ = pDynaHead->m_cAttrib;
		m_baseTime_ = pDynaHead->m_baseTime;
		int nNumDyna = (int)pDynaHead->m_wNum;
		/*	cout<<"dyna stock total:"<<nNumDyna<<endl;
		//cout<<"nbufsize:"<<nBufSize<<endl;*/
		if(nNumDyna<=nBufSize)
		{
			//cout<<"pDynaHead->m_cVer:"<<pDynaHead->m_cVer<<endl;*/
			//if(pDynaHead->m_cVer>CUR_DYNACPS_VER)
			//	nRet = -2;
			//else
			//{
				CBitStream stream;
				stream.SetBuffer((BYTE*)pData+sizeof(CPS_DYNA_HEAD),nDataLen-sizeof(CPS_DYNA_HEAD));

				m_bSeqCompress_ = pDynaHead->m_cAttrib&CPS_DYNA_HEAD::SEQ_CPS;
				WORD wStockID = 0;
				int i;
				for(i=0;i<nNumDyna;i++)
				{
					if(!ExtractDynaStream(stream,pDynaBuf+i,wStockID,pDynaHead->m_cVer,pStkDyna,pStkStatic))
						break;
				}
				nRet = i;
	/*		}*/
		}
		else
			nRet = -1;
		nBufSize = nNumDyna;
	}
	else
		nRet = -3;
	return nRet;
}

BOOL ExtractDC::ExtractStaticStream(STK_STATIC* pStatic,WORD& wStockID,DWORD& dwLastLabel)
{
	BOOL bRet = FALSE;
	char str[256] = {};
	DWORD dwBase,dw;

	memset(pStatic,0,sizeof(STK_STATIC));
	m_stream.SaveCurrentPos();

	try	//开始解码
	{
		//StockID
//		cout<<"CPS_STATIC_HEAD::SEQ_ID:"<<CPS_STATIC_HEAD::SEQ_ID<<endl;
//		cout<<"addr m_pStaticHead->m_cAttrib:"<<&(m_pStaticHead->m_cAttrib)<<endl;
//		cout<<"m_pStaticHead->m_cAttrib:"<<(int)(m_pStaticHead->m_cAttrib)<<endl;
		if(m_pStaticHead->m_cAttrib&CPS_STATIC_HEAD::SEQ_ID)
			pStatic->m_wStkID = wStockID+1;
		else
		{
			m_stream.SETBITCODE(stkIDCode);
			dwBase = wStockID;
//			cout<<"dwbase:"<<dwBase<<endl;
			pStatic->m_wStkID = (WORD)m_stream.DecodeData(&dwBase);
		}
		wStockID = pStatic->m_wStkID;
//        cout<<"static stkid:"<<pStatic->m_wStkID<<endl;
		//代码
		m_stream.SETBITCODE(labelCode);
		dw = m_stream.DecodeStringData(str,sizeof(str),dwLastLabel);
		strcpy(pStatic->m_strLabel,str);
//        cout<<"static lable:"<<pStatic->m_strLabel<<endl;
		//昨收
		m_stream.SETBITCODE(priceCode);
		pStatic->m_dwLastClose = m_stream.DecodeData();
//        cout<<"static lastclose:"<<pStatic->m_dwLastClose<<endl;
		//涨停
		m_stream.SETBITCODE(updnPriceCode);
		dwBase = pStatic->m_dwLastClose*11/10;
		pStatic->m_dwAdvStop = m_stream.DecodeData(&dwBase);
		if(pStatic->m_dwAdvStop==9999999+dwBase)
			pStatic->m_dwAdvStop = 0;
		else if(pStatic->m_dwAdvStop==9999998+dwBase)
			pStatic->m_dwAdvStop = 9999900;
		else if(pStatic->m_dwAdvStop==9999997+dwBase)
			pStatic->m_dwAdvStop = 99999000;
//        cout<<"static advstop:"<<pStatic->m_dwAdvStop<<endl;
		//跌停
		dwBase = pStatic->m_dwLastClose*9/10;
		pStatic->m_dwDecStop = m_stream.DecodeData(&dwBase);
		if(pStatic->m_dwDecStop==9999999+dwBase)
			pStatic->m_dwDecStop = 0;
		else if(pStatic->m_dwDecStop==9999998+dwBase)
			pStatic->m_dwDecStop = 1;
		else if(pStatic->m_dwDecStop==9999997+dwBase)
			pStatic->m_dwDecStop = 10;
//        cout<<"static decstop:"<<pStatic->m_dwDecStop<<endl;

		if(m_pStaticHead->m_cAttrib&CPS_STATIC_HEAD::FULLDATA)
		{
			//名称
			m_stream.GetString(str,sizeof(str));
			strncpy(pStatic->m_strName,str,sizeof(pStatic->m_strName)-1);

			//类型
			pStatic->m_cType = (BYTE)m_stream.Get(6);

			//价格最小分辨率
			m_stream.SETBITCODE(priceDigitCode);
			pStatic->m_nPriceDigit = (BYTE)m_stream.DecodeData();

			//成交量单位
			m_stream.SETBITCODE(volUnitCode);
			pStatic->m_nVolUnit = (short)m_stream.DecodeData();

			//流通股本
			m_stream.SETBITCODE(issudeCode);
			pStatic->m_mFloatIssued = m_stream.DecodeMWordData();
			//总股本
			pStatic->m_mTotalIssued = m_stream.DecodeMWordData();
		}
		else
		{
			//CRC
			WORD wCRC = (WORD)m_stream.Get(16);
			*(WORD*)(pStatic->m_strName+1) = wCRC;
		}
		bRet = TRUE;
	}
	catch(...)
	{
		m_stream.RestoreToSavedPos();
	}
	return bRet;
}

 BOOL ExtractDC::ExtractDynaStream(CBitStream& stream,STK_DYNA* pDyna,WORD& wStockID,BYTE cVersion,STK_DYNA *pStkDyna,STK_STATIC *pStkStatic,const STK_DYNA* pOldDyna,const STK_STATIC* pStatic)
 {
  	BOOL bRet = FALSE;
	int i,j;
	DWORD dwBase,dw;

	memset(pDyna,0,sizeof(STK_DYNA));
	stream.SaveCurrentPos();
	/*cout<<"dyna stream"<<endl;*/
	try	//开始解码
	{
		DWORD dw1Head = stream.Get(m_compressType_==TICK?3:5);
		int nNumPrice = (dw1Head>>2)&0x7;
		BOOL bHasVolAmount = dw1Head&0x2;
		BOOL bHasMMP = dw1Head&1;
		BOOL bOnlyMMP = nNumPrice==7;
		nNumPrice = min(4,nNumPrice);

		//StockID
		if(m_compressType_!=DYNA)
		{
			pDyna->m_wStkID = pStatic->m_wStkID;
		}
		else
		{
			if(IsHKSec())
				stream.SETBITCODE(hkStkIDCode);
			else
				stream.SETBITCODE(stkIDCode);
			dwBase = wStockID;
			pDyna->m_wStkID = (WORD)stream.DecodeData(&dwBase);
			wStockID = pDyna->m_wStkID;

			pOldDyna = NULL;
			if(m_bSeqCompress_)
			{
//			    cout<<"get oldnyna stkid:"<<pDyna->m_wStkID<<endl;
				pOldDyna = GetOldDynaByID(pStkDyna, pDyna->m_wStkID);
				if(!pOldDyna)
					throw("Get Dyna Info Fail");
			}
			pStatic = GetStkByID(pStkStatic,pDyna->m_wStkID);
			if(!pStatic)
				throw("Get Stack Info fail");
		}


		if(m_bSeqCompress_)
		{
			if(pOldDyna)
				memcpy(pDyna,pOldDyna,sizeof(STK_DYNA));
			else
				throw("Decode Cannot Find old Dyna Info from ID");
		}
		if(pStatic)
		{
			//时间
			if(m_compressType_==TICK || (m_cHeadAttr_&CPS_DYNA_HEAD::HAS_TIME_BIAS))
			{
				stream.SETBITCODE(timeCode);
				dwBase = m_compressType_==TICK ? (pOldDyna?pOldDyna->m_time:0) : m_baseTime_;
				pDyna->m_time = stream.DecodeData(&dwBase,m_compressType_==TICK?FALSE:TRUE);
			}
			else
				pDyna->m_time = m_baseTime_;

			if(!bOnlyMMP)
			{
				//持仓量,成交笔数
				if(IsFuture() || IsFutureIndex())
				{
					stream.SETBITCODE(oiCode);
					pDyna->m_dwOpenInterest = stream.DecodeData(pOldDyna ? &pOldDyna->m_dwOpenInterest : NULL);

					if(cVersion>DYNACPS_VER_0)
					{
						if(pOldDyna)
							stream.SETBITCODE(dynaPriceDiffCode);
						else
							stream.SETBITCODE(dynaPriceCode);
						pDyna->m_dwSettlePrice = stream.DecodeData(pOldDyna ? &pOldDyna->m_dwSettlePrice : &pStatic->m_dwLastClose);

					}
				}
				else if(HasTickCount())
				{
					stream.SETBITCODE(tickcountCode);
					pDyna->m_dwTickCount = stream.DecodeData(pOldDyna ? &pOldDyna->m_dwTickCount : NULL);

				}

				//价格(最新/最低/最高/开盘)
				if(nNumPrice)
				{
					if(pOldDyna)
						stream.SETBITCODE(dynaPriceDiffCode);
					else
						stream.SETBITCODE(dynaPriceCode);
					for(i=0;i<nNumPrice;i++)
					{
						DWORD* pPrice = ((DWORD*)&pDyna->m_dwOpen)+3-i;
						if(pOldDyna)
							dwBase = ((DWORD*)&pOldDyna->m_dwOpen)[3-i];
						else
							dwBase = pStatic->m_dwLastClose;
						*pPrice = stream.DecodeData(&dwBase);
					}
				}

				//成交量
				if(bHasVolAmount)
				{
					if(IsHKSec())
					{
						stream.SETBITCODE(hkVolZeroCode);
						dw = stream.DecodeData();

						if(pOldDyna)
							stream.SETBITCODE(hkDiffVolCode);
						else
							stream.SETBITCODE(hkVolCode);
						INT64 n = stream.DecodeMWordData();
						while(dw--)
							n *= 10;
						if(pOldDyna)
							n += pOldDyna->m_mVolume;
						pDyna->m_mVolume = n;
					}
					else
					{
						if(pOldDyna)
							stream.SETBITCODE(volDiffCode);
						else
							stream.SETBITCODE(volCode);
						pDyna->m_mVolume = stream.DecodeMWordData(pOldDyna ? &pOldDyna->m_mVolume : NULL);
					}
					if(pOldDyna)
					{
						pDyna->m_mInnerVol = pOldDyna->m_mInnerVol.GetABSValue();
						if(stream.Get(1))
							pDyna->m_mInnerVol = -(pDyna->m_mInnerVol+pDyna->m_mVolume - pOldDyna->m_mVolume);
					}
					else
					{
						stream.SETBITCODE(innerVolCode);
						pDyna->m_mInnerVol = stream.DecodeMWordData();
					}

					//成交额
					if(HasAmount())
					{
						if(pOldDyna)
							stream.SETBITCODE(amountDiffCode);
						else
							stream.SETBITCODE(amountCode);
						MWORD mVol = pDyna->m_mVolume;
						if(pOldDyna)
							mVol -= pOldDyna->m_mVolume;

						int nPriceDiv = 1;
						for(i=0;i<pStatic->m_nPriceDigit;i++)
							nPriceDiv *= 10;
						MWORD mAmountBase(0,0);
						if(pStatic->m_cType!=STK_STATIC::INDEX)
							mAmountBase = mVol*(int)(pStatic->m_nVolUnit*pDyna->m_dwNew)/nPriceDiv;
						pDyna->m_mAmount = stream.DecodeMWordData(&mAmountBase);
						if(pOldDyna)
							pDyna->m_mAmount += pOldDyna->m_mAmount;

					}
				}
			}

			//买卖盘
			if((IsFuture()||IsFExg()) && bHasMMP)
			{
//			    cout<<"future:MMP:"<<endl;
				if(pOldDyna)
				{
					stream.SETBITCODE(dynaPriceDiffCode);
					pDyna->m_dwBuyPrice[0] = stream.DecodeData(&pOldDyna->m_dwBuyPrice[0]);
					pDyna->m_dwSellPrice[0] = stream.DecodeData(&pOldDyna->m_dwSellPrice[0]);

					if(IsFuture())
					{
						stream.SETBITCODE(mmpDiffVolCode);
						pDyna->m_dwBuyVol[0] = stream.DecodeData(&pOldDyna->m_dwBuyVol[0]);
						pDyna->m_dwSellVol[0] = stream.DecodeData(&pOldDyna->m_dwSellVol[0]);
					}
				}
				else
				{
					dwBase = pStatic->m_dwLastClose;
					stream.SETBITCODE(dynaPriceCode);
					pDyna->m_dwBuyPrice[0] = stream.DecodeData(&dwBase);
					pDyna->m_dwSellPrice[0] = stream.DecodeData(&dwBase);

					if(IsFuture())
					{
						stream.SETBITCODE(mmpVolCode);
						pDyna->m_dwBuyVol[0] = stream.DecodeData();
						pDyna->m_dwSellVol[0] = stream.DecodeData();
					}
				}

			}
			else if(bHasMMP)
			{
//			    cout<<"MMP"<<endl;
				DWORD dwMMP[10],dwMMPVol[10];
				DWORD dwLastMMP[10],dwLastMMPVol[10];
				if(pOldDyna)
				{
					for(i=0;i<5;i++)
					{
						dwLastMMP[i] = pOldDyna->m_dwBuyPrice[4-i];
						dwLastMMPVol[i] = pOldDyna->m_dwBuyVol[4-i];
					}
					memcpy(dwLastMMP+5,pOldDyna->m_dwSellPrice,5*sizeof(DWORD));
					memcpy(dwLastMMPVol+5,pOldDyna->m_dwSellVol,5*sizeof(DWORD));
				}
				memset(dwMMP,0,sizeof(dwMMP));
				memset(dwMMPVol,0,sizeof(dwMMPVol));

				DWORD dwMMPError = 0;
				BOOL bHasMMPPrice = TRUE;
				DWORD dwVolPos = 0;
				if(pOldDyna)
				{
					//相关压缩，买卖盘价格
					if(IsHKSec())
					{
						if(!stream.Get(1))	//买卖盘价格无变化;
						{
							memcpy(dwMMP,dwLastMMP,sizeof(dwMMP));
							bHasMMPPrice = FALSE;
						}
					}
					else
					{

						stream.SETBITCODE(mmpPriceDiffCode);
						dw = stream.DecodeData();
						if(dw==(DWORD)-3)
							dwMMPError |= 2;
						else if(dw==(DWORD)-4)
						{
							dwMMPError |= 2;
							bHasMMPPrice = FALSE;
							memcpy(dwMMP,dwLastMMP,sizeof(dwMMP));
						}
						else if(dw!=(DWORD)-2)		//解出价格
						{
							bHasMMPPrice = FALSE;
							memcpy(dwMMP,dwLastMMP,sizeof(dwMMP));
							if(dw!=(DWORD)-1)		//价格有变动
							{
								stream.SETBITCODE(mmpInsCode);

								int nIns = (dw>>4)&0xf;
								int nDel = dw&0xf;
								if(nIns<nDel)
									memmove(dwMMP+nIns+1,dwMMP+nIns,(nDel-nIns)*sizeof(DWORD));
								else if(nIns>nDel)
									memmove(dwMMP+nDel,dwMMP+nDel+1,(nIns-nDel)*sizeof(DWORD));
								dwBase = nIns ? dwMMP[nIns-1] : dwMMP[nIns+1]-2;
								stream.SETBITCODE(mmpInsCode);
								dwMMP[nIns] = stream.DecodeData(&dwBase);
							}
						}
					}

					//买卖盘量数量及分布
					stream.SETBITCODE(mmpVolPosDiffCode);
					dwVolPos = stream.DecodeData();
				}
				if(bHasMMPPrice)	//买卖盘价格完整编码
				{
					//买卖盘数量
					stream.SETBITCODE(mmpPriceNumCode);
					dw = stream.DecodeData();
					if(dw&0x40)
					{
						dwMMPError |= 1;
						dw &= 0x3F;
					}
					int nNumBuy = (dw>>3)&0x7;
					int nNumSell = dw&7;
					if(!pOldDyna)
					{
						if(dwMMPError&1)
						{
							stream.SETBITCODE(mmpErrorVolPosCode);
							dwVolPos = stream.DecodeData();
							if(dwVolPos==0x400)
							{
								dwVolPos = 0x3FF>>(5-nNumSell);
								dwVolPos &= 0x3FF<<(5-nNumBuy);
							}
						}
						else
						{
							dwVolPos = 0x3FF>>(5-nNumSell);
							dwVolPos &= 0x3FF<<(5-nNumBuy);
						}
					}

					if(IsHKSec())
					{
						//价差编码
						dwBase = GetDefaultHKSpread(pDyna->m_dwNew);
						DWORD dwBuySpread=dwBase,dwSellSpread=dwBase;
						dw = stream.Get(1);
						if(dw)
						{
							stream.SETBITCODE(hkSpreadCode);
							if(nNumBuy)
								dwBuySpread = stream.DecodeData();
							if(nNumSell)
								dwSellSpread = stream.DecodeData();
						}
						stream.SETBITCODE(hkMmpPosCode);
						if(nNumBuy)
						{
							dw = stream.DecodeData();
							if(dw<1000)
								dwMMP[4] = pDyna->m_dwNew - dw*dwBuySpread;
							else
								dwMMP[4] = pDyna->m_dwNew - (dw-999999);
							for(i=3;i>=5-nNumBuy;i--)
								dwMMP[i] =  dwMMP[i+1]>dwBuySpread ? dwMMP[i+1] - dwBuySpread : 0;
						}
						if(nNumSell)
						{
							dw = stream.DecodeData();
							if(dw<1000)
								dwMMP[5] = pDyna->m_dwNew + dw*dwSellSpread;
							else
								dwMMP[5] = pDyna->m_dwNew + (dw-999999);
							for(i=6;i<5+nNumSell;i++)
								dwMMP[i] = dwMMP[i-1] + dwSellSpread;
						}
					}
					else
					{
						//Gap位置，数量
						if(pOldDyna)
							stream.SETBITCODE(mmpDiffGapPosCode);
						else
							stream.SETBITCODE(mmpGapPosCode);
						DWORD dwGap = stream.DecodeData();

						//买卖盘位置(用于定位买1或者卖1(无买盘的情况)大小)
						int nBase = 4;
						stream.SETBITCODE(mmpPosCode);
						DWORD dwBasePrice = stream.DecodeData(&pDyna->m_dwNew);
						if(dwBasePrice==pDyna->m_dwNew+999999)
						{
							nBase = 5;
							dwBasePrice = pDyna->m_dwNew;
						}
						else if(nNumBuy==0)
							nBase = 5;
						dwMMP[nBase] = dwBasePrice;

						//买卖盘Gap
						stream.SETBITCODE(mmpGapCode);
						for(i=nBase;i<9;i++)
						{
							if((dwGap>>i)&1)
								dwMMP[i+1] = stream.DecodeData(&dwMMP[i]);
							else if(i<nNumSell+4)
								dwMMP[i+1] = dwMMP[i]+1;
						}
						for(i=nBase-1;i>=0;i--)
						{
							if((dwGap>>i)&1)
								dwMMP[i] = stream.DecodeData(&dwMMP[i+1],TRUE);
							else if(i>=5-nNumBuy)
								dwMMP[i] = dwMMP[i+1]-1;
						}
					}
				}
				//买卖盘量
				if(dwVolPos)
				{
					if(IsHKSec())
					{
						for(i=0;i<10;i++)
						{
							if((dwVolPos>>i)&1)
							{
								stream.SETBITCODE(hkVolZeroCode);
								int nNumZero = stream.DecodeData();

								if(pOldDyna)
									stream.SETBITCODE(hkMmpDiffVolCode);
								else
									stream.SETBITCODE(hkMmpVolCode);
								dw = stream.DecodeData();
								while(nNumZero--)
									dw *= 10;
								dwMMPVol[i] = dw;
							}
						}
					}
					else
					{
						if(pOldDyna)
							stream.SETBITCODE(mmpDiffVolCode);
						else
							stream.SETBITCODE(mmpVolCode);
						for(i=0;i<10;i++)
						{
							if((dwVolPos>>i)&1)
								dwMMPVol[i] = stream.DecodeData();
						}
					}
				}
				if(pOldDyna)
				{
					if(dwMMPError&2)	//直接按照位置解码
					{
						for(i=0;i<10;i++)
							dwMMPVol[i] += dwLastMMPVol[i];
					}
					else
					{
						j = 0;
						for(i=0;i<5;i++)
						{
							if(dwMMP[i])
							{
								while(j<5 && dwMMP[i]>dwLastMMP[j])
									j++;
								if(j<5 && dwMMP[i]==dwLastMMP[j])
								{
									dwMMPVol[i] += dwMMP[j]?dwLastMMPVol[j]:0;
									j++;
								}
							}
						}
						j = 5;
						for(i=5;i<10;i++)
						{
							if(dwMMP[i])
							{
								while(j<10 && dwMMP[i]>dwLastMMP[j])
									j++;
								if(j<10 && dwMMP[i]==dwLastMMP[j])
								{
									dwMMPVol[i] += dwMMP[j]?dwLastMMPVol[j]:0;
									j++;
								}
							}
						}
					}
					if(dwMMP[4]==pDyna->m_dwNew)
						dwMMPVol[4] -= (DWORD)(pDyna->m_mVolume - pOldDyna->m_mVolume);
					if(dwMMP[5]==pDyna->m_dwNew)
						dwMMPVol[5] -= (DWORD)(pDyna->m_mVolume - pOldDyna->m_mVolume);

				}
				for(i=0;i<5;i++)
				{
					pDyna->m_dwBuyPrice[i] = dwMMP[4-i];
					pDyna->m_dwBuyVol[i] = dwMMPVol[4-i];
					pDyna->m_dwSellPrice[i] = dwMMP[5+i];
					pDyna->m_dwSellVol[i] = dwMMPVol[5+i];
				}
			}
			bRet = TRUE;
		}
		else
			throw("Cannot Get Static Info");

			//cout<<"pDyna->m_wStkID:"<<pDyna->m_wStkID<<endl;
			//cout<<"pDyna->m_timed:"<<pDyna->m_time<<endl;
			//cout<<"open:"<<pDyna->m_dwOpen<<endl;
			//cout<<"high:"<<pDyna->m_dwHigh<<endl;
			//cout<<"low:"<<pDyna->m_dwLow<<endl;
			//cout<<"new:"<<pDyna->m_dwNew<<endl;
			//cout<<"volume:"<<pDyna->m_mVolume<<endl;
			//cout<<"pDyna->m_mAmount:"<<pDyna->m_mAmount<<endl;
			//cout<<"innervol:"<<pDyna->m_mInnerVol<<endl;
			//cout<<"pDyna->m_dwTickCount"<<pDyna->m_dwTickCount<<endl;
			//cout<<"buyprice0:"<<pDyna->m_dwBuyPrice[0]<<endl;
			//cout<<"pDyna->m_dwBuyVol[0]"<<pDyna->m_dwBuyVol[0]<<endl;
			//cout<<"sellprice0:"<<pDyna->m_dwSellPrice[0]<<endl;
			//cout<<"pDyna->m_dwSellVol[0]"<<pDyna->m_dwSellVol[0]<<endl;
			//cout<<"pDyna->m_dwOpenInterest:"<<pDyna->m_dwOpenInterest<<endl;
			//cout<<"pDyna->m_dwSettlePrice:"<<pDyna->m_dwSettlePrice<<endl;

	}
	catch(...)
	{
		stream.RestoreToSavedPos();
	}
	return bRet;
 }


DWORD ExtractDC::GetDefaultHKSpread(DWORD dwPrice)
{
	struct HKSPREAD
	{
		WORD	m_wDiff;
		DWORD	m_dwPrice;
	};

	static HKSPREAD spread[] =
	{
		{1,		250},
		{5,		500},
		{10,	2000},
		{25,	5000},
		{50,	30000},
		{100,	50000},
		{250,	100000},
		{500,	200000},
		{1000,	1000000},
		{2500,	9995000},
	};
	DWORD dwRet = 0;
	for(int i=0;i<sizeof(spread)/sizeof(spread[0]);i++)
	{
		if(dwPrice<spread[i].m_dwPrice)
		{
			dwRet = spread[i].m_wDiff;
			break;
		}
	}
	return dwRet;
}

const struct STK_DYNA* ExtractDC::GetOldDynaByID(STK_DYNA * pStkDyna,WORD index)
{
	if(static_tag == true)
	{
		return pStkDyna + index;
	}
	else
		return NULL;
}

const struct STK_STATIC* ExtractDC::GetStkByID(STK_STATIC * pStkStatic, WORD index)
{
	if(index <= stk_total_)
		return pStkStatic + index;
	else
		return NULL;
}

int ExtractDC::ExpandL2MMPEx(const BYTE* pData,int nDataLen,SH_L2_MMPEX* pMMPExBuf,int& nBufSize)
{
	int nRet = 0;
	int i,j;
	DWORD dw,dwBase;

	CBitStream stream;
	stream.SetBuffer((BYTE*)pData,nDataLen);

	try
	{
		int nNum = stream.Get(16);
		if(nNum<=nBufSize)
		{
			unsigned short wMarket = (WORD)stream.Get(16);

			memset(pMMPExBuf,0,sizeof(SH_L2_MMPEX)*nNum);

			for(int n=0;n<nNum;n++)
			{
				SH_L2_MMPEX* pCur = pMMPExBuf+n;

				//ID
				pCur->m_wStkID = (WORD)stream.Get(16);

				//基价
				stream.SETBITCODE(lastCloseCode);
				DWORD dwLastClose = stream.DecodeData();

				//平均买卖价
				stream.SETBITCODE(dynaPriceCode);
				pCur->m_dwAvgBuyPrice = stream.DecodeData(&dwLastClose);
				pCur->m_dwAvgSellPrice = stream.DecodeData(&dwLastClose);

				//平均买卖量
				stream.SETBITCODE(volCode);
				pCur->m_mAllBuyVol = stream.DecodeMWordData();
				pCur->m_mAllSellVol = stream.DecodeMWordData();

				DWORD dwMMP[10],dwMMPVol[10];
				memset(dwMMP,0,sizeof(dwMMP));
				memset(dwMMPVol,0,sizeof(dwMMPVol));

				//买卖盘数量
				stream.SETBITCODE(mmpPriceNumCode);
				dw = stream.DecodeData();

				int nNumBuy = (dw>>3)&0x7;
				int nNumSell = dw&0x7;

				if(nNumBuy || nNumSell)
				{
					//买卖盘Gap位置
					stream.SETBITCODE(mmpGapPosCode);
					DWORD dwGap = stream.DecodeData();

					//买卖盘位置(用于定位买1或者卖1(无买盘的情况)大小)
					if(nNumBuy)
						j = 4;
					else
						j = 5;
					stream.SETBITCODE(dynaPriceCode);
					dwMMP[j] = stream.DecodeData(&dwLastClose);

					//买卖盘Gap
					stream.SETBITCODE(mmpGapCode);
					for(i=j;i<9 && i<4+nNumSell;i++)
					{
						dwBase = dwMMP[i];
						if(i==4)
							dwBase += 10;
						if((dwGap>>i)&1)
							dwMMP[i+1] = stream.DecodeData(&dwBase);
						else
							dwMMP[i+1] = dwBase+1;
					}
					for(i=j-1;i>=0 && i>=5-nNumBuy;i--)
					{
						dwBase = dwMMP[i+1];
						if(i==4)
							dwBase -= 10;
						if((dwGap>>i)&1)
							dwMMP[i] = stream.DecodeData(&dwBase,TRUE);
						else
							dwMMP[i] = dwBase-1;
					}

					stream.SETBITCODE(mmpVolCode);
					for(i=5-nNumBuy;i<5+nNumSell;i++)
					{
						dwMMPVol[i] = stream.DecodeData();
					}
					for(i=0;i<5;i++)
					{
						pCur->m_dwBuyPrice[i] = dwMMP[4-i];
						pCur->m_dwSellPrice[i] = dwMMP[5+i];
						pCur->m_dwBuyVol[i] = dwMMPVol[4-i];
						pCur->m_dwSellVol[i] = dwMMPVol[5+i];
					}
				}

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
