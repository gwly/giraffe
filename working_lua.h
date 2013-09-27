#ifndef     MONITOR_WORKING_LUA_H_
#define     MONITOR_WORKING_LUA_H_

#include <iostream>
#ifdef __linux
    #include <luajit-2.0/lua.hpp>
#else
    #include <lua.hpp>
#endif
#include "basethread.h"

using namespace std;

class Working_Lua:public BaseThread
{
public:
    Working_Lua(){};
    ~Working_Lua(){};
    void Init(zmq::context_t * context);
    void RegisterToLua(lua_State *L);
    void * RunThreadFunc();
protected:
private:
    zmq::context_t *context_;
};


#endif // MONITOR_WORKING_LUA_H_
