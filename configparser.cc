#include <assert.h>
#include <iostream>
#include "configparser.h"
#include "flags.h"

using namespace std;
using namespace log4cxx;


LoggerPtr ConfigParser::logger_(Logger::getLogger("config_parser"));

std::string ConfigParser::SplitDidTemplateId(std::string &did_template_path)
{
	int pre_pos;
#ifndef __linux
	pre_pos = did_template_path.find_last_of("/\\");
#elif __linux
	pre_pos = did_template_path.find_last_of("/");
#endif
	int last_pos = did_template_path.find_last_not_of(".xml");
	std::string did_template_id = did_template_path.substr(pre_pos+1 , last_pos-pre_pos);
	//cout<<did_template_id<<endl;
	LOG4CXX_INFO(logger_, "did template id:" << did_template_id);
	assert(!did_template_id.empty());
	return did_template_id;
}

void ConfigParser::Parse()
{
	xml2cfg config;
	deque<XML_ZMQ> *log_zmqdeque = xml_log_.get_zmqdeque();
	deque<XML_ZMQ> *business_errinfo_zmqdeque = xml_business_errinfo_.get_zmqdeque();
	config.open(CONFIG_FILE);
	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"listening");

	xmlNodeSetPtr log_nodeset = config.locate(NULL,"/listening/log");
	for(int i=0;i<log_nodeset->nodeNr;i++)
	{
		xmlNodePtr log_node = config.get_node(log_nodeset,i);
		xmlNodeSetPtr log_zmq_nodeset = config.locate(log_node,"zmqs/zmq");
		for(int i=0;i<log_zmq_nodeset->nodeNr;i++)
		{
			xmlNodePtr log_zmq_node = config.get_node(log_zmq_nodeset,i);
			std::string zmqpattern;
			std::string zmqsocketaction;
			std::string zmqsocketaddr;
			config.get_prop_string(log_zmq_node,"zmqpattern",zmqpattern,NULL);
			config.get_prop_string(log_zmq_node,"zmqsocketaction",zmqsocketaction,NULL);
			config.get_prop_string(log_zmq_node,"zmqsocketaddr",zmqsocketaddr,NULL);
			XML_ZMQ xml_zmq(zmqpattern,zmqsocketaction,zmqsocketaddr);
			log_zmqdeque->push_back(xml_zmq);
		}
	}
	//business error info
	xmlNodeSetPtr business_errinfo_nodeset = config.locate(NULL,"/listening/business_error_info");
	for(int i=0;i<business_errinfo_nodeset->nodeNr;i++)
	{
		xmlNodePtr business_errinfo_node = config.get_node(business_errinfo_nodeset,i);
		xmlNodeSetPtr business_errinfo_zmq_nodeset = config.locate(business_errinfo_node,"zmqs/zmq");
		for(int i=0;i<business_errinfo_zmq_nodeset->nodeNr;i++)
		{
			xmlNodePtr business_errinfo_zmq_node = config.get_node(business_errinfo_zmq_nodeset,i);
			std::string zmqpattern;
			std::string zmqsocketaction;
			std::string zmqsocketaddr;
			config.get_prop_string(business_errinfo_zmq_node,"zmqpattern",zmqpattern,NULL);
			config.get_prop_string(business_errinfo_zmq_node,"zmqsocketaction",zmqsocketaction,NULL);
			config.get_prop_string(business_errinfo_zmq_node,"zmqsocketaddr",zmqsocketaddr,NULL);
			XML_ZMQ xml_zmq(zmqpattern,zmqsocketaction,zmqsocketaddr);
			business_errinfo_zmqdeque->push_back(xml_zmq);
		}
	}

	xmlNodeSetPtr listeningitem_nodeset = config.locate(root_node,"//listeningitem");
	//cout<<listeningitem_nodeset->nodeNr<<endl;
	LOG4CXX_INFO(logger_, "listening connection num is " << listeningitem_nodeset->nodeNr );
	for(int i=0; i<listeningitem_nodeset->nodeNr; i++)
	{
		xmlNodePtr listeningitem_node =config.get_node(listeningitem_nodeset,i);
		std::string net_adapter;
		config.get_prop_string(listeningitem_node, "net_adapter", net_adapter, NULL);
		int port = config.get_prop_int(listeningitem_node,"port",0);
		ports_.push_back(port);
		std::string filter;
		config.get_prop_string(listeningitem_node,"filter",filter,NULL);
		//cout<<filter<<endl;
		LOG4CXX_INFO(logger_, "filter:" << filter);
		XML_ListeningItem listening_item(net_adapter, port, filter);

		xmlNodeSetPtr didtemplate_nodeset = config.locate(listeningitem_node,"did_templates/did_template");
		if(didtemplate_nodeset->nodeNr > 0)
		{
			for(int i=0;i<didtemplate_nodeset->nodeNr;i++)
			{
				xmlNodePtr didtemplate_node = config.get_node(didtemplate_nodeset,i);
				std::string did_template_path;
				config.get_prop_string(didtemplate_node,"did_template_path",did_template_path,NULL);
				listening_item.push_did_template_paths(did_template_path);
				std::string did_template_id = SplitDidTemplateId(did_template_path);
				listening_item.push_did_template_ids(did_template_id);
				listening_item.insert_did_filepath_map(std::pair<int, std::string>(atoi(did_template_id.c_str()), did_template_path));
			}
		}

		xmlNodeSetPtr thread_nodeset = config.locate(listeningitem_node,"thread");
		//cout<<"thread_nodeset:"<<thread_nodeset->nodeNr<<endl;
		LOG4CXX_INFO(logger_, "thread_nodeset:" << thread_nodeset->nodeNr);
		for(int i=0;i<thread_nodeset->nodeNr;i++)
		{
			xmlNodePtr thread_node = config.get_node(thread_nodeset,i);
			std::string threadclass;
			config.get_prop_string(thread_node,"threadclass",threadclass,NULL);
			//cout<<threadclass<<endl;
			LOG4CXX_INFO(logger_, "threadclass:" << threadclass);
			deque<XML_ZMQ> *zmqdeque;
			if("CaptureNetPacket" == threadclass)
			{
				zmqdeque = listening_item.get_capture_net_packet()->get_zmqdeque();
			}
			else if("ShuntNetPacket" == threadclass)
			{
				zmqdeque = listening_item.get_shunt_net_packet()->get_zmqdeque();
			}
			else if("CombineDCPacket" == threadclass)
			{
				zmqdeque = listening_item.get_combine_dc_packet()->get_zmqdeque();
			}
			else if("UncompressDCPacket" == threadclass)
			{
				zmqdeque = listening_item.get_uncompress_dc_packet()->get_zmqdeque();
			}
			else if("LuaRoutine" == threadclass)
			{
				zmqdeque = listening_item.get_lua_routine()->get_zmqdeque();
			}

			xmlNodeSetPtr zmq_nodeset = config.locate(thread_node,"zmqs/zmq");
			//cout<<"zmqnodeset:"<<zmq_nodeset->nodeNr<<endl;
			LOG4CXX_INFO(logger_, "zmqnodeset:" << zmq_nodeset->nodeNr);	
			for(int i=0;i<zmq_nodeset->nodeNr;i++)
			{
				xmlNodePtr zmq_node = config.get_node(zmq_nodeset,i);
				std::string zmqpattern;
				std::string zmqsocketaction;
				std::string zmqsocketaddr;
				config.get_prop_string(zmq_node,"zmqpattern",zmqpattern,NULL);
				config.get_prop_string(zmq_node,"zmqsocketaction",zmqsocketaction,NULL);
				config.get_prop_string(zmq_node,"zmqsocketaddr",zmqsocketaddr,NULL);
				XML_ZMQ zmq_item(zmqpattern,zmqsocketaction,zmqsocketaddr);
				zmqdeque->push_back(zmq_item);
			}
		}
		listeningitem_deque_.push_back(listening_item);
	}
}
