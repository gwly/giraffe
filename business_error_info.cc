#include "business_error_info.h"
#include <zmq.hpp>
#include <iostream>
#include <log4cxx/logger.h>
#include <ctime>


using namespace log4cxx;

LoggerPtr BusinessErrorInfo::logger_(Logger::getLogger("business_errinfo"));

void BusinessErrorInfo::Init()
{
	InitZMQ();
	LOG4CXX_INFO(logger_, "business thread: zmq has initialized!");
	InitLog();
	LOG4CXX_INFO(logger_, "business thread: log has initialized!");
	InitCurl();
	LOG4CXX_INFO(logger_, "business thead: curl has initialized!");
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

void BusinessErrorInfo::InitCurl()
{
	curl_ = curl_easy_init();
	if(!curl_)
	{
		LOG4CXX_ERROR(logger_, "curl init error!");
	}	
}

void BusinessErrorInfo::RunThreadFunc()
{
	zmq::message_t msg;
	while(true)
	{
		msg.rebuild();
		sock_recv_->recv(&msg);
		//DispatchToWebServer();
		LOG4CXX_ERROR(logger_business_error_, (char*)(msg.data()));
	}	
}

void BusinessErrorInfo::DispatchToWebServer(std::string &uri, std::string &market_id, std::string &error_id, std::string &type)
{
	std::string curl_url_str;
	time_t now_time = time(NULL);		
	char now_time_buf[128] = {0};
	sprintf(now_time_buf, "%llu", (unsigned long long)now_time);
	curl_url_str = uri + "?"  
					+ "time=" + now_time_buf 
					+ "exchange=" + market_id 
					+ "error=" + error_id 
					+ "type=" + type ;

	curl_easy_setopt(curl_, CURLOPT_URL, "http://10.15.63.121/Control/input.php?time=1&exchange=2&error=3&type=1");
	curl_easy_setopt(curl_, CURLOPT_TIMEOUT, 3);
	curl_res_code_ = curl_easy_perform(curl_);
	if(CURLE_OK == curl_res_code_)
	{
		char *cct;
		curl_res_code_ = curl_easy_getinfo(curl_, CURLINFO_CONTENT_TYPE, &cct);
		if((CURLE_OK == curl_res_code_) && cct)
		{
			LOG4CXX_INFO(logger_, "receive content-type:" << cct);
		}
		else
		{
			LOG4CXX_ERROR(logger_, "get received content-type error");
			curl_easy_cleanup(curl_);
		}	
	}	
	else
	{
		LOG4CXX_ERROR(logger_, "curl running error");
		curl_easy_cleanup(curl_);
	}
}
