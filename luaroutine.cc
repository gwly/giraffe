#include "luaroutine.h"
#include "flags.h"
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <csignal>

using namespace std;
using namespace log4cxx;

LoggerPtr LuaRoutine::logger_(Logger::getLogger("luaroutine"));
 
const char load_file[] = "process.lua";

unsigned long count_pack = 0;
struct itimerval tick;  

void PrintCountInfo(int signo)
{
    switch(signo)
    {
        case SIGALRM:
            //cout<<"pack count:"<<count_pack<<endl;
            count_pack= 0;
            break;
        default:
            break;
    }
    return ;
}


void LuaRoutine::Init()
{
	InitZMQ();
	InitLua();
}

void LuaRoutine::InitLua()
{
	lua_state_ = luaL_newstate();
	assert(NULL != lua_state_);
	luaL_openlibs(lua_state_);
	luaL_dofile(lua_state_ , load_file);
	
	lua_getglobal(lua_state_, "InitZMQ");
	lua_pushinteger(lua_state_, lua_zmqpattern_);
	lua_pushstring(lua_state_, lua_zmqsocketaction_.c_str());
	lua_pushstring(lua_state_, lua_zmqsocketaddr_.c_str());

	if(lua_pcall(lua_state_, 3, 0, 0) != 0)
	{
		string s = lua_tostring(lua_state_,-1);
		lua_pop(lua_state_,-1);
		lua_close(lua_state_);
	}
	else
	{
		lua_pop(lua_state_, -1);	
	}

	vector<std::string> & did_template_ids = listening_item_.get_did_template_ids();
	for(vector<std::string>::iterator iter = did_template_ids.begin();iter != did_template_ids.end();iter++)
	{
		lua_getglobal(lua_state_, "init");
		lua_pushstring(lua_state_, (*iter).c_str());
		
		if(lua_pcall(lua_state_,1,0,0) != 0)
		{
			string s = lua_tostring(lua_state_,-1);
			lua_pop(lua_state_,-1);
			lua_close(lua_state_);
		}
		else
		{
			lua_pop(lua_state_, -1);
		}
	}
}

void LuaRoutine::InitZMQ()
{
	assert(-1 != this->zmqitems_[0].zmqpattern);
	sock_ = new zmq::socket_t(*context_,this->zmqitems_[0].zmqpattern);
	//sock_->setsockopt(ZMQ_RCVHWM, &ZMQ_RCVHWM_SIZE, sizeof(ZMQ_RCVHWM_SIZE));
    if("bind" == this->zmqitems_[0].zmqsocketaction)
    {
        sock_->bind(this->zmqitems_[0].zmqsocketaddr.c_str());
    }
    else if("connect" == this->zmqitems_[0].zmqsocketaction)
    {
        sock_->connect(this->zmqitems_[0].zmqsocketaddr.c_str());
    }

	assert(-1 != this->zmqitems_[1].zmqpattern);
	lua_zmqpattern_ = zmqitems_[1].zmqpattern;
	
    if("bind" == this->zmqitems_[1].zmqsocketaction)
    {
		lua_zmqsocketaction_ = "bind";
    }
    else if("connect" == this->zmqitems_[1].zmqsocketaction)
    {
		lua_zmqsocketaction_ = "connect";
    }
	lua_zmqsocketaddr_ = zmqitems_[1].zmqsocketaddr;
}

struct STK_STATIC * LuaRoutine::GetStkByID(int stk_id)
{
	assert(NULL != stk_static_);
	return stk_static_ + stk_id;	
}

//void LuaRoutine::DispatchToMonitor(int stk_id, const char *value)
//{
//	assert(NULL != value);
//	STK_STATIC * pstkstaticitem = GetStkByID(stk_id);
//	MonitorMsg *monitor_msg  = (MonitorMsg *)(monitor_mapping_file_->GetMapMsg());
//	time_t t;
//	t = time(&t);
//	dzh_time_t current_time(t);
//	monitor_msg->time = current_time;
//	strcpy(monitor_msg->error_type,"BUSINESS");
//	strcpy(monitor_msg->error_level,"WARNING");
//	strcpy(monitor_msg->stock_label, pstkstaticitem->m_strLabel);
//	strcpy(monitor_msg->error_info, value);
//}

void LuaRoutine::DispatchToLua(unsigned char * pdcdata, int dc_type,int dc_general_intype, int stk_num, int struct_size, int did_template_id)
{
	assert(NULL != pdcdata);
	//did
	if(DCT_DID == dc_type)
	{
		lua_getglobal(lua_state_, "test_process_did");
		lua_pushinteger(lua_state_, did_template_id);
		lua_pushlightuserdata(lua_state_, pdcdata);
		lua_pushinteger(lua_state_, stk_num);
		if(lua_pcall(lua_state_, 3, 0, 0) != 0)
		{
			LOG4CXX_ERROR(logger_, lua_tostring(lua_state_,-1));
			lua_pop(lua_state_,-1);
			lua_close(lua_state_);
		}
		else
		{
			lua_pop(lua_state_,-1);
		}		
	}
	//static || dyna || shl2-mmpex || szl2-order-stat || szl2-order-five || szl2-trade-five
	else if (	DCT_STKSTATIC == dc_type 		|| \
				DCT_STKDYNA == dc_type 	 		|| \
				DCT_SHL2_MMPEx == dc_type 		|| \
				DCT_SZL2_ORDER_STAT == dc_type 	|| \
				DCT_SZL2_ORDER_FIVE == dc_type 	|| \
				DCT_SZL2_TRADE_FIVE == dc_type)
	{
		//working_lua
		//for(int i=0;i<stk_num;i++)
		//{
		//	count_pack += 1;
		//	//count_pack += stk_num*struct_size;
		//	lua_getglobal(lua_state_,"process");
		//	lua_pushinteger(lua_state_, dc_type);
		//	lua_pushlightuserdata(lua_state_,pdcdata+struct_size * i);
		//	//Sleep(50);
		//	if(lua_pcall(lua_state_,2,0,0) != 0)
		//	{
		//		string s = lua_tostring(lua_state_,-1);
		//		std::cout<<s<<endl;
		//		lua_pop(lua_state_,-1);
		//		lua_close(lua_state_);
		//	}
		//	else
		//	{
		//		//const char * lua_ret = lua_tostring(lua_state_,-1);
		//		//int stkid = lua_tonumber(lua_state_, -2);
		//		//if(NULL != lua_ret)
		//		//{
		//		//	//cout<<"lua stkid:"<<stkid<<"  lua_ret:"<<lua_ret<<endl;
		//		//	//DispatchToMonitor(stkid, lua_ret);
		//		//}
		//		lua_pop(lua_state_,-1);
		//	}
		//}
        lua_getglobal(lua_state_, "test_process");
        lua_pushinteger(lua_state_, dc_type);
        lua_pushinteger(lua_state_, stk_num);
        lua_pushlightuserdata(lua_state_, pdcdata);
        if(lua_pcall(lua_state_,3,0,0) != 0)
        {
            string s = lua_tostring(lua_state_,-1);
			LOG4CXX_ERROR(logger_, s);
            lua_pop(lua_state_,-1);
            lua_close(lua_state_);
        }
        else
        {
            lua_pop(lua_state_,-1);
        }
	}
	else if(DCT_GENERAL == dc_type)
	{
		unsigned char *pdata = pdcdata + stk_num *sizeof(WORD);
		lua_getglobal(lua_state_, "process_general");
		lua_pushinteger(lua_state_, dc_general_intype);
		lua_pushlightuserdata(lua_state_, pdata);
		lua_pushinteger(lua_state_, stk_num);
		if(lua_pcall(lua_state_, 3, 0, 0) != 0)
		{
			string s = lua_tostring(lua_state_,-1);
			LOG4CXX_ERROR(logger_, s);
			lua_pop(lua_state_,-1);
			lua_close(lua_state_);
		}
		else
			lua_pop(lua_state_, -1);
			
		//for(int i=0;i<stk_num;i++)
		//{
		//	lua_getglobal(lua_state_,"process_general");
		//	lua_pushinteger(lua_state_,dc_general_intype);
		//	lua_pushlightuserdata(lua_state_,pdata + struct_size * i);
		//	
		//	if(lua_pcall(lua_state_,2,0,0) != 0)
		//	{
		//		string s = lua_tostring(lua_state_,-1);
		//		LOG4CXX_ERROR(logger_, s);
		//		lua_pop(lua_state_,-1);
		//		lua_close(lua_state_);
		//	}
		//	else
		//	{
		//		lua_pop(lua_state_, -1);
		//	}
		//}
	}
	else if (DCT_SHL2_QUEUE)
	{
		/* code */
	}
	else
	{
			
	}
	free(pdcdata);
	pdcdata = NULL;
}


void LuaRoutine::RunThreadFunc()
{
	//unsigned char * pdata = (unsigned char *)malloc(2000*sizeof(struct STK_STATIC));
	//memset(pdata, 0, 2000*sizeof(struct STK_STATIC));
	//struct STK_STATIC stk_static;
	//memset(stk_static.m_strLabel, 0, sizeof(stk_static.m_strLabel));
	//memcpy(stk_static.m_strLabel, "fdafsdf",5);
	//memset(stk_static.m_strName, 0, sizeof(stk_static.m_strName));
	//memcpy(stk_static.m_strName,"hhhhh",3);
	//stk_static.m_cType = DCT_STKSTATIC;
	//stk_static.m_nPriceDigit = '1';
	//stk_static.m_nVolUnit = 321;
	//stk_static.m_mFloatIssued = 134;
	//stk_static.m_mTotalIssued = 321;
	//stk_static.m_dwLastClose = 4324;
	//stk_static.m_dwAdvStop = 432;
	//stk_static.m_dwDecStop = 23423;
	//struct STK_STATIC *p = (struct STK_STATIC *)pdata; 

	//for(int i=0;i<2000;i++)
	//{
	//	memcpy(p+i,(unsigned char *)&stk_static,sizeof(stk_static));	
	//}
	
    //signal(SIGALRM, PrintCountInfo);
    //tick.it_value.tv_sec = 10;
    //tick.it_value.tv_usec = 0;

    //tick.it_interval.tv_sec = 60;
    //tick.it_interval.tv_usec = 0;

    //setitimer(ITIMER_REAL,&tick,NULL);

	zmq::message_t msg_rcv(sizeof(Lua_ZMQ_MSG_Item));
	while(true)
	{
		msg_rcv.rebuild();
		sock_->recv(&msg_rcv);
		Lua_ZMQ_MSG_Item *msg_item = (Lua_ZMQ_MSG_Item*)(msg_rcv.data());
		stk_static_ = msg_item->stk_static;
		//free(msg_item->pdcdata);
		//msg_item->pdcdata = NULL;
		DispatchToLua(msg_item->pdcdata, msg_item->dc_type, msg_item->dc_general_intype, msg_item->stk_num, msg_item->struct_size, msg_item->did_template_id);
		//DispatchToLua(pdata, DCT_STKSTATIC, 0, 2000, sizeof(stk_static), 0);
	}
}
