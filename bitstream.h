#ifndef _BITSTREAM_H_INCLUDE
#define _BITSTREAM_H_INCLUDE

#include "mword.h"
#include "flags.h"

using namespace std;

#pragma pack(push,1)

//BITCODE::m_cCode

//'B':数据不超过m_dwCodeData位,
//'b':数据不超过m_dwCodeData位, 带符号数
//'m':数据不超过m_dwCodeData位, 负数
//'S':数据右移HIWORD(m_dwCodeData)后不超过LOWORD(m_dwCodeData)位
//'E':数据等于CodeData
//'Z':数据为x*10^N, x<CodeData,N==1-4
//'P':在m_dwCodeData位数据中，存在一个1，这个1的位置
//'D':原始整形数据，原始数据放入Bit流,总是放在最后面，不进行匹配
//'M':原始MWORD数据，原始数据放入Bit流,总是放在最后面，不进行匹配

struct BITCODE
{
	WORD	m_wCodeBits;	//编码
	char	m_nCodeLen;		//编码长度
	char	m_nDataLen;		//编码后跟随的数据长度,单位Bit，最大32Bit
	char	m_cCode;		//编码方法	  'B':数据不超过m_dwCodeData位,
										//'X':数据为x*10^N, x为CodeData位,N==1-4
										//'E':数据等于CodeData
										//'D':原始整形数据，原始数据放入Bit流,总是放在最后面，不进行匹配
										//'F':原始浮点数据，原始数据放入Bit流,总是放在最后面，不进行匹配
	DWORD	m_dwCodeData;	//编码数据
	DWORD	m_dwDataBias;	//编码数据偏移量
#ifdef _DEBUG
	DWORD	m_dwCodeCount;	//频度
#endif
	BOOL IsOriginalData() const {return m_cCode=='D' || m_cCode=='M';}
	BOOL IsBitPos() const {return m_cCode=='P';}
};

class CBitStream
{
protected:
	BYTE* m_pData;				//缓冲区
	int m_nBitSize;				//缓冲区大小，Bit
	int m_nCurPos;				//当前位置

	int m_nSavedPos;			//上一次保存的位置

    BITCODE*	m_pCodes;	//当前编解码码表
	int		m_nNumCode;

public:
	CBitStream();
	CBitStream(BYTE* pData,int nSize);			//直接使用给定缓冲区,nSize是字节数

	void SetBuffer(BYTE* pData,int nSize);		//直接使用给定缓冲区,nSize是字节数
	void AttachToBuffer(BYTE* pData,int nSize,int nBitPos);	//使用给定缓冲区，nSize是字节数,nBitPos是之前编码所到的位置,

	int GetBitLength(){return m_nCurPos;}						//得到当前Bit流Bit数
	int GetByteLength(){return (m_nCurPos+7)/8;}				//得到当前Bit流Byte数
	int GetCurPos(){return m_nCurPos;}
	int GetBitSize(){return m_nBitSize;}

	virtual DWORD Get(int nBit);				//取得nBit数据，最多32Bit
	virtual int GetNoMove(int nBit,DWORD& dw);	//不移动内部指针，取得nBit数据，最多32Bit，返回实际取得的长度
	int GetString(char* pBuf,int nBufSize);

	virtual int Put(DWORD dw,int nBit);			//向当前位置添加nBit数据
	int PutString(LPCSTR str);

	int Move(int nBit){return MoveTo(m_nCurPos+nBit);}		//移动内部指针，nBit可以为负，返回新的位置
	int MoveTo(int nPos);					//移动内部指针到nPos处

	void SaveCurrentPos(){m_nSavedPos = m_nCurPos;}		//保存当前位置
	int  RestoreToSavedPos(){return MoveTo(m_nSavedPos);}		//回卷到上一次保存的位置

	void SetBitCode(BITCODE* pCodes,int nNumCode){m_pCodes = pCodes;m_nNumCode = nNumCode;}

	int EncodeData(DWORD dwData,const DWORD* pdwLastData=NULL,BOOL bReverse=FALSE);	//根据情况编码，返回编码Bit长度
	int EncodeData(MWORD mData,const MWORD* pmLastData=NULL);	//根据情况编码，返回编码Bit长度
	int EncodeFloat(float f,BOOL bCheck0);				//返回编码Bit长度,不需要编码
	int EncodeStringData(LPCSTR strData,DWORD& dwLastData);							//根据情况编码，返回编码Bit长度,更新dwLastData

	DWORD DecodeData(const DWORD* pdwLastData=NULL,BOOL bReverse=FALSE);
	MWORD DecodeMWordData(const MWORD* pmLastData=NULL,BOOL bReverse=FALSE);
	float DecodeFloat(BOOL bCheck0);
	DWORD DecodeStringData(char* pBuf,int nBufSize,DWORD& dwLastData);						//如果解出数字Label，则返回该数字，否则返回0xFFFFFFFF,更新dwLastData

	int GetNumZero(DWORD& dw);
	int GetNumZero(MWORD& m);

protected:
	const BITCODE* EncodeFindMatch(DWORD& dw);							//找到配合的编码,编码后的数据通过dw带回
	const BITCODE* DecodeFindMatch(DWORD& dw);							//找到配合的编码，并通过dw返回

	const BITCODE* FindOrgMData();
	const BITCODE* FindStringData();
};

#define SETBITCODE(x)	SetBitCode(x,sizeof(x)/sizeof(x[0]))

#ifdef _DEBUG
float GetAvgCodeLen(BITCODE* pCode,int nNum);
#define AVGCODELEN(x)	GetAvgCodeLen(x,sizeof(x)/sizeof(x[0]))
#endif

#ifdef _DEBUG
void PrintOptimizedCode(LPCSTR name,BITCODE* pCodes,int nNum);
#define PRINTOPTCODE(y,x)	PrintOptimizedCode(x,y,sizeof(y)/sizeof(y[0]))
#endif

#pragma pack(pop)

#endif //_BITSTREAM_H_INCLUDE

