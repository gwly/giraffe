#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include <log4cxx/logger.h>
#include <log4cxx/rollingfileappender.h>
#include <log4cxx/basicconfigurator.h>
#include <log4cxx/patternlayout.h>
#include <log4cxx/simplelayout.h>
#include <log4cxx/logmanager.h>
//#include <zmq.hpp>
//#include "configsax2handler.h"
//#include "FriendlyExceptionHandle.h"
#include "xml2cfg.h"
#include "xml_class_set.h"
//#include "capture.h"
#include "log.h"
#include "capture_net_packet.h"
#include "shunt_net_packet.h"
#include "business_error_info.h"
#include "DIDTemplateToLuaStruct.h"
#include "configparser.h"
#include "flags.h"


using namespace std;
using namespace log4cxx;
using namespace log4cxx::helpers;

//extern map<string, int> logtagmap;

#ifndef __linux
bool Event_Routine(DWORD event_type)
{
	switch(event_type)
	{
	case CTRL_CLOSE_EVENT:
		return true;
	}
}
#else
#endif

//int get_network_adapter()
//{
//	pcap_if_t *alldevs;
//    char *errbuf= new char[PCAP_ERRBUF_SIZE];
//	int devsnum = 0;
//	int inum = 0;
//    pcap_if_t *d;
//
//	if(pcap_findalldevs(&alldevs,errbuf) == -1)
//	{
//		cout<<"Error in pcap_findalldevs %s "<<errbuf<<endl;
//		return -1;
//	}
//	if(NULL != errbuf)
//	{
//		delete [] errbuf;
//		errbuf = NULL;
//	}
//
//	for(d=alldevs; d; d=d->next)
//	{
//		cout<<++devsnum<<". "<<d->name;
//		if(d->description)
//			cout<<" ("<<d->description<<")"<<endl;
//		else
//			cout<<" (No description)"<<endl;
//	}
//
//	cout<<"Please input the number of adapter you choose:"<<endl;
//	cin>>inum;
//	if(inum<1||inum>devsnum)
//	{
//		cout<<"Adapter number out of range!"<<endl;
//		pcap_freealldevs(alldevs);
//		return  -2;
//	}
//	else
//		return inum;
//}

void RunLogThread(zmq::context_t &context, Log &log ,XML_Log & xml_log)
{
	deque<XML_ZMQ> * log_zmqdeque = xml_log.get_zmqdeque();
	//init log zmq property based on the config file
	for(deque<XML_ZMQ>::iterator iter = log_zmqdeque->begin();iter!=log_zmqdeque->end();iter++)
	{
		ZMQItem zmq_item;
		zmq_item.zmqpattern = (*iter).get_zmqpattern();
		zmq_item.zmqsocketaction = (*iter).get_zmqsocketaction();
		zmq_item.zmqsocketaddr = (*iter).get_zmqsocketaddr();
		log.AddZMQItem(zmq_item);
	}
	log.Start();
}

void RunBusinessErrorInfoThread(zmq::context_t &context, BusinessErrorInfo &business_errinfo ,XML_Business_Error_Info & xml_business_errinfo)
{
	deque<XML_ZMQ> * business_errinfo_zmqdeque = xml_business_errinfo.get_zmqdeque();
	//init log zmq property based on the config file
	for(deque<XML_ZMQ>::iterator iter = business_errinfo_zmqdeque->begin();iter!=business_errinfo_zmqdeque->end();++iter)
	{
		ZMQItem zmq_item;
		zmq_item.zmqpattern = (*iter).get_zmqpattern();
		zmq_item.zmqsocketaction = (*iter).get_zmqsocketaction();
		zmq_item.zmqsocketaddr = (*iter).get_zmqsocketaddr();
		business_errinfo.AddZMQItem(zmq_item);
	}
	business_errinfo.Init();
	business_errinfo.Start();
}

void TransformDidTemplateToLua(XML_ListeningItem & xml_listening_item)
{
	DIDTemplateToLuaStruct did_to_lua;
	did_to_lua.Transform(xml_listening_item);
}

void RunCaptureThread(zmq::context_t &context, deque<CaptureNetPacket> & cap_net_packet_deque, deque<ShuntNetPacket> & shunt_net_packet_deque, deque<XML_ListeningItem> &listeningitem_deque)
{
	//create capture thread based on the config file
	int index = 0;
	for(deque<XML_ListeningItem>::iterator item=listeningitem_deque.begin();item != listeningitem_deque.end();item++)
	{
		TransformDidTemplateToLua(*item);
		CaptureNetPacket capture_net_packet(item->get_net_adapter(), item->get_filter(), &context);
		ShuntNetPacket shunt_net_packet(&context, *item);

		deque<XML_ZMQ> *capture_net_packet_zmq_deque = (*item).get_capture_net_packet()->get_zmqdeque();
		for(deque<XML_ZMQ>::iterator iter_cap=capture_net_packet_zmq_deque->begin();iter_cap!=capture_net_packet_zmq_deque->end();iter_cap++)
		{
			ZMQItem capture_net_packet_zmq_item;
			capture_net_packet_zmq_item.zmqpattern = (*iter_cap).get_zmqpattern();
			capture_net_packet_zmq_item.zmqsocketaction = (*iter_cap).get_zmqsocketaction();
			char buf[16];
			memset(buf,0,sizeof(buf));
			sprintf(buf,"%d",index);
			capture_net_packet_zmq_item.zmqsocketaddr = (*iter_cap).get_zmqsocketaddr() + buf;
			capture_net_packet.AddZMQItem(capture_net_packet_zmq_item);
		}

		deque<XML_ZMQ> *shunt_net_packet_zmq_deque = (*item).get_shunt_net_packet()->get_zmqdeque();
		for(deque<XML_ZMQ>::iterator iter_handle=shunt_net_packet_zmq_deque->begin();iter_handle!=shunt_net_packet_zmq_deque->end();iter_handle++)
		{
			ZMQItem shunt_net_packet_zmq_item;
			shunt_net_packet_zmq_item.zmqpattern = (*iter_handle).get_zmqpattern();
			shunt_net_packet_zmq_item.zmqsocketaction = iter_handle->get_zmqsocketaction();
			char buf[16];
			memset(buf,0,sizeof(buf));
			sprintf(buf,"%d",index);
			shunt_net_packet_zmq_item.zmqsocketaddr = iter_handle->get_zmqsocketaddr() + buf;
			shunt_net_packet.AddZMQItem(shunt_net_packet_zmq_item);
			break;
		}

		index++;
		cap_net_packet_deque.push_back(capture_net_packet);
		shunt_net_packet_deque.push_back(shunt_net_packet);
	}

	for(deque<CaptureNetPacket>::iterator iter=cap_net_packet_deque.begin();iter!=cap_net_packet_deque.end();iter++)
	{
		iter->Init();
		iter->Start();
	}
	for(deque<ShuntNetPacket>::iterator iter=shunt_net_packet_deque.begin();iter!=shunt_net_packet_deque.end();iter++)
	{
		iter->Init();
		Utils::SleepUsec(1000000);
		iter->Start();
	}
}

void JoinCaptureThread(deque<CaptureNetPacket> & capture_net_packet_deque, deque<ShuntNetPacket> & shunt_net_packet_deque)
{
	for(deque<CaptureNetPacket>::iterator iter=capture_net_packet_deque.begin();iter!=capture_net_packet_deque.end();iter++)
	{
		iter->Join();
	}
	for(deque<ShuntNetPacket>::iterator iter=shunt_net_packet_deque.begin();iter!=shunt_net_packet_deque.end();iter++)
	{
		iter->Join();
	}
}

int main()
{
#ifndef __linux
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)Event_Routine,true);
#else
#endif

	log4cxx::LogString pattern_layout("%d [%t] %-5p - %m (%F:%L)%n");
	log4cxx::RollingFileAppender * rf_appender = new log4cxx::RollingFileAppender(log4cxx::LayoutPtr(new log4cxx::PatternLayout(pattern_layout)),"app.log",false);
	rf_appender->setMaxFileSize("1G");

	log4cxx::BasicConfigurator::configure(log4cxx::AppenderPtr(rf_appender));
	log4cxx::LoggerPtr root_logger = log4cxx::Logger::getRootLogger();
	root_logger->setLevel(log4cxx::Level::getDebug());
	root_logger->addAppender(log4cxx::AppenderPtr(rf_appender));
	log4cxx::LoggerPtr logger = log4cxx::Logger::getLogger("monitor");
	LOG4CXX_INFO(logger, "begin");

	//int adapter_id = get_network_adapter();
	//assert(adapter_id >= 0);
	ConfigParser config_parser(CONFIG_FILE);
	config_parser.Parse();

	zmq::context_t context(1);

	vector<int>& ports = config_parser.get_ports();
	XML_Log & xml_log = config_parser.get_log_obj();
	Log log(&context,ports);
	RunLogThread(context, log, xml_log);
	XML_Business_Error_Info & xml_business_errinfo = config_parser.get_business_error_info_obj();
	BusinessErrorInfo business_errinfo(&context);
	RunBusinessErrorInfoThread(context, business_errinfo, xml_business_errinfo);

	deque<CaptureNetPacket> cap_net_packet_deque;
	deque<ShuntNetPacket> shunt_net_packet_deque;
	deque<XML_ListeningItem> & listeningitem_deque = config_parser.get_listeningitems();
	RunCaptureThread(context, cap_net_packet_deque, shunt_net_packet_deque, listeningitem_deque);
	JoinCaptureThread(cap_net_packet_deque, shunt_net_packet_deque);

	pthread_exit(NULL);
	context.close();

	//////////////////////////////////////////////////////////////////////////////
	//EnableFriendllyExceptionHandle( true );
    //int n;
    //int i =0;

    //try
    //{
    //    XMLPlatformUtils::Initialize();
    //}
    //catch(const XMLException &err)
    //{
    //    char *msg = XMLString::transcode(err.getMessage());
    //    cout<<"initial error:"<<msg<<endl;
    //    XMLString::release(&msg);
    //    return 1;
    //}

    //char *xmlfile = CONFIGFILE;
    //SAX2XMLReader *reader = XMLReaderFactory::createXMLReader();
    //reader->setFeature(XMLUni::fgSAX2CoreValidation,true);

    //ConfigSAX2Handler * handler = new ConfigSAX2Handler();
    //reader->setContentHandler(handler);
    //reader->setErrorHandler(handler);

    //try
    //{
        //reader->parse(xmlfile);

        //zmq::context_t context(1);

//        n = ports.size();
//        for(i=0;i<n;i++)
//        {
//            logtagmap.insert(pair<string,int>(filters[i],ports[i]));
//        }
//
//        for(i=0;i<logs.size();i++)
//        {
//            logs[i].init(&context,ports);
//            logs[i].Start();
//        }
//
//		//for(i=0;i<working_luas.size();i++)
//		//{
//		//	working_luas[i].Init(&context);
//		//	working_luas[i].Start();
//		//}
//
//        for(i=0;i<n;i++)
//        {
//            cout<<ports[i]<<endl;
//            cout<<filters[i]<<endl;
//            cout<<caps[i].zmqitems_[0].zmqsocketaddr<<endl;
//            caps[i].init(&context, filters[i]);
//            caps[i].Start();
//#ifdef __linux
//            sleep(1);
//#else
//			Sleep(1000);
//#endif
//        }
//
//        for(i=0;i<n;i++)
//        {
//            parses[i].init(&context);
//            parses[i].Start();
//        }
//
//        for(i=0;i<logs.size();i++)
//        {
//            logs[i].Join();
//        }
//        //for(i=0;i<n;i++)
//        //{
//        //    caps[i].Join();
//        //    parses[i].Join();
//        //    //working_luas[i].Join();
//        //}

        //context.close();

    //}
    //catch(const XMLException &err)
    //{
    //    char *msg = XMLString::transcode(err.getMessage());
    //    cout<<"error:xml exceptiion"<<endl;
    //    XMLString::release(&msg);
    //    return -1;
    //}
    //catch(const SAXParseException &err)
    //{
    //    char *msg = XMLString::transcode(err.getMessage());
    //    cout<<"error:saxpase"<<endl;
    //    XMLString::release(&msg);
    //    return -1;
    //}
    //delete reader;
    //delete handler;

    return 0;
}


