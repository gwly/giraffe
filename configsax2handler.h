#include <iostream>
#include <vector>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include "structs.h"
#include "capture.h"
#include "parse.h"
#include "log.h"
#include "working_lua.h"
#include "xml_class_set.h"

using namespace std;
using namespace xercesc;

class ConfigSAX2Handler:public DefaultHandler
{
public:
    ConfigSAX2Handler();
    ~ConfigSAX2Handler();
    void startElement(const XMLCh *const uri, const XMLCh *const localname,const XMLCh *const qname, const Attributes &attrs);
    void endElement(const XMLCh *const uri, const XMLCh *const localname, const XMLCh *const qname);
    void characters(const XMLCh *const chars, const XMLSize_t length);
	vector <XML_ListeningItem>& get_listeningitem();
protected:
private:
    std::string content_;
    int countcapzmqs_;
    int countparsezmqs_;
    int countlogzmqs_;
    int countworkingluazmqs_;
    int threadtagcap_;
    int threadtagparse_;
    int threadtaglog_;
    int threadtagworkinglua_;
	vector<XML_ListeningItem> listeningitem_;
};

