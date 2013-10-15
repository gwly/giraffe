/********************************************************************
	created:	2013:1:17   13:16
	filename: 	dscomapi.h
	author:		fu.changyou
	
	purpose:	����ģ��ѹ���ӿ��ļ�����
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


//did����ģ�������
class COMPRESS_API CDidStructApi:private JUncopyable
{
public:
    DWORD m_dwCrc;  //ģ���ļ�CRC
    Dids* m_pDids;  //did�б�
    ds_did_info mDidInfo;   //�ֶ�
    ds_field_info* m_pFieldsInfo;   //�ֶ�����
    Cids* m_pCids;  //ѹ���㷨����
    CodeBitIds* m_pCodeBitIds;  //ѹ����������
    StrIds* m_pStrIds;  //�ַ����б�
    static unsigned int ds_crc32_table256[];    //����crc��

public:
    //����index���ַ����б��л�ȡ�ַ���
    static int getIndexByStr(char* str,StrList* pStrList);  

    //����crc
    static unsigned int ds_cal_crc(const char* p,unsigned int len);

    //�ֲ�����crc,��������ϼ���crc ^ 0xffffffff����
    static unsigned int ds_cal_crc_part(unsigned int oldcrc,const char* p,unsigned int len);

    //����DIDStruct����
    static CDidStructApi* CreateDidStructObj();

    //����DidStruct����
    static void DestoryDidStructObj(CDidStructApi* pDidStruct);

    //���ز�����did����ģ��
    virtual int LoadMemXmlBuffer(const char* xmlbuf,unsigned int xmlbuflen) = 0;   

    //��ӡ�ֶ�
    virtual int PrintfFields(void) = 0;
};

class CDidStruct;

#ifdef _REDHAT_VERSION
class CSlice;
#endif

//ָ������ָ��
//�ú�������ѹ��ʱȡ�ùؼ�����ͬ����һ����¼��Ϣ
//num��ʾpLastArgs�����ж��ٸ�
//pLastArgs������ǰ����֮ǰ����Ĺؼ�����Ϣ
//pNowArgs������ǰ������ֶ���Ϣ
//pDidStruct������ģ���ļ�����Ϣ
//pUserArg�����û��Լ�ָ�������ݵ�ַ
//ע�⵱�ùؼ��ּ�¼��������ʱ���뷵��NULL
//pSaveField�����������������ȡ�ô���ģ����ֶ���Ϣ�������ط�������
typedef char* (*LPGetBmData)(int num,LastArgs* pLastArgs,LastArgs* pNowArgs,DWORD did,CDidStruct* pDidStruct,char* pUserArg,ds_field_info*& pSaveField);

//�ú�������ĳ����¼ѹ�����ʱ�����ڴ棬��������һ�������ж����ؼ�����ͬ�ļ�¼��
//pNowArgs����������һ���ؼ�����Ϣ
//pRecBuf��¼��ʼ��ַ
//pUserArg�����û��Լ�ָ�������ݵ�ַ
typedef bool (*LPSetBmData)(LastArgs* pNowArgs,DWORD did,CDidStruct* pDidStruct,char* pUserArg,char* pRecBuf);


#ifdef __cplusplus
extern "C" {
#endif
//ѹ���ӿ�
class COMPRESS_API CDsCompressApi:private JUncopyable
{
public:
    //����ѹ������
    static CDsCompressApi* CreateCompressObj();

    //����ѹ������
    static void DestroyCompressObj(CDsCompressApi* pCompress);

    //��ʼ��
    virtual bool Initialize(CDidStructApi* pDidStruct,//pDidStruct���ݽṹ��ѹ���㷨����
        DWORD did,//����did
        int cid,//cidѹ���㷨id
        int lastcmp=0,//Ϊ1�������ܸ�last��¼��Ҫ���Ƚϣ�Ϊ1ʱ�ӵڶ�����ʼ�������йؼ��ֶ��ظ��ļ�¼��һ�����ݰ���
        DWORD staticday=0,//��ѹ���㷨����̬��Ϣ�������ʱ��Ҫ
        LPGetBmData pGetBmData=NULL,LPSetBmData pSetBmData=NULL,//pGetBmDataȡ����ͬ�ؼ��ֵ���һ����¼��pSetBmData���ݹؼ��ָ��¼�¼
        char* pUserArg=NULL,//pUserArg�����û��Լ�ָ�������ݵ�ַ,����ͻ�������ʹ�õķ��������ͣ��г���Ϣ��
        CDidStructApi* pComDidStruct=NULL//pComDidStruct��������ѹ���㷨��ģ��,���Ϊ�ձ�ʾ��pDidStruct����һ��
        ) = 0;


    //ѹ������
    //�����Ҫ�����ѹ��������Ԥ������
    //ʧ�ܷ���0���ɹ�����recnum����¼����Ԥ�����ѹ������������
    virtual int GetCompressOutputBufferLen(int recnum=1) = 0;
    //����ѹ������Ŀ�껺����
    virtual bool SetCompressOutputBuffer(char* pDestBuf,int destbuf_len) = 0;
    //һ����ѹ��������¼����
    //ѹ��ʧ�ܷ���-1���ɹ�����1
    //pOrgData��ѹ������,orgdata_len��ѹ�����ݳ���,��¼��
    //ѹ������Ժ����FinishCompressedData�������ѹ������ȡѹ��������ݳ���
    virtual int CompressData(char* pOrgData,UINT64 orgdata_len,int recnum) = 0;
    //ѹ��һ����¼,�ú�����������ѹ��(��last��¼��Ҫ���Ƚ�),���ڲ����˷���ѭ�����ö��ṩ,
    //ѹ��ȫ������Ժ����FinishCompressedData�������ѹ������ȡѹ��������ݳ���
    //pOrgOneRec��ѹ��������¼,onerec_len��ѹ��������¼,��¼��
    //ѹ��ʧ�ܷ���-1���ɹ�����1
    virtual int CompressDataOneRecData(	char* pOrgOneRec,UINT64 onerec_len) = 0;
    //���ѹ��,���ѹ���Ժ��Ŀ�껺���������ݳ���
    virtual int FinishCompressedData(void) = 0; 

    //��ȡ��ǰѹ���Ժ��Ŀ�껺�������ݳ���
    virtual int GetCompressedDataLen(void) = 0;
    //ѹ�����̽���


    //��ѹ����
    //���ý�ѹ����Ŀ�껺����
    virtual bool SetUnCompressOutputBuffer(char* pDestBuf,int destbuf_len) = 0;
    //��ѹ����
    //pOrgData����ѹ����,orgdata_len����ѹ���ݳ��ȣ�recnum����ѹ��¼��
    //��ѹʧ�ܷ���-1���ɹ�����1
    virtual int UnCompressData(char* pOrgData,UINT64 orgdata_len,int recnum,int fd=0) = 0;
    //��ɽ�ѹ��,��ý�ѹ���Ժ��Ŀ�껺���������ݳ���
    virtual int FinishUnCompressedData(bool bCmbCrc=true) = 0;
    //��ѹ���̽���

#ifdef _REDHAT_VERSION
    virtual int CompressZmAllRecsFullData(CSlice* pSlice) = 0;
#endif
};

#ifdef __cplusplus
}
#endif

#endif
