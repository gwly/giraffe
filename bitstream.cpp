#include "stdafx.h"
#include "bitstream.h"

////////////////////////////////////////////////////////////////////////////////////////////
//CBitStream

CBitStream::CBitStream()
{
	m_pData = NULL;
	m_nBitSize = 0;
	m_nCurPos = 0;
	m_nSavedPos = 0;
	m_pCodes = NULL;
	m_nNumCode = 0;
}

CBitStream::CBitStream(BYTE* pData,int nSize)
{
	SetBuffer(pData,nSize);
}

void CBitStream::SetBuffer(BYTE* pData,int nSize)
{
	m_pData = pData;
	m_nBitSize = nSize*8;
	m_nCurPos = 0;
	m_pCodes = NULL;
	m_nNumCode = 0;
}

void CBitStream::AttachToBuffer(BYTE* pData,int nSize,int nBitPos)
{
	if(nBitPos>=0 && nBitPos<=nSize*8)
	{
		m_pData = pData;
		m_nBitSize = nSize*8;
		m_nCurPos = nBitPos;
		m_pCodes = NULL;
		m_nNumCode = 0;
	}
	else
	{
		m_pData = NULL;
		m_nBitSize = 0;
		m_nCurPos = 0;
		m_pCodes = NULL;
		m_nNumCode = 0;
	}
}

DWORD CBitStream::Get(int nBit)
{
	DWORD dw = 0;
	int nMove = GetNoMove(nBit,dw);
	m_nCurPos += nMove;
	return dw;
}

int CBitStream::GetString(char* pBuf,int nBufSize)
{
	DWORD dw = 0;
	int i;
	for(i=0;i<nBufSize-1;i++)
	{
		dw = Get(8);
		if(!dw)
			break;
		pBuf[i] = (char)dw;
	}
	pBuf[i] = '\0';
	return i;
}

int CBitStream::GetNoMove(int nBit,DWORD& dw)
{
	dw = 0;
	if(nBit<0 || nBit>32)
		throw("Get Bit Len Error");
	if(m_nBitSize<=m_nCurPos)
		throw("Get Pos Out of Range");
	else if(nBit>0)
	{
		if(nBit>m_nBitSize-m_nCurPos)
			nBit = m_nBitSize-m_nCurPos;
		int nPos = m_nCurPos/8;
		dw = m_pData[nPos++];
		int nGet = 8 - (m_nCurPos%8);
		if(nGet!=8)
			dw &= (0xFF>>(8-nGet));
		int nLeft = nBit-nGet;
		while(nLeft>0)
		{
			if(nLeft>=8)
			{
				dw = (dw<<8) | m_pData[nPos++];
				nLeft -= 8;
			}
			else
			{
				dw = (dw<<nLeft) | (m_pData[nPos++]>>(8-nLeft));
				nLeft = 0;
			}
		}
		if(nLeft<0)
			dw >>= -nLeft;
	}
	return nBit;
}


int CBitStream::MoveTo(int nPos)
{
	m_nCurPos = nPos;
	if(m_nCurPos<0)
		m_nCurPos = 0;
	else if(m_nCurPos>m_nBitSize)
		m_nCurPos = m_nBitSize;
	return m_nCurPos;
}

int CBitStream::PutString(LPCSTR str)
{
	int nLen = strlen(str);
	for(int i=0;i<=nLen;i++)
		Put(str[i],8);
	return (nLen+1)*8;
}

int CBitStream::Put(DWORD dw,int nBit)
{
	int nRet = 0;
	if(nBit>0)
	{
		if(m_nCurPos+nBit<=m_nBitSize)
		{
			dw <<= 32-nBit;
			BYTE* pHighByte = (BYTE*)&dw+3;

			nRet = nBit;
			int nLastBit = m_nCurPos%8;
			if(nLastBit)
			{
				int nSpace = 8-nLastBit;
				BYTE c = m_pData[m_nCurPos/8] & (0xFF<<nSpace);
				int nAdd = Min(nBit,nSpace);
				DWORD dwNew = nAdd>=nSpace ? (dw>>(nBit-nSpace)) : (dw<<(nSpace-nBit));
				c |= (dw>>(32-nSpace)) & (0xFF>>nLastBit);
				m_pData[m_nCurPos/8] = c;
				nBit -= nSpace;
				dw <<= nSpace;
				m_nCurPos += nAdd;
			}

			if(nBit)
			{
				while(nBit>0)
				{
					m_pData[m_nCurPos/8] = *pHighByte;
					m_nCurPos += Min(8,nBit);
					nBit -= 8;
					dw <<= 8;
				}
			}
		}
		else
			throw("Put Out of Range");
	}
	return nRet;
}

const BITCODE* CBitStream::FindOrgMData()
{
	const BITCODE* pRet = NULL;
	if(m_pCodes)
	{
		for(int i=0;i<m_nNumCode;i++)
		{
			if(m_pCodes[i].m_cCode=='M')
			{
				pRet = m_pCodes+i;
#ifdef _DEBUG
				((BITCODE*)pRet)->m_dwCodeCount++;
#endif
				break;
			}
		}
	}
	return pRet;
}

const BITCODE* CBitStream::FindStringData()
{
	const BITCODE* pRet = NULL;
	if(m_pCodes)
	{
		for(int i=0;i<m_nNumCode;i++)
		{
			if(m_pCodes[i].m_cCode=='s')
			{
				pRet = m_pCodes+i;
#ifdef _DEBUG
				((BITCODE*)pRet)->m_dwCodeCount++;
#endif
				break;
			}
		}
	}
	return pRet;
}

const BITCODE* CBitStream::EncodeFindMatch(DWORD& dwData)
{
	const BITCODE* pRet = NULL;
	int i,j;
	if(m_pCodes)
	{
		for(i=0;i<m_nNumCode;i++)
		{
			const BITCODE* pCur = m_pCodes+i;
			DWORD dw = dwData;
			if(pCur->m_cCode=='b')
			{
				if(dwData&0x80000000)	//负数
				{
					dw += pCur->m_dwDataBias;
					DWORD dwMask = (0xFFFFFFFF<<(pCur->m_dwCodeData-1));
					if((dw&dwMask)==dwMask)
						pRet = pCur;
				}
				else
				{
					dw -= pCur->m_dwDataBias;
					if((dw>>(pCur->m_dwCodeData-1))==0)
						pRet = pCur;
				}
			}
			else if(pCur->m_cCode=='Z')
			{
				if(dw && (dw%10==0))
				{
					for(j=0;j<4;j++)
					{
						if(dw%10)
							break;
						dw /= 10;
					}
					dw -= pCur->m_dwDataBias;
					if((dw>>pCur->m_dwCodeData)==0)
					{
						dw = (dw<<2)|(j-1);
						pRet = pCur;
					}
				}
			}
			else if(pCur->m_cCode=='m')
			{
				if(dwData&0x80000000)	//负数
				{
					dw += pCur->m_dwDataBias;
					DWORD dwMask = (0xFFFFFFFF<<pCur->m_dwCodeData);
					if((dw&dwMask)==dwMask)
						pRet = pCur;
				}
			}
			else
			{
				dw -= pCur->m_dwDataBias;
				switch(pCur->m_cCode)
				{
				case 'B':
					if((dw>>pCur->m_dwCodeData)==0)
						pRet = pCur;
					break;
				case 'S':
					if( (dw&(0xFFFFFFFF>>(32-HIWORD(pCur->m_dwCodeData))))==0 &&
						(dw>>(HIWORD(pCur->m_dwCodeData)+LOWORD(pCur->m_dwCodeData)))==0 )
						pRet = pCur;
					break;
				case 'E':
					if(dw==pCur->m_dwCodeData)
						pRet = pCur;
					break;
				case 'P':
					for(j=0;j<(int)pCur->m_dwCodeData;j++)
					{
						if((dw>>j)&1)
						{
							if((dw>>j)==1)
							{
								dw = j;		//返回位置
								pRet = pCur;
							}
							else
								break;
						}
					}
					break;
				case 'D':
				case 'M':
				case 's':
					pRet = pCur;
					break;
				default:
					break;
				}
			}
			if(pRet)
			{
#ifdef _DEBUG
				((BITCODE*)pRet)->m_dwCodeCount++;
#endif
				dwData = dw;
				break;
			}
		}
	}
	return pRet;
}

const BITCODE* CBitStream::DecodeFindMatch(DWORD& dw)
{
	const BITCODE* pCode = NULL;
	dw = 0;
	if(m_pCodes)
	{
		DWORD dwNextCode = 0;
		int nCodeLen = GetNoMove(16,dwNextCode);
		int i;
		for(i=0;i<m_nNumCode;i++)
		{
			const BITCODE* pCur = m_pCodes+i;
			if(pCur->m_wCodeBits == (dwNextCode>>(nCodeLen-pCur->m_nCodeLen)))	//找到
			{
				pCode = pCur;
				break;
			}
		}
		if(pCode)
		{
			Move(pCode->m_nCodeLen);
			if(pCode->m_nDataLen)
				dw = Get(pCode->m_nDataLen);
			switch(pCode->m_cCode)
			{
			case 'B':
			case 'D':
			case 'M':
			case 's':
				break;
			case 'b':
				if(dw&(1<<(pCode->m_nDataLen-1)))	//负数
					dw |= (0xFFFFFFFF<<pCode->m_nDataLen);
				break;
			case 'm':
				dw |= (0xFFFFFFFF<<pCode->m_nDataLen);
				break;
			case 'S':
				dw <<= HIWORD(pCode->m_dwCodeData);
				break;
			case 'E':
				dw = pCode->m_dwCodeData;
				break;
			case 'Z':
				{
					int nExp = dw&3;
					dw >>= 2;
					dw += pCode->m_dwDataBias;
					for(i=0;i<=nExp;i++)
						dw *= 10;
				}
				break;
			case 'P':
				dw = (1<<dw);
				break;
			default:
				break;
			}
			if((dw&0x80000000) && (pCode->m_cCode=='b'))
				dw -= pCode->m_dwDataBias;
			else if(!pCode->IsOriginalData() && pCode->m_cCode!='Z' && pCode->m_cCode!='s')
				dw += pCode->m_dwDataBias;
		}
		else
			throw("Decode Cannot Find Match");
	}
	return pCode;
}


int CBitStream::EncodeData(DWORD dwData,const DWORD* pdwLastData,BOOL bReverse)
{
	int nRet = 0;
	DWORD dw = dwData;
	if(pdwLastData)
	{
		if(bReverse)
			dw = *pdwLastData - dw;
		else
			dw -= *pdwLastData;
	}
	const BITCODE* pCode = EncodeFindMatch(dw);
	if(pCode)
	{
		nRet = Put(pCode->m_wCodeBits,pCode->m_nCodeLen);
		if(pCode->IsOriginalData())
		{
			if(pCode->m_cCode=='D')
				nRet += Put(dwData,pCode->m_nDataLen);
			else
				throw("Encode Should be DWORD");
		}
		else if(pCode->m_nDataLen)
			nRet += Put(dw,pCode->m_nDataLen);
	}
	else
		throw("Encode Cannot Find Match");
	return nRet;
}

int CBitStream::EncodeData(MWORD mData,const MWORD* pmLastData)
{
	int nRet = 0;
	DWORD dw = 0;
	BOOL bDirectOrg = FALSE;
	INT64 n = mData;
	if(pmLastData)
		n -= (INT64)(*pmLastData);
	if(n<=MWORD::MAXBASE && n>=-MWORD::MAXBASE)
		dw = (DWORD)n;
	else
		bDirectOrg = TRUE;

	const BITCODE* pCode = NULL;
	if(bDirectOrg)
		pCode = FindOrgMData();
	else
		pCode = EncodeFindMatch(dw);
	if(pCode)
	{
		nRet = Put(pCode->m_wCodeBits,pCode->m_nCodeLen);
		if(pCode->IsOriginalData())
		{
			if(pCode->m_cCode=='M')
				nRet += Put(mData.GetRawData(),pCode->m_nDataLen);
			else
				throw("Encode Should be MData");
		}
		else if(pCode->m_nDataLen)
			nRet += Put(dw,pCode->m_nDataLen);
	}
	else
		throw("Encode Cannot Find Match");
	return nRet;
}

int CBitStream::EncodeFloat(float f,BOOL bCheck0=FALSE)
{
	int nRet = 0;
	DWORD* pdw = (DWORD*)&f;

	if(bCheck0)
	{
		if(f==0.0f)
			nRet = Put(0,1);
		else
		{
			Put(1,1);
			nRet = Put(*pdw,32)+1;
		}
	}
	else
		nRet = Put(*pdw,32);

	return nRet;
}

int CBitStream::EncodeStringData(LPCSTR strData,DWORD& dwLastData)
{
	int nRet = 0;
	DWORD dw = 0,dwCode=0;
	int nLen = strlen(strData);
	int i;
	for(i=0;i<nLen;i++)
	{
		if(!isdigit(strData[i]))
			break;
	}
	const BITCODE* pCode = NULL;
	if(i==nLen)
	{
		dwCode = atoi(strData);
		dwCode |= (nLen<<24);
		dw = dwCode - dwLastData;
		pCode = EncodeFindMatch(dw);
	}
	else
		pCode = FindStringData();
	if(pCode)
	{
		nRet = Put(pCode->m_wCodeBits,pCode->m_nCodeLen);
		if(pCode->IsOriginalData())
		{
			if(pCode->m_cCode=='D')
				nRet += Put(dwCode,pCode->m_nDataLen);
			else
				throw("Encode Should be MData");
		}
		else if(pCode->m_cCode=='s')
			nRet += PutString(strData);
		else if(pCode->m_nDataLen)
			nRet += Put(dw,pCode->m_nDataLen);
	}
	else
		throw("Encode Cannot Find Match");
	dwLastData = dwCode;
	return nRet;
}

DWORD CBitStream::DecodeData(const DWORD* pdwLastData,BOOL bReverse)
{
	DWORD dw = 0;
	const BITCODE* pCode = DecodeFindMatch(dw);
	if(pCode)
	{
		if(!pCode->IsOriginalData() && pdwLastData)
		{
			if(bReverse)
				dw = *pdwLastData - dw;
			else
				dw += *pdwLastData;
		}
	}
	return dw;
}

MWORD CBitStream::DecodeMWordData(const MWORD* pmLastData,BOOL bReverse)
{
	MWORD m(0);
	DWORD dw = 0;
	const BITCODE* pCode = DecodeFindMatch(dw);
	if(pCode)
	{
		if(pCode->m_cCode=='M')
			m.SetRawData(dw);
		else
		{
			if(!pCode->IsOriginalData() && pmLastData)
			{
				INT64 dLast = (INT64)*pmLastData;
				INT64 dThis = (int)dw;
				if(bReverse)
					m = dLast - dThis;
				else
					m = dLast + dThis;
			}
			else
				m = (int)dw;
		}
	}
	return m;
}

float CBitStream::DecodeFloat(BOOL bCheck0=FALSE)
{
	float fRet = 0.0f;
	BOOL b = TRUE;
	if(bCheck0)
		b = Get(1);
	if(b)
	{
		DWORD* pdw = (DWORD*)&fRet;
		*pdw = Get(32);
	}
	return fRet;
}

DWORD CBitStream::DecodeStringData(char* pBuf,int nBufSize,DWORD& dwLastData)						//如果解出数字Label，则返回该数字，否则返回0xFFFFFFFF,更新dwLastData
{
	DWORD dw = 0;
	const BITCODE* pCode = DecodeFindMatch(dw);
	if(pCode)
	{
		if(pCode->m_cCode=='s')
		{
			dwLastData = 0;
			dw = 0xFFFFFFFF;
			GetString(pBuf,nBufSize);
		}
		else
		{
			if(!pCode->IsOriginalData())
				dw += dwLastData;
			dwLastData = dw;
			char fmt[16];
			sprintf(fmt,"%%0%dd",dw>>24);
			sprintf(pBuf,fmt,dw&0xFFFFFF);
		}
	}
	return dw;
}

int CBitStream::GetNumZero(DWORD& dw)
{
	int nRet = 0;
	if(dw)
	{
		int n = dw;
		while(n%10==0)
		{
			n /= 10;
			nRet++;
		}
		dw = n;
	}
	return nRet;
}

int CBitStream::GetNumZero(MWORD& m)
{
	INT64 n = m;
	int nRet = 0;
	if(n)
	{
		while(n%10==0)
		{
			n /= 10;
			nRet++;
		}
	}
	m = n;
	return nRet;
}

#ifdef _DEBUG
float GetAvgCodeLen(BITCODE* pCode,int nNum)
{
	float f = 0.0f;
	int n = 0;
	for(int i=0;i<nNum;i++)
	{
		f += (pCode[i].m_nCodeLen+pCode[i].m_nDataLen)*pCode[i].m_dwCodeCount;
		n += pCode[i].m_dwCodeCount;
	}
	return f/n;
}

void PrintOptimizedCode(LPCSTR strName,BITCODE* pCodes,int nNum)
{
	char s[256];
	DWORD nCount = 0;
	int i;
	double accu = 0;
	for(i=0;i<nNum;i++)
	{
		nCount += pCodes[i].m_dwCodeCount;
		accu += pCodes[i].m_dwCodeCount*(pCodes[i].m_nCodeLen+pCodes[i].m_nDataLen);
	}
	TRACE3("-------%s------count=%d,avgLen=%.2fBit\n",strName,nCount,accu/nCount);
	if(nCount==0)
		nCount = 1;
	for(i=0;i<nNum;i++)
	{
		sprintf(s,"%c:CodeLen=%d,DataLen=%d,Percent=%d%%\n",pCodes[i].m_cCode,pCodes[i].m_nCodeLen,pCodes[i].m_nDataLen,pCodes[i].m_dwCodeCount*1000/nCount);
		TRACE(s);
	}
}

#endif

