#ifndef MONITOR_EXTRACT_DC_H_
#define MONITOR_EXTRACT_DC_H_

#include <iostream>
#include "crc16.h"
#include "flags.h"
#include "structs.h"
#include "data.h"
#include "datacollect.h"
#include "bitstream.h"
#include "data.h"

#define DYNACPS_VER_0		0
#define CUR_DYNACPS_VER   1

#pragma pack(push,1)

static BITCODE stkIDCode[] =	//StockID
{
	{0		,	1,	0,'E',		1,	0},			// 0				˳��
	{0x2	,	2,	4,'B',		4,	2},			// 10	+4Bit		˳��,2+4Bit
	{0x3	,	2, 16,'D',		0,	0},			// 11	+16Bit		ֱ�Ӵ��wStockID
};

static BITCODE labelCode[] =		//����
{
	{0		,	1,	0,'E',		1,	0},			//0					˳��
	{0x4	,	3,	4,'B',		4,	2},			//100  +4Bit		˳��,+2+4Bit
	{0x5	,	3,	8,'B',		8, 18},			//101  +8Bit		˳��+16+2+8Bit
	{0x6	,	3, 32,'D',		0,	0},			//110 +32Bit		ֱ�ӵ�������Label
	{0x7	,	3,	0,'s',		0,	0},			//111 +�ַ���		������Label
};

static BITCODE priceCode[] =	//����
{
	{0x2	,	2,  8,'B',		8,  0},			// 10	+8Bit		= 8Bit
	{0x0	,	1, 16,'B',	   16,256},			// 0	+16Bit		= 16Bit+256
	{0x6	,	3, 24,'B',	   24,65792},		// 110	+24Bit		= 24Bit+65536+256
	{0x7	,	3, 32,'D',	    0,	0},			// 111	+32Bit		= 32Bit Org
};

static BITCODE updnPriceCode[] = //	��ͣ/��ͣ(������*1.1,*0.9Ϊ��׼)
{
	{0x4	,	3,	0,'E',9999999,	0},			//100			= 0
	{0x5	,	3,	0,'E',9999998,	0},			//101			= 9999900
	{0xC	,	4,	0,'E',9999997,	0},			//1100			= 1
	{0		,	2,	0,'E',		0,	0},			//00				= ��׼
	{0x1	,	2,	4,'b',		4,	0},			//01   +4Bit	= ��׼+4Bit��������
	{0xD	,	4,	8,'b',		8,	8},			//1101 +8Bit	= ��׼+8+8Bit��������
	{0xE	,	4, 16,'b',	   16,136},			//1110 +16Bit	= ��׼+128+8+16Bit��������
	{0xF	,	4, 32,'D',		0,	0},			//1111 +32Bit	= DWORD
};


static BITCODE priceDigitCode[]	=	//			�۸���С�ֱ���
{
	{0		,	1,	0,'E',		2,	0},			//0				= 2
	{2		,	2,	0,'E',		3,	0},			//10			= 3
	{0x6	,	3,	0,'E',		4,	0},			//110			= 4
	{0x7	,	3,	8,'D',		0,	0},			//111+8Bit		= 8Bit
};

static BITCODE volUnitCode[] =		//			�ɽ�����λ
{
	{0		,	1,	0,'E',	 100,	0},			//0				= 100
	{2		,	2,	0,'E',	  10,	0},			//10			= 10
	{6		,	3,	0,'E',	   1,	0},			//110			= 1
	{7		,	3, 16,'D',	   0,	0},			//111+16Bit		= 16Bit
};


static BITCODE 	issudeCode[] = //		��ͨ�ɱ�/�ܹɱ�
{
	{0		,	1,	0,'E',	  0,	0},			//0				= 0
	{1		,	1, 32,'M',	  0,	0},			//1+32Bit			= 32Bit
};

static BITCODE hkStkIDCode[] =	//StockID
{
	{0x0	,	1, 12,'B',	   12,  0},			// 0	+12Bit		˳��,12Bit
	{0x1	,	1, 16,'D',		0,	0},			// 11	+16Bit		ֱ�Ӵ��wStockID
};

static BITCODE timeCode[] =		//�ɽ�ʱ��
{
	{0		,	1,	0,'E',		0,	0},			// 0				= m_baseTime
	{0x2	,	2,	4,'B',		4,	1},			// 10	+4Bit		= m_baseTime + 4Bit + 1
	{0x6	,	3,  8,'B',		8,	17},		// 110	+8Bit		= m_baseTime + 8Bit + 17
	{0x7	,	3, 32,'D',		0,	0},			// 111	+32Bit		= 32Bit Org
};

static BITCODE oiCode[] =	//�ֲ���
{
	{0		,	2,  4,'b',	    4,	0},			// 00	+4Bit		= ��׼+4Bit
	{1		,	2,  8,'b',	    8,	8},			// 01	+8Bit		= ��׼+8Bit+8
	{2		,	2, 16,'b',	   16,136},			// 10	+16Bit		= ��׼+16Bit+128+8
	{3		,	2, 32,'D',	    0,	0},			// 11	+32Bit		= 32Bit Org
};

static BITCODE tickcountCode[] = //�ɽ�����
{
	{0x4	,	3,	0,'E',		0,	0},			// 100				= 0
	{0x5	,	3,	0,'E',		1,	0},			// 101				= 1
	{0x6	,	3,	0,'E',		2,	0},			// 110				= 2
	{0x0	,	1,	4,'B',		4,	3},			// 0	+4Bit		= 4Bit+3
	{0xE	,	4, 16,'B',	   16, 19},			// 1110	+16Bit		= 16Bit+17
	{0xF	,	4, 24,'D',		0,	0},			// 1111	+24Bit		= 24Bit Org
};

static BITCODE dynaPriceDiffCode[] = //���ѹ���۸�
{
	{0		,	1,	0,'E',		0,	0},			// 0				= ��׼
	{0x2	,	2,	4,'b',		4,	0},			// 10	+4Bit		= ��׼+4Bit
	{0x6	,	3,  6,'b',	    6,  8},			// 110	+6Bit		= ��׼+6Bit+8
	{0xE	,	4,  8,'b',		8, 40},			// 1110	+8Bit		= ��׼+8Bit+32+8
	{0x1E	,	5, 12,'b',	   12,168},			// 11110+12Bit		= ��׼+12Bit+128+32+8
	{0x1F	,	5, 32,'D',	    0,	0},			// 11111+32Bit		= 32Bit Org
};

static BITCODE dynaPriceCode[] =	//�����ѹ���۸�
{
	{0xE	,	4,	0,'E',		0,	0},			// 1110				= ��׼
	{0x2	,	2,	4,'b',		4,	0},			// 10	+4Bit		= ��׼+4Bit
	{0x0	,	1,  6,'b',	    6,  8},			// 0	+6Bit		= ��׼+6Bit+8
	{0x6	,	3,  8,'b',		8, 40},			// 110	+8Bit		= ��׼+8Bit+32+8
	{0x1E	,	5, 12,'b',	   12,168},			// 11110+12Bit		= ��׼+12Bit+128+32+8
	{0x1F	,	5, 32,'D',	    0,	0},			// 11111+32Bit		= 32Bit Org
};

static BITCODE volDiffCode[] = //���ѹ���ɽ���
{
	{0x6	,	3,  4,'Z',	    2,	1},			// 110	+2Bit+2Bit	= ��׼+2Bit*10^N
	{0xE	,	4,  6,'Z',	    4,  5},			// 1110	+4Bit+2Bit	= ��׼+(4Bit+4)*10^N
	{0x0	,	1,  6,'B',	    6,	0},			// 0	+6Bit		= ��׼+6Bit
	{0x2	,	2, 12,'B',	   12, 64},			// 10	+12Bit		= ��׼+12Bit+64
	{0x1E	,	5, 16,'B',	   16,4160},		// 11110+16Bit		= ��׼+16Bit+4096+64
	{0x1F	,	5, 32,'M',	    0,	0},			// 11111+32Bit		= 32Bit Org
};

static BITCODE volCode[] =	//�����ѹ���ɽ���
{
	{0x6	,	3, 12,'B',	   12,	0},			// 110	+12Bit		= ��׼+12Bit
	{0x0	,	1, 16,'B',	   16,4096},		// 0	+16Bit		= ��׼+16Bit+4096
	{0x2	,	2, 24,'B',	   24,69632},		// 10	+24Bit		= ��׼+24Bit+65536+4096
	{0x7	,	3, 32,'M',	    0,	0},			// 111  +32Bit		= 32Bit Org
};

static BITCODE amountCode[] =	//�����ѹ�� �Գɽ���*����*ÿ�ֹ���Ϊ��׼(��������)
{
	{0x6	,	3, 12,'b',	   12,	0},			// 110	+12Bit		= ��׼+12Bit
	{0x2	,	2, 16,'b',	   16,2048},		// 10	+16Bit		= ��׼+16Bit+2048
	{0x0	,	1, 24,'b',	   24,34816},		// 0	+24Bit		= ��׼+24Bit+32768+2048
	{0x7	,	3, 32,'M',	   0,	0},			// 111	+32Bit		= 32Bit float
};

static BITCODE amountDiffCode[] =	//���ѹ��������һ�εĳɽ���+����*Ϊ����*ÿ�ֹ���Ϊ��׼(��������)��ָ������
{
	{0x0	,	1, 0,'E',	   0,	0},			// 0				= ��׼
	{0x4	,	3, 4,'b',	   4,	0},			// 100	+4Bit		= ��׼+4Bit
	{0x5	,	3, 8,'b',	   8,	8},			// 101	+8Bit		= ��׼+8Bit+8
	{0xC	,	4,16,'b',	  16, 136},			// 1100	+16Bit		= ��׼+16Bit+128+8
	{0xD	,	4,24,'b',	  24, 32904},		// 1101	+16Bit		= ��׼+24Bit+32768+128+8
	{0x7	,	3,32,'M',	   0,	0},			// 111	+32Bit		= 32Bit float
};

static BITCODE mmpPriceDiffCode[] =		//���ѹ�������̼۸�
{
	{0x0	,	1,  0,'E',(DWORD)-1,0},			// 0				= ����
	{0x4	,	3,  0,'E',  0x90,	0},			// 100+Ins			= ������5��ɾ����5
	{0x5	,	3,  0,'E',  0x09,	0},			// 101+Ins			= ������5��ɾ����5
	{0x18	,	5,  0,'E',	0x95,	0},			// 11000+Ins		= ������5��ɾ����1
	{0x19	,	5,  0,'E',	0x04,	0},			// 11001+Ins		= ������5��ɾ����1
	{0x1A	,	5,  0,'E',	0x59,	0},			// 11010+Ins		= ������1��ɾ����5
	{0x1B	,	5,  0,'E',	0x40,	0},			// 11011+Ins		= ������1��ɾ����5
	{0xE	,	4,  8,'B',	   8,	0},			// 1110+4Bit+4Bit+Ins= ����1ɾ��1,4Bit��ʾ���λ��0-9,4Bit��ʾɾ��λ��0-9
	{0x1E	,	5,  0,'E',(DWORD)-2,0},			// 11110			= ��������Ҫʹ�÷���ط����õ��۸�ֲ���������
	{0x3E	,	6,  0,'E',(DWORD)-3,0},			// 111110			= �����̼۸���ң���Ҫʹ�÷���ط����õ��۸�ֲ��������棬���������þ���λ�ñ���
	{0x3F	,	6,  0,'E',(DWORD)-4,0},			// 111111			= �����̼۸���ң����Ǽ۸񲻱䣬���������þ���λ�ñ���
};

static BITCODE mmpInsCode[] =	//���ѹ�������������۸�
{
	{0xE	,	4,  0,'E',	   0,	0},			// 1110				= ��ֵΪ0
	{0x0	,	1,  0,'E',	   1,	0},			// 0				= �۲�1
	{0x2	,	2,  4,'B',	   4,   1},			// 10	+4Bit		= �۲�1+4Bit
	{0x6	,	3, 16,'B',	  16,  17},			// 110  +16Bit		= �۲�1+16+16Bit
	{0xF	,	4, 32,'D',	   0,   0},			// 1111 +32Bit		= DWORD
};

static BITCODE mmpVolPosDiffCode[] =		//���ѹ������������λ��
{
	{0x2	,	2,  0,'E',	  0,	0},			// 10				= 0����
	{0x3	,	2,  4,'P',	 10,	0},			// 11+4Bit			= 1������λ����4Bit
	{0x0	,	1, 10,'D',	  0,	0},			// 0+10Bit			= ��10��Bitλ�þ����Ƿ�����������λ��
};

static BITCODE mmpErrorVolPosCode[] =		//��������������λ��
{
	{0x0	,	1,  0,'E',0x400,	0},			// 10				= 0����
	{0x1	,	1, 10,'D',	  0,	0},			// 0+10Bit			= ��10��Bitλ�þ����Ƿ�����������λ��
};

static BITCODE mmpPriceNumCode[] =			//����������
{
	{0x0	,	1,  0,'E', 0x2D,	0},			//0				= 10��
	{0x4	,	3,  0,'E', 0x28,	0},			//100			= 5��0��
	{0xE	,	4,  0,'E',  0x5,	0},			//1110			= 0��5��
	{0x5	,	3,  2,'B',	  2, 0x29},			//101+2Bit		= 5��N��, N = 2Bit+1
	{0x6	,	3,  2,'S',0x3002, 0xD},			//110+2Bit		= N��5��, N = 2Bit+1
	{0x1E	,	5,  6,'B',	  6,	0},			//11110+3Bit+3Bit	= N��M��, N = 3Bit, M = 3Bit
	{0x1F	,	5,  6,'B',	  6, 0x40},			//11111+3Bit+3Bit	= �쳣�����̣�N��M��, N = 3Bit, M = 3Bit
};

static BITCODE mmpDiffGapPosCode[] =			//������Gapλ�á�����,���ѹ��
{
	{0x7	,	3,  0,'E',	  0,	0},			// 111				= ����
	{0x4	,	3,  0,'E', 0x10,	0},			// 100				= 1 Gap��λ����4(��1��1֮��)
	{0x5	,	3,  4,'P',	  9,	0},			// 101+4Bit			= 1 Gap��λ����4Bit
	{0x6	,	3,  4,'P',	  9, 0x10},			// 110+4Bit			= 2 Gap��1��λ����4Bit,��һ��Ϊ4
	{0x0	,	1,  9,'D',	  0,	0},			// 0+9Bit			= ��9��λ�ã�9Bitÿһλ��ʾ��λ���Ƿ���Gap
};

static BITCODE mmpGapPosCode[] =			//������Gapλ�á�����,�����ѹ��
{
	{0x0	,	1,  0,'E',	  0,	0},			// 0				= ����
	{0xE	,	4,  0,'E', 0x10,	0},			// 1110				= 1 Gap��λ����4(��1��1֮��)
	{0x6	,	3,  4,'P',	  9,	0},			// 110+4Bit			= 1 Gap��λ����4Bit
	{0xF	,	4,  4,'P',	  9, 0x10},			// 1111+4Bit		= 2 Gap��1��λ����4Bit,��һ��Ϊ4
	{0x2	,	2,  9,'D',	  0,	0},			// 10+9Bit			= ��9��λ�ã�9Bitÿһλ��ʾ��λ���Ƿ���Gap
};

static BITCODE mmpPosCode[] = //��1�۸�λ
{
	{0x0	,	2,  0,'E',999999,	0},			// 00				= ��1 = ���¼�
	{0x1	,	2,  0,'E',	   0,	0},			// 01				= ��1 = ���¼�
	{0x2	,	2,  4,'b',	   4,	0},			// 10+4Bit			= ��1 = ���¼�+4Bit(������)
	{0x6	,	3,  8,'b',	   8,   8},			// 110+8Bit			= ��1 = ���¼�+8Bit(������)+8
	{0x7	,	3, 32,'D',	   0,	0},			// 111+32Bit		= ��1 = DWORD
};

static BITCODE mmpGapCode[] = //������Gap��С
{
	{0xE	,	4,  0,'E',	   0,	0},			// 1110				= 0��λ
	{0x0	,	1,  0,'E',	   2,	0},			// 0				= 2��λ
	{0xC	,	4,  0,'E',	   3,	0},			// 1100				= 3��λ
	{0x2	,	2,  4,'B',	   4,	4},			// 10+4Bit			= 4+4Bit��λ
	{0xD	,	4,  8,'B',	   8,  20},			// 1101+8Bit		= 4+16+8Bit��λ
	{0x1E	,	5,  8,'m',	   8,	0},			// 11110+8Bit		= 8Bit����,����������
	{0x1F	,	5, 32,'D',	   0,	0},			// 11111+32Bit		= DWORD
};

static BITCODE mmpDiffVolCode[] =	//���ѹ����������
{
	{0x0	,	1,  6,'b',	   6,	0},			// 0+6Bit			= ��׼+6Bit
	{0x5	,	3,  4,'Z',	   2,	1},			// 101+2Bit+2Bit	= ��׼+2Bit*10^2Bit
	{0xE	,	4,  6,'Z',	   4,	5},			// 1110+4Bit+2Bit	= ��׼+(4Bit+4)*10^2Bit
	{0x4	,	3,  8,'b',	   8,  32},			// 100+8Bit			= ��׼+8Bit+32
	{0x6	,	3, 12,'b',	  12, 160},			// 110+12Bit		= ��׼+12Bit+128+32
	{0x1E	,	5, 16,'b',	  16,2208},			// 11110+16Bit		= ��׼+16Bit+2048+128+32
	{0x1F	,	5, 32,'D',	   0,	0},			// 11111+32Bit		= 32Bit DWORD
};

static BITCODE mmpVolCode[] =	//�����ѹ����������
{
	{0x1	,	2,  6,'B',	   6,	0},			// 01	+6Bit		= 6Bit
	{0x0	,	2,  8,'B',	   8,  64},			// 00	+8Bit		= 8Bit+64
	{0x6	,	3, 12,'B',	  12, 320},			// 110	+12Bit		= 12Bit+256+64
	{0x2	,	2, 16,'B',	  16,4416},			// 10	+16Bit		= 16Bit+4096+256+64
	{0xE	,	4, 24,'B',	  24,69952},		// 1110 +24Bit		= 24Bit+65536+4096+256+64
	{0xF	,	4, 32,'D',	   0,	0},			// 1111 +32Bit		= 32Bit DWORD
};


static BITCODE hkSpreadCode[] =	//�����̼۲�
{
	{0x2	,	2,  0,'E',	   1,	0},			//	10				= 1 (10 - 250)									1826	800
	{0x0	,	1,  0,'E',	   5,	0},			//	0				= 5 (250 - 500)									1038	300
	{0x6	,	3,  0,'E',	  10,	0},			//	110				= 10 (500 - 2000)								715		400
	{0xE	,	4,  0,'E',	  25,	0},			//	1110			= 25 (2000 - 5000)								322		150
	{0x1E	,	5,  0,'E',	  50,	0},			//	11110			= 50 (5000 - 30000)		3(500 - 9999950)		162		120
	{0xF8	,	8,  0,'E',	 100,	0},			//	11111000		= 100 (30000 - 50000)							40		12
	{0xF9	,	8,  0,'E',	 250,	0},			//	11111001		= 250 (50000 - 100000)							28		16
	{0xFA	,	8,  0,'E',	 500,	0},			//	11111010		= 500 (100000 - 200000)							12		11
	{0xFB	,	8,  0,'E',	1000,	0},			//	11111011		= 1000 (200000 - 1000000)						1
	{0xFC	,	8,  0,'E',	2500,	0},			//	11111100		= 2500 (1000000 - 9995000)
	{0xFD	,	8,  0,'E',	 125,	0},			//	11111101		= 125 2(500 - 9999875)
	{0xFE	,	8,  0,'E',	   0,	0},			//	11111110		= 0
	{0xFF	,	8, 16,'D',	   0,	0},			//	11111111		= 16Bit org
};

static BITCODE hkMmpPosCode[] =	//������λ��
{
	{0x0	,	2,  0,'E',	   0,	0},			//	00				= 0
	{0x1	,	2,  0,'E',	   1,	0},			//	01				= 1
	{0x1E	,	5,  0,'E',(DWORD)-1,0},			//	11110			= -1
	{0x1F	,	5,  0,'E',(DWORD)-2,0},			//	11111			= -2
	{0x4	,	3,  2,'B',	   2,	2},			//	100				= 2Bit +2
	{0x5	,	3,  4,'B',	   4,	6},			//	101				= 4Bit +6
	{0xC	,	4,  8,'B',	   8,999999},		//	1100			= 8Bit
	{0xD	,	4, 16,'B',	  16,999999},		//	1101			= 16Bit
	{0xE	,	4, 32,'D',	   0,999999},		//	1110			= 32Bit Org
};

static BITCODE hkVolZeroCode[] =	//�ɽ���0�ĸ���
{
	{0xE	,	4,  0,'E',	   0,	0},			//	1110			= 0
	{0x1E	,	5,  0,'E',	   1,	0},			//	11110			= 1
	{0x6	,	3,  0,'E',	   2,	0},			//	110				= 2
	{0x0	,	2,  0,'E',	   3,	0},			//	00				= 3
	{0x1	,	2,  0,'E',	   4,	0},			//	01				= 4
	{0x2	,	2,  2,'B',	   2,	5},			//	10				= 2Bit+5
	{0x1F	,	5,  4,'D',	   0,	9},			//	11111			= 4Bit+5
};

static BITCODE hkDiffVolCode[] =	//���ѹ����
{
	{0x0	,	1,  2,'B',	   2,	0},			// 0+2Bit		= ��׼+2Bit
	{0x2	,	2,  4,'B',	   4,	4},			// 10+4Bit		= ��׼+4Bit+4
	{0x6	,	3,  8,'B',	   8,  20},			// 110+8Bit		= ��׼+8Bit+16+4
	{0xE	,	4, 12,'B',	  12, 276},			// 1110+12Bit	= ��׼+12Bit+256+16+4
	{0x1E	,	5, 16,'B',	  16,4372},			// 11110+16Bit	= ��׼+16Bit+4096+256+16+4
	{0x1F	,	5, 32,'M',	   0,	0},			// 11111+32Bit	= 32Bit MWORD
};

static BITCODE hkVolCode[6] =	//�����ѹ����
{
	{0x4	,	3,  6,'B',	   6,	0},			// 100+4Bit		= 6Bit
	{0x5	,	3,  8,'B',	   8,  64},			// 101+8Bit		= 8Bit+64
	{0x0	,	2, 12,'B',	  12, 320},			// 00+12Bit		= 12Bit+256+64
	{0x1	,	2, 16,'B',	  16,4416},			// 01+16Bit		= 16Bit+4096+256+64
	{0x6	,	3, 24,'B',	  24,69952},		// 110+24Bit	= 24Bit+65536+4096+256+64
	{0x7	,	3, 32,'M',	   0,	0},			// 111+32Bit	= 32Bit MWORD
};

static BITCODE hkMmpDiffVolCode[] =	//���ѹ����������
{
	{0x0	,	1,  4,'B',	   4,	0},			// 0+4Bit		= ��׼+4Bit
	{0x4	,	3,  6,'B',	   6,	8},			// 100+6Bit		= ��׼+6Bit+8
	{0xD	,	4,  8,'B',	   8,  72},			// 1101+8Bit	= ��׼+8Bit+64+8
	{0xC	,	4, 12,'B',	  12, 328},			// 1100+12Bit	= ��׼+12Bit+256+64+8
	{0x3C	,	6, 16,'B',	  16,4424},			// 111100+16Bit	= ��׼+16Bit+4096+256+64+8
	{0x5	,	3,  6,'B',	   6,(DWORD)-64},	// 101+8Bit		= ��׼+6Bit����
	{0xE	,   4, 12,'B',	  12,(DWORD)-4160}, // 1110+12Bit	= ��׼+12Bit����-64
	{0x3D	,   6, 16,'B',	  16,(DWORD)-69696},// 111101+16Bit	= ��׼+16Bit����-4096-64
	{0x3E	,	6, 24,'B',	  24,69960},		// 111110+24Bit	= ��׼+24Bit+65536+4096+256+64+8
	{0x3F	,	6, 32,'D',	   0,	0},			// 111111+32Bit	= 32Bit DWORD
};

static BITCODE hkMmpVolCode[] =	//�����ѹ����
{
	{0x0	,	2,  3,'B',	   3,	0},			// 00+3Bit		= ��׼+3Bit
	{0x1	,	2,  6,'B',	   6,	8},			// 01+6Bit		= ��׼+6Bit+8
	{0x2	,	2, 12,'B',	  12,  72},			// 10+12Bit		= ��׼+12Bit+64+8
	{0x6	,	3, 24,'B',	  24,4168},			// 110+24Bit	= ��׼+24Bit+4096+64+8
	{0x7	,	3, 32,'D',	   0,	0},			// 111+32Bit	= 32Bit DWORD
};

static BITCODE innerVolCode[] =	//������
{
	{0x0	,	2,  8,'b',	   8,  0},			// 00+8Bit		= 8Bit
	{0x1	,	2, 12,'b',	  12, 128},			// 01+12Bit		= 12Bit+128
	{0x2	,	2, 16,'b',	  16,2176},			// 10+16Bit		= 16Bit+2048+128
	{0x6	,	3, 24,'b',	  24,34944},		// 110+24Bit	= 24Bit+32768+2048+128
	{0x7	,	3, 32,'M',	   0,	0},			// 111+32Bit	= 32Bit MWORD
};

//ѹ����չ������

static BITCODE lastCloseCode[] =	//����
{
	{0x2	,	2,  8,'B',		8,  0},			// 10	+8Bit		= 8Bit
	{0x0	,	1, 16,'B',	   16,256},			// 0	+16Bit		= 16Bit+256
	{0x6	,	3, 24,'B',	   24,65792},		// 110	+24Bit		= 24Bit+65536+256
	{0x7	,	3, 32,'D',	    0,	0},			// 111	+32Bit		= 32Bit Org
};

class ExtractDC
{
public:
	ExtractDC():stk_total_(0),static_tag(false),static_before_dyna_tag(false) {
	};
    ~ExtractDC(){};
	void inline set_static_before_dyna_tag(bool tag)
	{
		static_before_dyna_tag = tag;
	}
    int ExtractData(int dc_type, const unsigned char *pbufsrc, int bufsizesrc, unsigned char * pbufdes, int bufsizedes, STK_DYNA *pStkDyna, STK_STATIC *pStkStatic);
    int ExtractStaticData(const BYTE* pData,int nDataLen,STK_STATIC* pStaticBuf,int& nBufSize);
    int ExtractDynaData(const BYTE* pData,int nDataLen,STK_DYNA* pDynaBuf,int& nBufSize, STK_DYNA *pStkDyna, STK_STATIC *pStkStatic);
    int ExpandL2MMPEx(const BYTE* pData,int nDataLen,SH_L2_MMPEX* pMMPExBuf,int& nBufSize);

    BOOL ExtractStaticStream(STK_STATIC* pData,WORD& wStockID,DWORD& dwLastLabel);
    BOOL ExtractDynaStream(CBitStream& stream,STK_DYNA* pDyna,WORD& wStockID,BYTE cVersion, STK_DYNA *pStkDyna, STK_STATIC *pStkStatic,const STK_DYNA* pOldDyna=NULL,const STK_STATIC* pStatic=NULL);


	BOOL IsFuture(){return (m_cHeadAttr_&CPS_DYNA_HEAD::TYPEMASK)==CPS_DYNA_HEAD::FUTURE;}
	BOOL IsFutureIndex(){return (m_cHeadAttr_&CPS_DYNA_HEAD::TYPEMASK)==CPS_DYNA_HEAD::FTR_IDX;}
	BOOL IsHKSec(){return (m_cHeadAttr_&CPS_DYNA_HEAD::TYPEMASK)==CPS_DYNA_HEAD::HKSEC;}
	BOOL IsFExg(){return (m_cHeadAttr_&CPS_DYNA_HEAD::TYPEMASK)==CPS_DYNA_HEAD::FOREIGN_EXG;}

    BOOL HasTickCount(){return m_cHeadAttr_&CPS_DYNA_HEAD::HAS_TICK_COUNT ? TRUE : FALSE;}
	BOOL HasAmount(){return m_cHeadAttr_&CPS_DYNA_HEAD::HAS_AMOUNT ? TRUE : FALSE;}
	
	void set_stk_total(int stk_total)
	{
		stk_total_ = stk_total;
	}

    const struct STK_DYNA* GetOldDynaByID(STK_DYNA * pStkDyna, WORD index);
    const struct STK_STATIC* GetStkByID(STK_STATIC * pStkStatic, WORD index);

	DWORD GetDefaultHKSpread(DWORD dwPrice);

	WORD m_wMarket;
	BOOL m_bNoMMP;			//��������ѹ��

    WORD m_wMarketStatic;
protected:
    BOOL m_bFullData;
	CBitStream m_stream;

	CPS_STATIC_HEAD* m_pStaticHead;
private:
    BOOL m_bSeqCompress_;
    enum COMPRESS_TYPE
	{
		DYNA = 0,			//��̬����
		TICK,				//��ʳɽ�
		SEP_DYNA			//����ѹ����̬����
	} m_compressType_;
    BYTE m_cHeadAttr_;		//CPS_TICK_HEAD::m_cAttrib or CPS_DYNA_HEAD::m_cAttrib
	time_t m_baseTime_;		//��̬ѹ����׼ʱ��
	bool static_tag;
	bool static_before_dyna_tag;
	int stk_total_;
};

#pragma pack(pop)

#endif // MONITOR_EXTRACT_DC_H_
