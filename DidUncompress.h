#ifndef _UNCOM_
#define _UNCOM_
#include "dscomapi.h"
#include <log4cxx/logger.h>

#ifdef _MSC_VER
	#include "MSXML_DOMDocument.h"
#else
	#include "LIBXML_DOMDocument.h"
#endif

#include <map>

#ifdef _MSC_VER
	#include <hash_map>
#else
	
	#include <ext/hash_map>
#endif

using namespace std;

#ifdef _MSC_VER
	typedef hash_map<unsigned int, int> STKIDMAP;
#else
	typedef __gnu_cxx::hash_map<unsigned int,int> STKIDMAP;
#endif


struct TStructForGetBmData 
{
	STKIDMAP* pStkIDMapHash;			//stkid hashӳ���ϵ
	char* pFullData;					//ȫ�������ڴ��ַ
	unsigned int iBufferSize;			//ȫ�������ڴ��С
	unsigned int iTotalStk;    
	TStructForGetBmData()
	{
		memset(	this, 
				0x0, 
				sizeof(TStructForGetBmData));
	}
	TStructForGetBmData(const TStructForGetBmData& obj)
	{
		memcpy(	this, 
				&obj, 
				sizeof(TStructForGetBmData));
	}
};

class DataBuffer
{
public:
	DataBuffer();
	~DataBuffer();
	int Allocate(int lt);
	void Flush();
	char* GetData();
	int GetLen();
private:
	char* data;
	int len;
};

class DidUncompress
{
public:
	struct TDidInfo 
	{
		int did;
		string strFilePath;				//did ��������ģ���ļ�·��
		bool bCompress;					//�Ƿ�����������ѹ�����did ����
		bool bFull;						//�Ƿ�����������������did ���ݣ�Ϊfalse ��ʾ������������
		bool bServerSupport;			//��¼������Ƿ�֧�ָ�did
		bool bNeedSynaDidStructFile;	//�Ƿ���Ҫ�����ͬ����������ģ���ļ�
		CDidStructApi* pDidStruct;		//��������ģ��
		CDsCompressApi* pUnCompress;	//��ѹ������
		char* pUnCompressBuffer;		//��ѹ��������
		int iBufferLen;					//��������С
		bool bInitialized;				//��ʼ���ɹ�

		TDidInfo(){
			did = 0;
			strFilePath = "";
			bCompress = true;
			bFull = true;
			bServerSupport = false;
			bNeedSynaDidStructFile = true;
			pDidStruct = NULL;
			pUnCompress = NULL;
			pUnCompressBuffer = NULL;
			iBufferLen = 0;
			bInitialized = false;
		}

		TDidInfo(const TDidInfo& obj)
		{
			did = obj.did;
			strFilePath = obj.strFilePath;
			bCompress = obj.bCompress;
			bFull = obj.bFull;
			bServerSupport = obj.bServerSupport;
			bNeedSynaDidStructFile = obj.bNeedSynaDidStructFile;
			pDidStruct = obj.pDidStruct;
			pUnCompress = obj.pUnCompress;
			pUnCompressBuffer = obj.pUnCompressBuffer;
			iBufferLen = obj.iBufferLen;
			bInitialized = obj.bInitialized;
		}
	};

public:
	DidUncompress(string & path);
	~DidUncompress(void);
	int ReadConfig();
	void Initialize();
	//int Uncompress(char* source_data, int source_size, int num, int did, char* des_data, int des_size );
	//int Uncompress();
	//For Test
	int Compress(char* source_data, int source_size, int num, int did, char* des_data, int& des_size );
	int DisassemblePack(DC_HEAD* pPack,DataBuffer& buf);

private:
	void ReadDidStruct(STEP::DOMNode* node);
	int UpdateDSData(TDidInfo* pDidFile,char* pTemplateData,int len);

private:
	string configFilePath;
	map<int, TDidInfo> m_mapDidFile;
	map<int,TStructForGetBmData> m_mapStructForBmData;
	static log4cxx::LoggerPtr logger_;
};


#endif /*_UNCOM_*/
