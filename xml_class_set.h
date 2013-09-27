#ifndef MONITOR_XML_CLASS_SET_H
#define MONITOR_XML_CLASS_SET_H
#include <zmq.h>
#include <deque>
#include <map>
#include <vector>
#include <string>
using namespace std;

class XML_ZMQ
{
public:
	XML_ZMQ(std::string zmqpattern,std::string zmqsocketaction,std::string zmqsocketaddr):zmqpattern_(zmqpattern),\
																																	zmqsocketaction_(zmqsocketaction),\
																																	zmqsocketaddr_(zmqsocketaddr){};
	~XML_ZMQ(){};
	inline int get_zmqpattern()
	{
		if("PAIR" == zmqpattern_)
			return ZMQ_PAIR;
		else if("PUB" == zmqpattern_)
			return ZMQ_PUB;
		else if("SUB" == zmqpattern_)
			return ZMQ_SUB;
		else if("REQ" == zmqpattern_)
			return ZMQ_REQ;
		else if("REP" == zmqpattern_)
			return ZMQ_REP;
		else if("DEALER" == zmqpattern_)
			return ZMQ_DEALER;
		else if("ROUTER" == zmqpattern_)
			return ZMQ_ROUTER;
		else if("PULL" == zmqpattern_)
			return ZMQ_PULL;
		else if("PUSH" == zmqpattern_)
			return ZMQ_PUSH;
		else if("XPUB" == zmqpattern_)
			return ZMQ_XPUB;
		else if("XSUB" == zmqpattern_)
			return ZMQ_XSUB;
	}
	inline std::string& get_zmqsocketaction()
	{
		return zmqsocketaction_;
	}
	inline std::string& get_zmqsocketaddr()
	{
		return zmqsocketaddr_;
	}
private:
	std::string zmqpattern_;
	std::string zmqsocketaction_;
	std::string zmqsocketaddr_;
};

class XML_Capture
{
public:
	XML_Capture()
	{
	}
	~XML_Capture()
	{
	}
    inline deque<XML_ZMQ>* get_zmqdeque()
	{
		return &zmqdeque_;
	}
private:
	deque<XML_ZMQ> zmqdeque_;
};

class XML_CaptureNetPacket
{
public:
	XML_CaptureNetPacket(){};
	~XML_CaptureNetPacket(){};
	inline deque<XML_ZMQ> * get_zmqdeque()
	{
		return &zmqdeque_;
	}
private:
	deque<XML_ZMQ> zmqdeque_;
};

class XML_ShuntNetPacket
{
public:
	XML_ShuntNetPacket(){};
	~XML_ShuntNetPacket(){};
	inline deque<XML_ZMQ> * get_zmqdeque()
	{
		return &zmqdeque_;
	}
private:
	deque<XML_ZMQ> zmqdeque_;

};

// class XML_Parse
// {
// public:
// 	XML_Parse()
// 	{
		
// 	}
// 	~XML_Parse()
// 	{
		
// 	}
// 	inline deque<XML_ZMQ>* get_zmqdeque()
// 	{
// 		return &zmqdeque_;
// 	}
// private:
// 	deque<XML_ZMQ> zmqdeque_;
// };

class XML_UncompressDCPacket
{
public:
	XML_UncompressDCPacket()
	{
		
	}
	~XML_UncompressDCPacket()
	{
		
	}
	inline deque<XML_ZMQ>* get_zmqdeque()
	{
		return &zmqdeque_;
	}
private:
	deque<XML_ZMQ> zmqdeque_;
};

class XML_CombineDCPacket
{
public:
	XML_CombineDCPacket()
	{
		
	}
	~XML_CombineDCPacket()
	{
		
	}
	inline deque<XML_ZMQ>* get_zmqdeque()
	{
		return &zmqdeque_;
	}
private:
	deque<XML_ZMQ> zmqdeque_;
};

class XML_Lua_Routine
{
public:
	XML_Lua_Routine(){};
	~XML_Lua_Routine(){};
	inline deque<XML_ZMQ> *get_zmqdeque()
	{
		return &zmqdeque_;
	}
private:
	deque<XML_ZMQ> zmqdeque_;
};

class XML_Log
{
public:
	XML_Log()
	{
		
	}
	~XML_Log()
	{
		
	}
	inline deque<XML_ZMQ>* get_zmqdeque()
	{
		return &zmqdeque_;
	}
private:
	deque<XML_ZMQ> zmqdeque_;
};

class XML_Business_Error_Info
{
public:
	XML_Business_Error_Info(){};
	~XML_Business_Error_Info(){};
	inline deque<XML_ZMQ> * get_zmqdeque()
	{
		return &zmqdeque_;
	}
private:
	deque<XML_ZMQ> zmqdeque_;
};

//class	XML_DID
//{
//public:
//	XML_DID(){};
//	~XML_DID(){};
//	static void set_did_config_path(std::string did_config_path)
//	{
//		did_config_path_ = did_config_path;
//	}
//	static  std::string get_did_config_path()
//	{
//		return did_config_path_;
//	}
//	static void push_did_templates_path(std::string & did_template_path)
//	{
//		did_templates_path_.push_back(did_template_path);
//	}
//	static vector<std::string>& get_did_templates_path()
//	{
//		return did_templates_path_;
//	}
//private:
//	static std::string did_config_path_;
//	static vector<std::string> did_templates_path_;
//};

class XML_ListeningItem
{
public:
	XML_ListeningItem(std::string net_adapter,int port,std::string filter):net_adapter_(net_adapter),port_(port),filter_(filter)
	{
	}
	~XML_ListeningItem()
	{

	}
	inline std::string & get_net_adapter()
	{	
		return net_adapter_;
	}
	inline int& get_port()
	{
		return port_;
	}
	inline std::string& get_filter()
	{
		return filter_;
	}
	inline XML_CaptureNetPacket *get_capture_net_packet()
	{
		return &capture_net_packet_;
	}
	inline XML_ShuntNetPacket *get_shunt_net_packet()
	{
		return &shunt_net_packet_;
	}
	// inline XML_Parse *get_parse()
	// {
	// 	return &parse_;
	// }
	inline XML_CombineDCPacket *get_combine_dc_packet()
	{
		return &combine_dc_packet_;
	}
	inline XML_UncompressDCPacket *get_uncompress_dc_packet()
	{
		return &uncompress_dc_packet_;
	}
	inline XML_Lua_Routine *get_lua_routine()
	{
		return &lua_routine_;
	}
	inline void push_did_template_ids(std::string did_template_id)
	{
		did_templates_ids_.push_back(did_template_id);
	}
	inline void push_did_template_paths(std::string & did_template_path)
	{
		did_templates_paths_.push_back(did_template_path);
	}
	inline void insert_did_filepath_map(std::pair<int, std::string> element)
	{
		did_filepath_map_.insert(element);
	}
	inline map<int, std::string> & get_did_filepath_map()
	{
		return did_filepath_map_;
	}
	inline vector<std::string>& get_did_template_paths()
	{
		return did_templates_paths_;
	}
	inline vector<std::string> & get_did_template_ids()
	{
		return did_templates_ids_;
	}
protected:
private:
	std::string net_adapter_;
	int port_;
	std::string filter_;
	XML_Capture cap_;
	//XML_Parse parse_;
	XML_CombineDCPacket combine_dc_packet_;
	XML_UncompressDCPacket uncompress_dc_packet_;
	XML_Lua_Routine lua_routine_;
	XML_CaptureNetPacket capture_net_packet_;
	XML_ShuntNetPacket shunt_net_packet_;
	vector<std::string> did_templates_paths_;
	vector<std::string> did_templates_ids_;
	map<int,std::string> did_filepath_map_;
};

#endif
