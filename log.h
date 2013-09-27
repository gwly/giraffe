#ifndef MONITOR_LOG_H_
#define MONITOR_LOG_H_

#include <signal.h>
#include <pthread.h>
#include <deque>
#include <map>
#include <iostream>
#include <log4cxx/logger.h>
#include "log4cpp/Category.hh"
#include "log4cpp/Appender.hh"
#include "log4cpp/FileAppender.hh"
#include "log4cpp/RollingFileAppender.hh"
#include "log4cpp/Layout.hh"
#include "log4cpp/BasicLayout.hh"
#include "log4cpp/Priority.hh"
#include "log4cpp/PatternLayout.hh"
#include "flags.h"
#include "structs.h"
#include "basethread.h"
//#include "zhelpers.hpp"

using namespace std;

class Log:public BaseThread
{
public:
    Log(zmq::context_t *context,vector<int> & ports):context_(context),ports_(ports)
	{
	};
    ~Log(){};
	void *RunThreadFunc();
protected:
private:
    zmq::context_t *context_;
    vector<int> ports_;
	static log4cxx::LoggerPtr logger_;	
};

#endif // MONITOR_LOG_H_
