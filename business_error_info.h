#ifndef MONITOR_BUSINESS_ERROR_INFO_H
#define MONITOR_BUSINESS_ERROR_INFO_H

#include <zmq.hpp>
#include <log4cxx/logger.h>
#include <log4cxx/dailyrollingfileappender.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/patternlayout.h>
#include "basethread.h"

class BusinessErrorInfo: public BaseThread
{
public:
	BusinessErrorInfo(zmq::context_t *context):context_(context)
	{
		sock_recv_ = NULL;
		logger_business_error_ = NULL;
		pattern_layout_ = NULL;
		appender_ = NULL;
	}	
	~BusinessErrorInfo()
	{
		if(NULL != sock_recv_)
		{
			sock_recv_->close();
			delete sock_recv_;
			sock_recv_ = NULL;
		}
		if(NULL != logger_business_error_)
		{
			delete logger_business_error_;
			logger_business_error_ = NULL;
		}
		if(NULL != pattern_layout_)
		{
			delete pattern_layout_;
			pattern_layout_ = NULL;
		}
		if(NULL != appender_)
		{
			delete appender_;
			appender_ = NULL;
		}
	};
	void Init();
	void *RunThreadFunc();
private:
	void InitZMQ();
	void InitLog();
	void WriteToLog(const std::string str);

	zmq::context_t *context_;
	zmq::socket_t *sock_recv_;
	static log4cxx::LoggerPtr logger_;
	log4cxx::LoggerPtr logger_business_error_;
	log4cxx::PatternLayoutPtr pattern_layout_;
	log4cxx::DailyRollingFileAppenderPtr appender_;
};

#endif
