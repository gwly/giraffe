#ifndef MONITOR_LUA_ROUTINE_H

#define MONITOR_LUA_ROUTINE_H
#include <log4cxx/logger.h>
#include "zmq.hpp"
#include "basethread.h"
#include "xml_class_set.h"
//#include "MonitorFileMap.h"
#include "datacollect.h"

#ifdef __linux
    #include <luajit-2.0/lua.hpp>
#else
    #include <lua.hpp>
#endif


class LuaRoutine:public BaseThread
{
public:
	LuaRoutine(zmq::context_t *context, XML_ListeningItem & listening_item):context_(context),listening_item_(listening_item)
	{
		lua_state_ = NULL;
		sock_ = NULL;
		stk_static_ = NULL;
		//monitor_mapping_file_ = new MonitorFileMap("FX_191111.dat",MonitorFileMap::BUILD);
	}
	~LuaRoutine()
	{
		if(NULL != lua_state_)
		{
			lua_close(lua_state_);
			lua_state_ = NULL;
		}
		if(NULL != sock_)
		{
			delete sock_;
			sock_ = NULL;
		}
		//if(NULL != monitor_mapping_file_)
		//{
		//	delete monitor_mapping_file_;
		//	monitor_mapping_file_ = NULL;
		//}
	}
	void Init();
	void* RunThreadFunc();
protected:
private:
	void InitLua();
	void InitZMQ();
	//Lua_ZMQ_MSG_Item Test();
	void DispatchToLua(unsigned char * pdcdata, int dc_type, int dc_general_intype,int stk_num, int struct_size, int did_template_id);
	//void DispatchToMonitor(int stk_id, const char * value);
	struct STK_STATIC* GetStkByID(const int stk_id);
	zmq::context_t *context_;
	zmq::socket_t *sock_;
	int lua_zmqpattern_;
	std::string lua_zmqsocketaction_;
	std::string lua_zmqsocketaddr_;
	XML_ListeningItem listening_item_;
	lua_State * lua_state_;
	//MonitorFileMap * monitor_mapping_file_;
	STK_STATIC* stk_static_;
	static log4cxx::LoggerPtr logger_;
};

#endif
