#ifdef _MSC_DER
	#include "StdAfx.h"
#endif
#include "DidUncompress.h"
#include "public.h"
#include "zlib.h"

#define MaxStocksCount 4000;

char* GetBmData(int num,LastArgs* pLastArgs,LastArgs* pNowArgs,DWORD did,CDidStruct* pDidStruct,char* pUserArg,ds_field_info*& pSaveField)
{
	if (NULL == pUserArg)
		return NULL;

	TStructForGetBmData* pObj = (TStructForGetBmData*)pUserArg;
	if (NULL == pObj->pFullData || NULL == pObj->pStkIDMapHash)
		return NULL;

	//one rec size
	int iRecSize = ((CDidStructApi*)pDidStruct)->mDidInfo.m_dwRRecSize;

	unsigned int stk_id = *((unsigned int*)pNowArgs->pRec);
	if(!pObj->pStkIDMapHash->count(stk_id))
		return NULL;

	int iPos = iRecSize * pObj->pStkIDMapHash->find(stk_id)->second;
	if (iPos + iRecSize > pObj->iBufferSize)
		return NULL;

	char* pRecordData = (pObj->pFullData) + iPos;
	if (*(unsigned int*)pRecordData != stk_id)
		return NULL;

	return pRecordData;
}

bool SetBmData(LastArgs* pNowArgs,DWORD did,CDidStruct* pDidStruct,char* pUserArg,char* pRecBuf)
{
	if (NULL == pUserArg)
		return false;

	TStructForGetBmData* pObj = (TStructForGetBmData*)pUserArg;
	if (NULL == pObj->pFullData || NULL == pObj->pStkIDMapHash)
		return false;

	int iPos = 0;
	int iRecSize = ((CDidStructApi*)pDidStruct)->mDidInfo.m_dwRRecSize;
	unsigned int stk_id = *((unsigned int*)pRecBuf);
	if(!pObj->pStkIDMapHash->count(stk_id))
	{
		if (pObj->iTotalStk >= pObj->iBufferSize/iRecSize)
			return false;
		(*(pObj->pStkIDMapHash))[stk_id] = pObj->iTotalStk;
		pObj->iTotalStk++;
		char* pPos = pObj->pFullData + ((pObj->iTotalStk-1) * iRecSize);

		//copy data to full data buffer
		memcpy(pPos, pRecBuf, iRecSize);
	}
	else
	{
		iPos = iRecSize * (*(pObj->pStkIDMapHash))[stk_id];

		if (iPos + iRecSize > pObj->iBufferSize)
			return false;

		char* pPos = pObj->pFullData;
		pPos += iPos;

		if (*(unsigned int*)pPos != stk_id)
			return false;

		//copy data to full data buffer
		memcpy(pPos, pRecBuf, iRecSize);
	}
	return true;
}



DataBuffer::DataBuffer()
{
	data = NULL;
	len = 0;
}

DataBuffer::~DataBuffer()
{
	if(data)
	{
		free(data);
		data = NULL;
	}
	len = 0;
}

int DataBuffer::Allocate(int lt)
{
	if(lt <= 0)return -1;

	if(data)
	{
		free(data);
		data = NULL;
	}

	data = (char*)malloc(lt);
	if(data)
		len = lt;
	else
	{
		len = 0;
		return -1;
	}
	return 1;
}

void DataBuffer::Flush()
{
	if(data)
	{
		free(data);
		data = NULL;
	}
	len = 0;
}

char* DataBuffer::GetData()
{
	return data;
}

int DataBuffer::GetLen()
{
	return len;
}

DidUncompress::DidUncompress(string& path)
{
	configFilePath = path;
	if(!m_mapDidFile.empty())
		m_mapDidFile.clear();
	if(!m_mapStructForBmData.empty())
		m_mapStructForBmData.clear();
}


DidUncompress::~DidUncompress(void)
{
	if(!m_mapDidFile.empty())
	{
		map<int, TDidInfo>::iterator i = m_mapDidFile.begin();
		for (; i != m_mapDidFile.end(); ++i)
		{
			if(i->second.pDidStruct)
				CDidStructApi::DestoryDidStructObj(i->second.pDidStruct);
			if(i->second.pUnCompress)
				CDsCompressApi::DestroyCompressObj(i->second.pUnCompress);
		}
		m_mapDidFile.clear();
	}

	if(!m_mapStructForBmData.empty())
	{
		map<int, TStructForGetBmData>::iterator i = m_mapStructForBmData.begin();
		for (; i != m_mapStructForBmData.end(); ++i)
		{
			if(i->second.pStkIDMapHash)
				delete i->second.pStkIDMapHash;
			if(i->second.pFullData)
				free(i->second.pFullData);
		}
		m_mapStructForBmData.clear();
	}
}

int DidUncompress::ReadConfig()
{
#ifdef _MSC_VER
	STEP::DOMDocumentPtr pDoc = STEP::DOMDocumentPtr(new STEP::MSXML_DOMDocument());
#else
	STEP::DOMDocumentPtr pDoc = STEP::DOMDocumentPtr(new STEP::LIBXML_DOMDocument());
#endif
	if (!pDoc->load(configFilePath))
		return -1;
	STEP::DOMNodePtr pNode = pDoc->getNode("/DidStruct");
	if (!pNode.get()) return -1;
	STEP::DOMNodePtr pDID = pNode->getFirstChildNode();
	while (pDID.get())
	{
		if(pDID->getName() == "did")
		{
			ReadDidStruct(pDID.get());
		}
		RESET_AUTO_PTR(pDID, pDID->getNextSiblingNode());
	}
	return 1;
}

void DidUncompress::ReadDidStruct(STEP::DOMNode* node)
{
	STEP::DOMAttributesPtr attr = node->getAttributes();
	//string did;
	string did_list;
	string file;
	string whole;
	string compress;

	//if (attr->get("id", did) && attr->get("file", file))
	if (attr->get("id", did_list) && attr->get("file", file))
	{
		TDidInfo did_info;
		//did_info.did = atoi(did.c_str());
		did_info.strFilePath = file;

		if (attr->get("whole", whole))
			did_info.bFull = (whole == "0" ? false : true);
		else
			did_info.bFull = true;

		if (attr->get("compress", compress))
			did_info.bCompress = (compress == "0" ? false : true);
		else
			did_info.bCompress = true;

		if (!did_info.bFull && !did_info.bCompress)
		{
			//Config error, not support whole=0 and compress=0, use default 1
			did_info.bFull = true;
			did_info.bCompress = true;
		}
		//m_mapDidFile.insert(make_pair<DWORD, TDidInfo>(did_info.did, did_info));

		CSplitString split(did_list.c_str(), ',');
		char* pDid = split.Next();
		while (NULL != pDid)
		{
			if (*pDid != '\0')
			{
				CSplitString split2(pDid, '-');
				char* pStart = split2.Next();
				char* pEnd = split2.Next();
				if (NULL != pStart)
				{
					int iStart = atoi(pStart);
					int iEnd = (NULL != pEnd ? atoi(pEnd) : iStart);
					while (iStart <= iEnd)
					{
						did_info.did = iStart++;
						m_mapDidFile.insert(make_pair<DWORD, TDidInfo>(did_info.did, did_info));
					}
				}
			}
			pDid = split.Next();
		}
	}
}

void DidUncompress::Initialize()
{
	char* pFileContent = NULL;
	int iFileLen = 0;

	map<int, TDidInfo>::iterator it = m_mapDidFile.begin();
	for (; it != m_mapDidFile.end(); ++it)
	{
		if(!it->second.pDidStruct)
			it->second.pDidStruct = CDidStructApi::CreateDidStructObj();

		if(!it->second.pUnCompress)
			it->second.pUnCompress = CDsCompressApi::CreateCompressObj();

		if(!it->second.pDidStruct||!it->second.pUnCompress)
		{
			if(it->second.pDidStruct) 
			{
				CDidStructApi::DestoryDidStructObj(it->second.pDidStruct);
				it->second.pDidStruct = NULL;
			}
			else if(it->second.pUnCompress)
			{
				CDsCompressApi::DestroyCompressObj(it->second.pUnCompress);
				it->second.pUnCompress = NULL;
			}
			continue;
		}

		if (NULL != pFileContent)
		{
			free(pFileContent);
			pFileContent = NULL;
		}
		iFileLen = 0;
		pFileContent = GetFileContent(it->second.strFilePath.c_str(), iFileLen);
		if (NULL == pFileContent || iFileLen <= 0)
		{
			CDidStructApi::DestoryDidStructObj(it->second.pDidStruct);
			it->second.pDidStruct = NULL;
			CDsCompressApi::DestroyCompressObj(it->second.pUnCompress);
			it->second.pUnCompress = NULL;
			if (NULL != pFileContent)
			{
				free(pFileContent);
				pFileContent = NULL;
			}
			iFileLen = 0;
			continue;
		}

		if(it->second.pDidStruct->LoadMemXmlBuffer(pFileContent, iFileLen) < 0)
		{
			CDidStructApi::DestoryDidStructObj(it->second.pDidStruct);
			it->second.pDidStruct = NULL;
			CDsCompressApi::DestroyCompressObj(it->second.pUnCompress);
			it->second.pUnCompress = NULL;
			free(pFileContent);
			pFileContent = NULL;
			iFileLen = 0;
			continue;
		}

		int cid = 1;
		int lastcmp = 0;
		if(!it->second.bFull)
		{
			TStructForGetBmData* pCompressUserArg = &m_mapStructForBmData[it->first];
			pCompressUserArg->iBufferSize = it->second.pDidStruct->mDidInfo.m_dwRRecSize * MaxStocksCount;
			pCompressUserArg->pFullData = (char*)malloc(pCompressUserArg->iBufferSize);
			pCompressUserArg->iTotalStk = 0;
			pCompressUserArg->pStkIDMapHash = new STKIDMAP;

			if(!pCompressUserArg->pFullData||!pCompressUserArg->pStkIDMapHash)
			{
				if(pCompressUserArg->pFullData)free(pCompressUserArg->pFullData);
				else if(pCompressUserArg->pStkIDMapHash)delete(pCompressUserArg->pStkIDMapHash);
				m_mapStructForBmData.erase(it->first);
				CDidStructApi::DestoryDidStructObj(it->second.pDidStruct);
				it->second.pDidStruct = NULL;
				CDsCompressApi::DestroyCompressObj(it->second.pUnCompress);
				it->second.pUnCompress = NULL;
				free(pFileContent);
				pFileContent = NULL;
				iFileLen = 0;

				continue;
			}

			cid = 2;
			lastcmp = 1;
			if(!it->second.pUnCompress->Initialize(	it->second.pDidStruct,
													it->first,
													cid,
													lastcmp,
													0,
													GetBmData,
													SetBmData,
													(char*)pCompressUserArg))
			{
				CDidStructApi::DestoryDidStructObj(it->second.pDidStruct);
				it->second.pDidStruct = NULL;
				CDsCompressApi::DestroyCompressObj(it->second.pUnCompress);
				it->second.pUnCompress = NULL;
				free(pFileContent);
				pFileContent = NULL;
				iFileLen = 0;
				free(pCompressUserArg->pFullData);
				delete(pCompressUserArg->pStkIDMapHash);
				m_mapStructForBmData.erase(it->first);

				continue;
			}
		}
		else
		{
			if(!it->second.pUnCompress->Initialize(	it->second.pDidStruct,
													it->first,
													cid,
													lastcmp))
			{
				CDidStructApi::DestoryDidStructObj(it->second.pDidStruct);
				it->second.pDidStruct = NULL;
				CDsCompressApi::DestroyCompressObj(it->second.pUnCompress);
				it->second.pUnCompress = NULL;
				free(pFileContent);
				pFileContent = NULL;
				iFileLen = 0;
				continue;
			}
		}
		it->second.bInitialized = true;
	}
}
/*
int DidUncompress::Uncompress(char* source_data, int source_size, int num, int did, char* des_data, int des_size )
{
	if(!source_data || source_size <= 0 || num <= 0 || !des_data || des_size <= 0)
		return -1;

	if(!m_mapDidFile.count(did))
		return -1;

	map<int, TDidInfo>::iterator it = m_mapDidFile.find(did);
	if(!it->second.bInitialized)
		return -1;

	char* input_buff = (char*)malloc(source_size);
	if(!input_buff) return -1;
	memcpy(input_buff,source_data,source_size);

	it->second.pUnCompress->SetUnCompressOutputBuffer(des_data,des_size);

	if(it->second.pUnCompress->UnCompressData(input_buff,source_size,num) == -1)
		return -1;

	//des_size = it->second.pUnCompress->FinishUnCompressedData();
	if(it->second.pUnCompress->FinishUnCompressedData() != des_size)
		return -1;

	free(input_buff);

	return 1;

}
*/

int DidUncompress::Compress(char* source_data, int source_size, int num, int did, char* des_data, int& des_size)
{
	if(!source_data || source_size <= 0 || num <= 0 || !des_data || des_size <= 0)
		return -1;

	if(!m_mapDidFile.count(did))
		return -1;

	map<int, TDidInfo>::iterator it = m_mapDidFile.find(did);
	if(!it->second.bInitialized)
		return -1;

	char* input_buff = (char*)malloc(source_size);
	if(!input_buff) return -1;
	memcpy(input_buff,source_data,source_size);

	it->second.pUnCompress->SetCompressOutputBuffer(des_data,des_size);

	if(it->second.pUnCompress->CompressData(input_buff,source_size,num) == -1)
		return -1;

	des_size = it->second.pUnCompress->FinishCompressedData();

	free(input_buff);

	return 1;
}


int DidUncompress::DisassemblePack(DC_HEAD* pPack,DataBuffer& buf)
{
	if(!pPack)return -1;

	DC_HEAD* pHead = (DC_HEAD*)pPack;
	DataBuffer t;
	if (pPack->m_cTag == DC_TAG && (pPack->m_wAttrib & DC_CPS_MASK) == DC_ZLIB_CPS)
	{
		ZLIB_HEAD* pZlibHead = (ZLIB_HEAD*)(pPack+1);
		if(t.Allocate(pZlibHead->m_dwSourceLen + sizeof(DC_HEAD) == -1))
			return -1;
		memcpy(t.GetData(),pPack,sizeof(DC_HEAD));
		int iDestLen = pZlibHead->m_dwSourceLen;
		char* pSrcData = (char*)(pZlibHead+1);
		uncompress(	(Bytef*)(t.GetData() + sizeof(DC_HEAD)), 
					(uLongf*)&iDestLen, 
					(Bytef*)pSrcData, 
					pPack->m_nLen-sizeof(ZLIB_HEAD));
		if(iDestLen != pZlibHead->m_dwSourceLen)
			return -1;
		pHead = (DC_HEAD*)t.GetData();
		pHead->m_nLen = pZlibHead->m_dwSourceLen;
	}

	if(pPack->m_cType == DCT_DID)
	{
		DC_DIDHead* pDidHead = (DC_DIDHead*)(pHead+1);
		int iDidHeadLen = pDidHead->GetLen();
		int len = 0;
		if (pPack->m_cTag == DC_TAG && (pPack->m_wAttrib & DC_CPS_MASK) == DC_NOCOMPRESS)
		{
			len = pHead->m_nLen - iDidHeadLen;
			if(buf.Allocate(len)!= -1)
			{
				memcpy(buf.GetData(),(char*)pDidHead + iDidHeadLen,len);
				return 1;
			}
			else
				return -1;
		}

		if(pPack->m_cTag == DC_TAG && (pPack->m_wAttrib & DC_CPS_MASK) == DC_XMLCID_CPS)
		{
			int did = pDidHead->GetDid();
			int num = pDidHead->GetRecNum();
			//pDidHead->
			DC_DIDCompress* pDidCompress = (DC_DIDCompress*)((char*)pDidHead + iDidHeadLen);
			len = pDidCompress->GetUnComLen();
			int iCompressHeadLen = pDidCompress->GetLen();
			int iCompressLen = pHead->m_nLen - iDidHeadLen - iCompressHeadLen;
			char* pData = (char*)pDidCompress+iCompressHeadLen;

			if(!m_mapDidFile.count(did))
				return -1;

			map<int, TDidInfo>::iterator it = m_mapDidFile.find(did);
			if(!it->second.bInitialized)
				return -1;

			if(pDidHead->m_bFullRec == 1 && !it->second.bFull)
			{
				CDsCompressApi* puc = CDsCompressApi::CreateCompressObj();
				if(!puc) return -1;
				if(!puc->Initialize(	it->second.pDidStruct,
										did,
										pDidCompress->m_bCid,
										0
									)
				)
				{
					CDsCompressApi::DestroyCompressObj(puc);
					return -1;
				}

				if(buf.Allocate(len)!= -1)
				{
					puc->SetUnCompressOutputBuffer(buf.GetData(),len);
					if(puc->UnCompressData(pData,iCompressLen,num) == -1)
						return -1;
					if(puc->FinishUnCompressedData() != len)
						return -1;

					TStructForGetBmData* pObj = &m_mapStructForBmData[did];
					if(!pObj->pStkIDMapHash->empty())pObj->pStkIDMapHash->clear();
					memset(pObj->pFullData,0,pObj->iBufferSize);
					pObj->iTotalStk = 0;

					int iRecSize = it->second.pDidStruct->mDidInfo.m_dwRRecSize;
					char* record = buf.GetData();

					for(int i = 0;i < num;i++)
					{
						unsigned int stk_id = *((unsigned int*)record);
						if(!pObj->pStkIDMapHash->count(stk_id))
						{
							if (pObj->iTotalStk >= pObj->iBufferSize/iRecSize)
								return -1;
							(*(pObj->pStkIDMapHash))[stk_id] = pObj->iTotalStk;
							pObj->iTotalStk++;
							char* pPos = pObj->pFullData + ((pObj->iTotalStk-1) * iRecSize);

							//copy data to full data buffer
							memcpy(pPos, record, iRecSize);
						}
						else
						{
							int iPos = iRecSize * (*(pObj->pStkIDMapHash))[stk_id];

							if (iPos + iRecSize > pObj->iBufferSize)
								return -1;

							char* pPos = pObj->pFullData;
							pPos += iPos;

							if (*(unsigned int*)pPos != stk_id)
								return -1;

							//copy data to full data buffer
							memcpy(pPos, record, iRecSize);
						}
						record += iRecSize;
					}
					return 1;
				}
				else
					return -1;
			}
			else
			{
				if(buf.Allocate(len)!= -1)
				{
					it->second.pUnCompress->SetUnCompressOutputBuffer(buf.GetData(),len);
					if(it->second.pUnCompress->UnCompressData(pData,iCompressLen,num) == -1)
						return -1;
					if(it->second.pUnCompress->FinishUnCompressedData() != len)
						return -1;
					return 1;
				}
				else
					return -1;
			}
		}
	}
	else if (pPack->m_cType == DCT_DSDATA)
	{
/*
		DataBuffer t;
		if (pPack->m_cTag == DC_TAG && (pPack->m_wAttrib & DC_CPS_MASK) == DC_ZLIB_CPS)
		{
			 ZLIB_HEAD* pZlibHead = (ZLIB_HEAD*)(pPack+1);
			 if(t.Allocate(pZlibHead->m_dwSourceLen + sizeof(DC_HEAD) == -1))
				 return -1;
			 memcpy(t.GetData(),pPack,sizeof(DC_HEAD));
			 int iDestLen = pZlibHead->m_dwSourceLen;
			 char* pSrcData = (char*)(pZlibHead+1);
			 uncompress((Bytef*)(t.GetData() + sizeof(DC_HEAD)), 
						(uLongf*)&iDestLen, 
						(Bytef*)pSrcData, 
						pPack->m_nLen-sizeof(ZLIB_HEAD));
			 if(iDestLen != pZlibHead->m_dwSourceLen)
				 return -1;
			 pHead = (DC_HEAD*)t.GetData();
			 pHead->m_nLen = pZlibHead->m_dwSourceLen;
		}
*/

		DC_DSDATA* pDsData = (DC_DSDATA*)(pHead+1);
		int iDataLen =	pHead->m_nLen + sizeof(DC_DSLEN) - sizeof(DC_DSDATA) -
						pDsData->m_dwDidNum * sizeof(DC_DSLEN);

		if (pDsData->m_dwDidNum > 0)
		{
			DC_DSLEN* pDsLen = &(pDsData->mDsLen[0]);
			if (iDataLen < pDsLen->m_dwLen)
				return -1;
			char* pDidTemplateData = (char*)pDsLen + (pDsData->m_dwDidNum * sizeof(DC_DSLEN));
			for (int i = 0; i < pDsData->m_dwDidNum; i++)
			{
				if (iDataLen < pDsLen->m_dwLen)
					return -1;
				map<int, TDidInfo>::iterator it1 = m_mapDidFile.find(pDsLen->m_dwDid);
				if (it1 != m_mapDidFile.end() && it1->second.bNeedSynaDidStructFile)
				{
					CDidStructApi* pDidStruct = CDidStructApi::CreateDidStructObj();
					if (NULL == pDidStruct)
						return -1;
					if (pDidStruct->LoadMemXmlBuffer(pDidTemplateData, pDsLen->m_dwLen) < 0)
					{
						CDidStructApi::DestoryDidStructObj(pDidStruct);
						return -1;
					}

					char sFilePath[256] = {0};
					sprintf(sFilePath, "%u.xml\0", pDidStruct->mDidInfo.did);
					vector<TDidInfo*> vecDidFile;

					//一个模板文件可能同时支持多个did
					for (int i = 0; i < pDidStruct->m_pDids->m_wDidsNum; i++)
					{
						int did = pDidStruct->m_pDids->mDidName[i].m_dwDid;
						map<int, TDidInfo>::iterator it2 = m_mapDidFile.find(did);
						if (it2 != m_mapDidFile.end())
						{
							//删除旧文件
							remove(it2->second.strFilePath.c_str());
							vecDidFile.push_back(&it2->second);
						}
					}
					//写入新文件
					FILE* fp = fopen(sFilePath, "wb");
					if (NULL == fp)
					{
						CDidStructApi::DestoryDidStructObj(pDidStruct);
						return -1;
					}
					fwrite(pDidTemplateData, pDsLen->m_dwLen, 1, fp);
					fclose(fp);
					fp = NULL;
					for (int i = 0; i < vecDidFile.size(); i++)
					{
						TDidInfo* pDidFile = vecDidFile[i];
						pDidFile->bNeedSynaDidStructFile= false;
						pDidFile->strFilePath= sFilePath;
						if(UpdateDSData(pDidFile,pDidTemplateData,pDsLen->m_dwLen) == -1)
						{
							CDidStructApi::DestoryDidStructObj(pDidStruct);
							return -1;
						}
					}
					CDidStructApi::DestoryDidStructObj(pDidStruct);
					pDidStruct = NULL;
				}
				iDataLen -= pDsLen->m_dwLen;
				pDidTemplateData += pDsLen->m_dwLen;
				pDsLen++;
			}
		}
		return 1;
	}
	else if(pPack->m_cType == DCT_DSCRC)
	{
		DC_DSCRC* pDSCrc = (DC_DSCRC*)(pHead + 1);
		int iLen = pHead ->m_nLen;
		iLen -= sizeof(DC_DSCRC);
		for (int i = 0; i <= iLen; i += sizeof(DC_DSCRC))
		{
			map<int, TDidInfo>::iterator it = m_mapDidFile.find(pDSCrc->m_dwDid);
			if (it != m_mapDidFile.end())
			{
				it->second.bServerSupport = true;
				if (it->second.pDidStruct->m_dwCrc == pDSCrc->m_dwCrc)
					it->second.bNeedSynaDidStructFile = false;
				else
					it->second.bNeedSynaDidStructFile = true;
			}
			pDSCrc++;
		}
		return 1;
	}
	return -1;
}

int DidUncompress::UpdateDSData(TDidInfo* pDidFile,char* pTemplateData,int len)
{
	if(!pDidFile || !pTemplateData || (len <= 0))
		return -1;
	if(pDidFile->bInitialized)
	{
		CDidStructApi::DestoryDidStructObj(pDidFile->pDidStruct);
		pDidFile->pDidStruct = NULL;
		CDsCompressApi::DestroyCompressObj(pDidFile->pUnCompress);
		pDidFile->pUnCompress = NULL;

		if(!pDidFile->bFull)
		{
			TStructForGetBmData* pCompressUserArg = &m_mapStructForBmData[pDidFile->did];
			free(pCompressUserArg->pFullData);
			delete(pCompressUserArg->pStkIDMapHash);
			m_mapStructForBmData.erase(pDidFile->did);
		}

		pDidFile->bInitialized = false;
	}

	pDidFile->pDidStruct = CDidStructApi::CreateDidStructObj();
	pDidFile->pUnCompress = CDsCompressApi::CreateCompressObj();

	if(!pDidFile->pDidStruct||!pDidFile->pUnCompress)
	{
		if(pDidFile->pDidStruct) 
		{
			CDidStructApi::DestoryDidStructObj(pDidFile->pDidStruct);
			pDidFile->pDidStruct = NULL;
		}
		else if(pDidFile->pUnCompress)
		{
			CDsCompressApi::DestroyCompressObj(pDidFile->pUnCompress);
			pDidFile->pUnCompress = NULL;
		}
		return -1;
	}

	if(pDidFile->pDidStruct->LoadMemXmlBuffer(pTemplateData, len) < 0)
	{
		CDidStructApi::DestoryDidStructObj(pDidFile->pDidStruct);
		pDidFile->pDidStruct = NULL;
		CDsCompressApi::DestroyCompressObj(pDidFile->pUnCompress);
		pDidFile->pUnCompress = NULL;
		return -1;
	}

	int cid = 1;
	int lastcmp = 0;
	if(!pDidFile->bFull)
	{
		TStructForGetBmData* pCompressUserArg = &m_mapStructForBmData[pDidFile->did];
		pCompressUserArg->iBufferSize = pDidFile->pDidStruct->mDidInfo.m_dwRRecSize * MaxStocksCount;
		pCompressUserArg->pFullData = (char*)malloc(pCompressUserArg->iBufferSize);
		pCompressUserArg->iTotalStk = 0;
		pCompressUserArg->pStkIDMapHash = new STKIDMAP;

		if(!pCompressUserArg->pFullData||!pCompressUserArg->pStkIDMapHash)
		{
			if(pCompressUserArg->pFullData)free(pCompressUserArg->pFullData);
			else if(pCompressUserArg->pStkIDMapHash)delete(pCompressUserArg->pStkIDMapHash);
			m_mapStructForBmData.erase(pDidFile->did);
			CDidStructApi::DestoryDidStructObj(pDidFile->pDidStruct);
			pDidFile->pDidStruct = NULL;
			CDsCompressApi::DestroyCompressObj(pDidFile->pUnCompress);
			pDidFile->pUnCompress = NULL;
			return -1;
		}

		cid = 2;
		lastcmp = 1;
		if(!pDidFile->pUnCompress->Initialize(	pDidFile->pDidStruct,
												pDidFile->did,
												cid,
												lastcmp,
												0,
												GetBmData,
												SetBmData,
												(char*)pCompressUserArg))
		{
			CDidStructApi::DestoryDidStructObj(pDidFile->pDidStruct);
			pDidFile->pDidStruct = NULL;
			CDsCompressApi::DestroyCompressObj(pDidFile->pUnCompress);
			pDidFile->pUnCompress = NULL;
			free(pCompressUserArg->pFullData);
			delete(pCompressUserArg->pStkIDMapHash);
			m_mapStructForBmData.erase(pDidFile->did);
			return -1;
		}
	}
	else
	{
		if(!pDidFile->pUnCompress->Initialize(	pDidFile->pDidStruct,
												pDidFile->did,
												cid,
												lastcmp))
		{
			CDidStructApi::DestoryDidStructObj(pDidFile->pDidStruct);
			pDidFile->pDidStruct = NULL;
			CDsCompressApi::DestroyCompressObj(pDidFile->pUnCompress);
			pDidFile->pUnCompress = NULL;
			return -1;
		}
	}
	pDidFile->bInitialized = true;
	return 1;
}
