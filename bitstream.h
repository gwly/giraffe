#ifndef _BITSTREAM_H_INCLUDE
#define _BITSTREAM_H_INCLUDE

#include "mword.h"
#include "flags.h"

using namespace std;

#pragma pack(push,1)

//BITCODE::m_cCode

//'B':���ݲ�����m_dwCodeDataλ,
//'b':���ݲ�����m_dwCodeDataλ, ��������
//'m':���ݲ�����m_dwCodeDataλ, ����
//'S':��������HIWORD(m_dwCodeData)�󲻳���LOWORD(m_dwCodeData)λ
//'E':���ݵ���CodeData
//'Z':����Ϊx*10^N, x<CodeData,N==1-4
//'P':��m_dwCodeDataλ�����У�����һ��1�����1��λ��
//'D':ԭʼ�������ݣ�ԭʼ���ݷ���Bit��,���Ƿ�������棬������ƥ��
//'M':ԭʼMWORD���ݣ�ԭʼ���ݷ���Bit��,���Ƿ�������棬������ƥ��

struct BITCODE
{
	WORD	m_wCodeBits;	//����
	char	m_nCodeLen;		//���볤��
	char	m_nDataLen;		//������������ݳ���,��λBit�����32Bit
	char	m_cCode;		//���뷽��	  'B':���ݲ�����m_dwCodeDataλ,
										//'X':����Ϊx*10^N, xΪCodeDataλ,N==1-4
										//'E':���ݵ���CodeData
										//'D':ԭʼ�������ݣ�ԭʼ���ݷ���Bit��,���Ƿ�������棬������ƥ��
										//'F':ԭʼ�������ݣ�ԭʼ���ݷ���Bit��,���Ƿ�������棬������ƥ��
	DWORD	m_dwCodeData;	//��������
	DWORD	m_dwDataBias;	//��������ƫ����
#ifdef _DEBUG
	DWORD	m_dwCodeCount;	//Ƶ��
#endif
	BOOL IsOriginalData() const {return m_cCode=='D' || m_cCode=='M';}
	BOOL IsBitPos() const {return m_cCode=='P';}
};

class CBitStream
{
protected:
	BYTE* m_pData;				//������
	int m_nBitSize;				//��������С��Bit
	int m_nCurPos;				//��ǰλ��

	int m_nSavedPos;			//��һ�α����λ��

    BITCODE*	m_pCodes;	//��ǰ��������
	int		m_nNumCode;

public:
	CBitStream();
	CBitStream(BYTE* pData,int nSize);			//ֱ��ʹ�ø���������,nSize���ֽ���

	void SetBuffer(BYTE* pData,int nSize);		//ֱ��ʹ�ø���������,nSize���ֽ���
	void AttachToBuffer(BYTE* pData,int nSize,int nBitPos);	//ʹ�ø�����������nSize���ֽ���,nBitPos��֮ǰ����������λ��,

	int GetBitLength(){return m_nCurPos;}						//�õ���ǰBit��Bit��
	int GetByteLength(){return (m_nCurPos+7)/8;}				//�õ���ǰBit��Byte��
	int GetCurPos(){return m_nCurPos;}
	int GetBitSize(){return m_nBitSize;}

	virtual DWORD Get(int nBit);				//ȡ��nBit���ݣ����32Bit
	virtual int GetNoMove(int nBit,DWORD& dw);	//���ƶ��ڲ�ָ�룬ȡ��nBit���ݣ����32Bit������ʵ��ȡ�õĳ���
	int GetString(char* pBuf,int nBufSize);

	virtual int Put(DWORD dw,int nBit);			//��ǰλ�����nBit����
	int PutString(LPCSTR str);

	int Move(int nBit){return MoveTo(m_nCurPos+nBit);}		//�ƶ��ڲ�ָ�룬nBit����Ϊ���������µ�λ��
	int MoveTo(int nPos);					//�ƶ��ڲ�ָ�뵽nPos��

	void SaveCurrentPos(){m_nSavedPos = m_nCurPos;}		//���浱ǰλ��
	int  RestoreToSavedPos(){return MoveTo(m_nSavedPos);}		//�ؾ���һ�α����λ��

	void SetBitCode(BITCODE* pCodes,int nNumCode){m_pCodes = pCodes;m_nNumCode = nNumCode;}

	int EncodeData(DWORD dwData,const DWORD* pdwLastData=NULL,BOOL bReverse=FALSE);	//����������룬���ر���Bit����
	int EncodeData(MWORD mData,const MWORD* pmLastData=NULL);	//����������룬���ر���Bit����
	int EncodeFloat(float f,BOOL bCheck0);				//���ر���Bit����,����Ҫ����
	int EncodeStringData(LPCSTR strData,DWORD& dwLastData);							//����������룬���ر���Bit����,����dwLastData

	DWORD DecodeData(const DWORD* pdwLastData=NULL,BOOL bReverse=FALSE);
	MWORD DecodeMWordData(const MWORD* pmLastData=NULL,BOOL bReverse=FALSE);
	float DecodeFloat(BOOL bCheck0);
	DWORD DecodeStringData(char* pBuf,int nBufSize,DWORD& dwLastData);						//����������Label���򷵻ظ����֣����򷵻�0xFFFFFFFF,����dwLastData

	int GetNumZero(DWORD& dw);
	int GetNumZero(MWORD& m);

protected:
	const BITCODE* EncodeFindMatch(DWORD& dw);							//�ҵ���ϵı���,����������ͨ��dw����
	const BITCODE* DecodeFindMatch(DWORD& dw);							//�ҵ���ϵı��룬��ͨ��dw����

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

