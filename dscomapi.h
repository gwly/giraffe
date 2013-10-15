/********************************************************************
	created:	2013:1:17   13:16
	filename: 	dscomapi.h
	author:		fu.changyou
	
	purpose:	数据模板压缩接口文件定义
*********************************************************************/
#ifndef _DZH_DS_COMPRESS_API_H_
#define _DZH_DS_COMPRESS_API_H_
#include "ds_dat.h"
#ifndef BYTE
typedef unsigned char BYTE;
#endif

#ifndef COMPRESS_API
#ifdef DLL_FILE
#define COMPRESS_API _declspec(dllexport)
#elif _LINUX_VERSION
#define  COMPRESS_API
#else
#define COMPRESS_API _declspec(dllimport)
#endif
#endif


//did数据模板解析器
class COMPRESS_API CDidStructApi:private JUncopyable
{
public:
    DWORD m_dwCrc;  //模板文件CRC
    Dids* m_pDids;  //did列表
    ds_did_info mDidInfo;   //字段
    ds_field_info* m_pFieldsInfo;   //字段数组
    Cids* m_pCids;  //压缩算法数组
    CodeBitIds* m_pCodeBitIds;  //压缩编码数组
    StrIds* m_pStrIds;  //字符串列表
    static unsigned int ds_crc32_table256[];    //计算crc用

public:
    //根据index从字符串列表中获取字符串
    static int getIndexByStr(char* str,StrList* pStrList);  

    //计算crc
    static unsigned int ds_cal_crc(const char* p,unsigned int len);

    //分步计算crc,最后计算完毕加上crc ^ 0xffffffff即可
    static unsigned int ds_cal_crc_part(unsigned int oldcrc,const char* p,unsigned int len);

    //创建DIDStruct对象
    static CDidStructApi* CreateDidStructObj();

    //销毁DidStruct对象
    static void DestoryDidStructObj(CDidStructApi* pDidStruct);

    //加载并解析did数据模板
    virtual int LoadMemXmlBuffer(const char* xmlbuf,unsigned int xmlbuflen) = 0;   

    //打印字段
    virtual int PrintfFields(void) = 0;
};

class CDidStruct;

#ifdef _REDHAT_VERSION
class CSlice;
#endif

//指向函数的指针
//该函数用来压缩时取得关键字相同的上一条记录信息
//num表示pLastArgs数组有多少个
//pLastArgs描述当前级别之前级别的关键字信息
//pNowArgs描述当前级别的字段信息
//pDidStruct描述该模板文件的信息
//pUserArg用于用户自己指定的数据地址
//注意当该关键字记录还不存在时，请返回NULL
//pSaveField参数用于行情服务器取得存贮模板的字段信息，其它地方无意义
typedef char* (*LPGetBmData)(int num,LastArgs* pLastArgs,LastArgs* pNowArgs,DWORD did,CDidStruct* pDidStruct,char* pUserArg,ds_field_info*& pSaveField);

//该函数用来某条记录压缩完成时更新内存，（解决如果一个包里有多条关键字相同的记录）
//pNowArgs用来描数第一级关键字信息
//pRecBuf记录起始地址
//pUserArg用于用户自己指定的数据地址
typedef bool (*LPSetBmData)(LastArgs* pNowArgs,DWORD did,CDidStruct* pDidStruct,char* pUserArg,char* pRecBuf);


#ifdef __cplusplus
extern "C" {
#endif
//压缩接口
class COMPRESS_API CDsCompressApi:private JUncopyable
{
public:
    //创建压缩对象
    static CDsCompressApi* CreateCompressObj();

    //销毁压缩对象
    static void DestroyCompressObj(CDsCompressApi* pCompress);

    //初始化
    virtual bool Initialize(CDidStructApi* pDidStruct,//pDidStruct数据结构，压缩算法描述
        DWORD did,//数据did
        int cid,//cid压缩算法id
        int lastcmp=0,//为1表明可能跟last记录需要做比较，为1时从第二级开始不允许有关键字段重复的记录在一个数据包里
        DWORD staticday=0,//当压缩算法跟静态信息日期相关时需要
        LPGetBmData pGetBmData=NULL,LPSetBmData pSetBmData=NULL,//pGetBmData取得相同关键字的上一条记录，pSetBmData根据关键字更新记录
        char* pUserArg=NULL,//pUserArg用于用户自己指定的数据地址,比如客户端驱动使用的服务器类型，市场信息等
        CDidStructApi* pComDidStruct=NULL//pComDidStruct用于描述压缩算法的模板,如果为空表示和pDidStruct参数一样
        ) = 0;


    //压缩过程
    //获得需要分配的压缩缓冲区预估长度
    //失败返回0，成功返回recnum条记录数需预分配的压缩缓冲区长度
    virtual int GetCompressOutputBufferLen(int recnum=1) = 0;
    //设置压缩数据目标缓冲区
    virtual bool SetCompressOutputBuffer(char* pDestBuf,int destbuf_len) = 0;
    //一次性压缩多条记录数据
    //压缩失败返回-1，成功返回1
    //pOrgData待压缩数据,orgdata_len待压缩数据长度,记录数
    //压缩完成以后调用FinishCompressedData函数完成压缩并获取压缩后的数据长度
    virtual int CompressData(char* pOrgData,UINT64 orgdata_len,int recnum) = 0;
    //压缩一条记录,该函数用于增量压缩(跟last记录需要做比较),便于采数端方便循环调用而提供,
    //压缩全部完成以后调用FinishCompressedData函数完成压缩并获取压缩后的数据长度
    //pOrgOneRec待压缩单条记录,onerec_len待压缩单条记录,记录数
    //压缩失败返回-1，成功返回1
    virtual int CompressDataOneRecData(	char* pOrgOneRec,UINT64 onerec_len) = 0;
    //完成压缩,获得压缩以后的目标缓冲区里数据长度
    virtual int FinishCompressedData(void) = 0; 

    //获取当前压缩以后的目标缓冲区数据长度
    virtual int GetCompressedDataLen(void) = 0;
    //压缩过程结束


    //解压过程
    //设置解压数据目标缓冲区
    virtual bool SetUnCompressOutputBuffer(char* pDestBuf,int destbuf_len) = 0;
    //解压数据
    //pOrgData待解压数据,orgdata_len待解压数据长度，recnum待解压记录数
    //解压失败返回-1，成功返回1
    virtual int UnCompressData(char* pOrgData,UINT64 orgdata_len,int recnum,int fd=0) = 0;
    //完成解压缩,获得解压缩以后的目标缓冲区里数据长度
    virtual int FinishUnCompressedData(bool bCmbCrc=true) = 0;
    //解压过程结束

#ifdef _REDHAT_VERSION
    virtual int CompressZmAllRecsFullData(CSlice* pSlice) = 0;
#endif
};

#ifdef __cplusplus
}
#endif

#endif
