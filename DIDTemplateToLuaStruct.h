#ifndef MONITOR_DID_TEMPLATE_TO_LUA_STRUCT_H
#define MONITOR_DID_TEMPLATE_TO_LUA_STRUCT_H

#include <map>
#include <iostream>
#include "xml_class_set.h"

using namespace std;

class DIDTemplateToLuaStruct
{
public:
	DIDTemplateToLuaStruct(){};
	~DIDTemplateToLuaStruct(){};
	void Transform(XML_ListeningItem & listening_item);
	//void AutoAddToDidTypeMap();
	//vector<std::string>& GetPathFiles();
	
private:
	vector<std::string> did_template_files_;
};

#endif