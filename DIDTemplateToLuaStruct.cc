#include "DIDTemplateToLuaStruct.h"
#include "xml_class_set.h"
#include "xml2cfg.h"
#ifndef __linux
#include <io.h>
#else 
#include <sys/io.h>
#endif

#include <iostream>

typedef std::map< std::string, int > map_typesize_t;
map_typesize_t did_type_map;

int cat_to( char *dest, const char *val, int pos )
{
	int _len = strlen( val );
	strcat( dest + pos, val );
	return pos + _len;
}

//void DIDTemplateToLuaStruct::AutoAddToDidTypeMap()
//{
//	did_type_map.insert( std::make_pair("repeat", 4) ); 
//	did_type_map.insert( std::make_pair("char", 1) ); 
//	did_type_map.insert( std::make_pair("int16", 2) ); 
//	did_type_map.insert( std::make_pair("int32", 4) ); 
//	did_type_map.insert( std::make_pair("int64", 8) ); 
//	did_type_map.insert( std::make_pair("byte", 1) ); 
//	did_type_map.insert( std::make_pair("uint16", 2) ); 
//	did_type_map.insert( std::make_pair("uint24", 3) ); 
//	did_type_map.insert( std::make_pair("uint32", 4) ); 
//	did_type_map.insert( std::make_pair("uint64", 8) ); 
//	did_type_map.insert( std::make_pair("pfloat", 8) ); 
//	did_type_map.insert( std::make_pair("dzh_time_t", 8) ); 
//	did_type_map.insert( std::make_pair("vfloat", 8) );
//}

//vector<std::string> & DIDTemplateToLuaStruct::GetPathFiles()
//{
//	_finddata_t file;
//	long lfile;
//	std::string files = did_template_file_path_ + "\*.*";
//	if((lfile = _findfirst(files.c_str(),&file)) == -1l)
//	{
//		cout<<"can't find the did template files"<<endl;
//	}
//	else
//	{
//		while(_findnext(lfile,&file) == 0)
//		{
//			std::string filename = file.name;
//			filename = did_template_file_path_ + "\\" +  filename;
//			did_template_files_.push_back(filename);
//		}
//	}
//	return did_template_files_;
//}

void DIDTemplateToLuaStruct::Transform(XML_ListeningItem &listening_item)
{
	xml2cfg config;
	vector<std::string> & files = listening_item.get_did_template_paths();
	for(vector<std::string>::iterator iter = files.begin();iter != files.end();iter++)
	{
		config.open((*iter).c_str());

		char _lua_cdef[2048] = {0};
		char _name[128]  = {0};
		char _cname[128] = {0};
		xmlNodeSetPtr did_nodeset = config.locate(NULL,"/datas/data/dids/did");
		xmlNodePtr did_node = config.get_node(did_nodeset,0);
		std::string lua_file_name_pre;
		config.get_prop_string(did_node,"id",lua_file_name_pre,NULL);
		config.get_prop_string(did_node,"name",_name,NULL);
		config.get_prop_string(did_node,"des",_cname,NULL);

		int _pos = 0;
		_pos = cat_to( _lua_cdef, "local ffi = require(\"ffi\")\nffi.cdef[[\n#pragma pack(1)\ntypedef struct ", _pos );
		std::string _head = std::string(_name) + "//" + _cname + "\n{\n";
		_pos = cat_to( _lua_cdef, _head.c_str(), _pos );
		
		xmlNodeSetPtr field_nodeset = config.locate(NULL,"/datas/data/fields/*");
		for(int i=0;i<field_nodeset->nodeNr;i++)
		{
			xmlNodePtr  field_node= config.get_node(field_nodeset,i);
			std::string type;
			std::string name;
			std::string commet;
			type = std::string((const char *)field_node->name);
			config.get_prop_string(field_node,"name",name,NULL);
			config.get_prop_string(field_node,"des",commet,NULL);

			std::string _line;
			if("BuyCount" == name || "SellCount" == name)
			{
				std::string arraysize;
				config.get_prop_string(field_node,"arraysize",arraysize,"10");
				_line = type +
					"\t"+
					name+
					"["+
					arraysize+
					"];//"+
					commet+
					"\n";
			}
			else
			{
				_line =  type +
					"\t"+
					name+
					";//"+
					commet+
					"\n";
			}

			_pos = cat_to( _lua_cdef, _line.c_str(), _pos );
		}
		_pos = cat_to( _lua_cdef, "}", _pos );
		_pos = cat_to(_lua_cdef,_name,_pos);
		_pos = cat_to(_lua_cdef,";\n#pragma pack()\n]]\n",_pos);
		char lua_file[64] = {0};
		sprintf(lua_file,"%s.lua",lua_file_name_pre.c_str());
		FILE *_fp = fopen( lua_file, "wb" );
		if( _fp	 != NULL )
		{
			fwrite( _lua_cdef, 1, _pos, _fp );
			fclose( _fp );
		}

	}
}
