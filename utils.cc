#include "utils.h"
#include "datacollect.h"
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

/*get flag name based on the flag number*/
char* Utils::tcp_flag_to_str( unsigned char flag )
{
	switch((unsigned short)flag)
	{
	case FIN:
		return "[FIN]";
		break;
	case SYN:
		return "[SYN]";
		break;
	case RST:
		return "[RST]";
		break;
	case PSH:
		return "[PSH]";
		break;
	case ACK:
		return "[ACK]";
		break;
	case FINACK:
		return "[FIN,ACK]";
		break;
	case SYNACK:
		return "[SYN,ACK]";
		break;
	case RSTACK:
		return "[RST,ACK]";
		break;
	case PSHACK:
		return "[PSH,ACK]";
		break;
	case FINPSHACK:
		return "[FIN,PSH,ACK]";
		break;
	case URG:
		return "[URG]";
		break;
	default:
		return "[UNKNOW]";
		break;
	}
}

//be used for getting the value of type MWORD.
int64 Utils::ToMword(unsigned long x)
{
    int i;
    int64 n;
    int mul;
    n = x&0x3fffffff ;  //low 30 bit
    mul=x>>30&0x3;// high 2 bit
    for(i=0;i<mul;i++)
        n*=16;
    return n;
}

unsigned int Utils::UINT24to32(unsigned short low,unsigned char high)
{
    return low + (high<<16);
}

char * Utils::DCTypeToString(int dc_type)
{
	switch(dc_type)
	{
	case DCT_NUL:
		return "DCT_NUL";
	case DCT_KEEPALIVE:
		return "DCT_KEEPALIVE";
	case DCT_LOGIN:
		return "DCT_LOGIN";
	case DCT_REQPASS:
		return "DCT_REQPASS";
	case DCT_USERnPASS:
		return "DCT_USERnPASS";
	case DCT_READY:
		return "DCT_READY";
	case DCT_RESEND:
		return "DCT_RESEND";
	case DCT_STKSTATIC:
		return "DCT_STKSTATIC";
	case DCT_STKDYNA:
		return "DCT_STKDYNA";
	case DCT_SHL2_MMPEx:
		return "DCT_SHL2_MMPEx";
	case DCT_SHL2_REPORT:
		return "DCT_SHL2_REPORT";
	case DCT_SHL2_BIG_WD:
		return "DCT_SHL2_BIG_WD";
	case DCT_SHL2_ACCU_WD:
		return "DCT_SHL2_ACCU_WD";
	case DCT_HK_STATIC:
		return "DCT_HK_STATIC";
	case DCT_HK_DYNA:
		return "DCT_HK_DYNA";
	case DCT_XML:
		return "DCT_XML";
	case DCT_SHL2_QUEUE:
		return "DCT_SHL2_QUEUE";
	case DCT_GENERAL:
		return "DCT_GENERAL";
	case DCT_USERSTAT:
		return "DCT_USERSTAT";
	case DCT_RAWDATA:
		return "DCT_RAWDATA";
	case DCT_NEWS:
		return "DCT_NEWS";
	case DCT_SZL2_ORDER_QUEUE:
		return "DCT_SZL2_ORDER_QUEUE";
	case DCT_SZL2_ORDER_STAT:
		return "DCT_SZL2_ORDER_STAT";
	case DCT_DSCRC:
		return "DCT_DSCRC";
	case DCT_DSDID:
		return "DCT_DSDID";
	case DCT_DSDATA:
		return "DCT_DSDATA";
	case DCT_DID:
		return "DCT_DID";
	case DCT_DIDSTATIC:
		return "DCT_DIDSTATIC";
	case DCT_INCODESTATIC:
		return "DCT_INCODESTATIC";
	case DCT_SZL2_ORDER_FIVE:
		return "DCT_SZL2_ORDER_FIVE";
	case DCT_SZL2_TRADE_FIVE:
		return "DCT_SZL2_TRADE_FIVE";
	default:
		return "UNKNOW DC_TYPE";
	}
}

char *Utils::DCGeneral_IntypeToString(int dc_general_intype)
{
	switch(dc_general_intype)
	{
	case GE_STATIC_EX:
		return "GE_STATIC_EX";
	case GE_HKDYNA:
		return "GE_HKDYNA";
	case GE_BLK_STK:
		return "GE_BLK_STK";
	case GE_IOPV:
		return "GE_IOPV";
	case GE_MATU_YLD:
		return "GE_MATU_YLD";
	case GE_HKREL_TXT:
		return "GE_HIREL_TXT";
	default:
		return "UNKNOW DC_GENERAL INNER TYPE";
	}
}

void Utils::Print_Thread_ID(pthread_t tid)
{
	size_t i;
	for(i=sizeof(i);i;--i)
		printf("%02x",*(((unsigned char *)&tid)+i-1));
	printf("\n");
}

void Utils::WriteIntoFile(const char *file_name, const char *mode, const void* data , size_t length)
{
	FILE * fp = fopen(file_name, mode);
	if(NULL != fp)
	{
		fwrite(data, 1, length + 1, fp);
		fclose(fp);
	}
	else
	{
		//cout<<"open file error!"<<endl;
		assert(0);
	}
}

void Utils::SleepUsec(const int usec)
{
	struct timeval tval;
	tval.tv_sec = 0;
	tval.tv_usec = usec;
	select(0,NULL,NULL,NULL,&tval);
}

unsigned long Utils::GetCurrentTime()
{
	struct timeval tval;
	gettimeofday(&tval, NULL);
	return tval.tv_sec*1000000+tval.tv_usec;	
}
