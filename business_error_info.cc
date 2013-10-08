#include "business_error_info.h"
#include <zmq.hpp>
#include <iostream>
#include <log4cxx/logger.h>

using namespace log4cxx;

LoggerPtr BusinessErrorInfo::logger_(Logger::getLogger("business_errinfo"));

void BusinessErrorInfo::Init()
{
	InitZMQ();
	LOG4CXX_INFO(logger_, "business thread: zmq has initialized!");
	InitLog();
	LOG4CXX_INFO(logger_, "business thread: log has initialized!");
}

void BusinessErrorInfo::InitZMQ()
{
    sock_recv_ = new zmq::socket_t (*context_, this->zmqitems_[0].zmqpattern);
    if("bind" == this->zmqitems_[0].zmqsocketaction)
    {
        sock_recv_->bind(this->zmqitems_[0].zmqsocketaddr.c_str());
    }
    else if("connect" == this->zmqitems_[0].zmqsocketaction)
    {
        sock_recv_->connect(this->zmqitems_[0].zmqsocketaddr.c_str());
    }	
}

void BusinessErrorInfo::InitLog()
{
	log4cxx::LogString pattern_format("%d:%p:%m%n");
	log4cxx::LogString date_pattern("'.'yyyy-MM-dd");
	log4cxx::LogString file_name("business_error.log");
	
	pattern_layout_ = new log4cxx::PatternLayout(pattern_format);

	appender_ = new log4cxx::DailyRollingFileAppender(pattern_layout_, file_name, date_pattern);
	appender_->setAppend(true);
	
	logger_business_error_ = Logger::getLogger("business_error");
	logger_business_error_->addAppender(appender_);
	logger_business_error_->setAdditivity(false);	
}

void BusinessErrorInfo::RunThreadFunc()
{
	zmq::message_t msg;	
	while(true)
	{
		msg.rebuild();
		sock_recv_->recv(&msg);
		//std::cout<<"nihao"<<std::endl;
		LOG4CXX_ERROR(logger_business_error_, (char*)msg.data());
	}	
}
