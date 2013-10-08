#ifndef MONITOR_HANDLE_NET_PACKET_H
#define MONITOR_HANDLE_NET_PACKET_H
#include <log4cxx/logger.h>
#include "zmq.hpp"
#include "basethread.h"
#include "xml_class_set.h"
//#include "parse.h"
#include "combine_dc_packet.h"
#include "uncompress_dc_packet.h"
#include "luaroutine.h"


const int kPoolSize = 1;

class ShuntNetPacket:public BaseThread
{
public:
	ShuntNetPacket(zmq::context_t *context,XML_ListeningItem &listening_item):context_(context),listening_item_(listening_item),curent_pool_size_(0)
	{
		sock_ = NULL;
	}
	virtual ~ShuntNetPacket()
	{
		if (NULL != sock_)
		{
			delete sock_;
			sock_ = NULL;
		}
		while(!sock_ex_deque_.empty())
		{
			zmq::socket_t *sock = sock_ex_deque_.front();
			if(NULL != sock)
			{
				delete sock;
				sock = NULL;
			}
			sock_ex_deque_.pop_front();
		}
		//while(!parse_deque_.empty())
		//{
		//	Parse *parse = parse_deque_.front();
		//	if(NULL != parse)
		//	{
		//		delete parse;
		//		parse = NULL;
		//	}
		//	parse_deque_.pop_front();
		//}
		while(!combine_dc_deque_.empty())
		{
			CombineDCPacket *combine_dc_packet = combine_dc_deque_.front();
			if(NULL != combine_dc_packet)
			{
				delete combine_dc_packet;
				combine_dc_packet = NULL;
			}
			combine_dc_deque_.pop_front();
		}
		while(!uncompress_dc_deque_.empty())
		{
			UncompressDCPacket *uncompress_dc_packet = uncompress_dc_deque_.front();
			if(NULL != uncompress_dc_packet)
			{
				delete uncompress_dc_packet;
				uncompress_dc_packet = NULL;
			}
			uncompress_dc_deque_.pop_front();
		}
		while(!lua_routine_deque_.empty())
		{
			LuaRoutine *lua_routine = lua_routine_deque_.front();
			if(NULL != lua_routine)
			{
				delete lua_routine;
				lua_routine = NULL;
			}
			lua_routine_deque_.pop_front();
		}
		sock_ex_map_.clear();
	};
	void Init();
	void InitZMQEx(int index);
	inline void AddZMQItemEx(ZMQItem item)
	{
		zmqitems_ex_.push_back(item);
	}
	void AddToZMQDequeEx(int index);
	void RunLuaRoutineThread(int index);
	void RunParseThread(int index);
	void RunCombineDCPacketThread(int index);
	void RunUncompressDCPacketThread(int index);
	bool IncreasePool(int pool_sizse=kPoolSize);
	void DispatchData(zmq::socket_t * sock, void * data, int size);
	bool IsTcpDisConnection(unsigned char flags);
	bool IsTcpConnection(unsigned char flags, int &tcpconntag, int &tcpconnstatus);
	void WriteDidConfFile(const char * file_name, vector<DidStruct> &did_structs);
	void CreateDidConfContent(vector<DidStruct> & did_structs, char * out_str);
	void PreHandleADisconnection();
	void RunThreadFunc();
private:
	zmq::context_t *context_;
	zmq::socket_t *sock_;
	XML_ListeningItem listening_item_;
	//deque<Parse *> parse_deque_;
	deque<CombineDCPacket *> combine_dc_deque_;
	deque<UncompressDCPacket *> uncompress_dc_deque_;
	deque<LuaRoutine*> lua_routine_deque_;
	deque<zmq::socket_t *>sock_ex_deque_;
	map<std::string,zmq::socket_t *> sock_ex_map_;
	map<zmq::socket_t *, unsigned long> sock_delta_time_map_;
	int curent_pool_size_;
	std::vector<ZMQItem> zmqitems_ex_;
	static log4cxx::LoggerPtr logger_;
};

#endif
