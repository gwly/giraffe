#ifndef __GW_UTILIY_XML_to_CONFIG_HEADER__
#define __GW_UTILIY_XML_to_CONFIG_HEADER__
#include <string>
#include <map>
#include <vector>
#include <cstring>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <errno.h>

using namespace std;

class xml2cfg
{
public:
	xml2cfg():
	  docptr_(NULL), xmlobjptr_(NULL),xmlctxptr_(NULL),xmlrootctxptr_(NULL),rootnode_(NULL)
	  {
	  }

	  int open( const char *filename )
	  {
		  try
		  {
			  docptr_ = xmlParseFile( filename );
		  }
		  catch(...)
		  {
			  if ( errno == 2 )
				  return -1;
		  }
		  if( docptr_ == NULL )
			  return -1;

		  xmlctxptr_ = xmlXPathNewContext( docptr_ );
		  xmlrootctxptr_ = xmlXPathNewContext( docptr_ );
		  return 0;
	  }

	  xmlNodeSetPtr locate( xmlNodePtr currentroot, const char *xpath )
	  {
		  if( xmlctxptr_ == NULL )
			  return NULL;
		  xmlXPathContextPtr _ptr = NULL;
		  if( currentroot == NULL )
			  _ptr = xmlrootctxptr_;
		  else
		  {
			  _ptr = xmlctxptr_;
			  _ptr->node = currentroot;
		  }
		  xmlobjptr_ = xmlXPathEvalExpression( (const xmlChar *)xpath, _ptr ); 
		  if( xmlobjptr_ == NULL )
			  return NULL;

		  return xmlobjptr_->nodesetval;
	  }

	  int get_nodes_num( xmlNodeSetPtr nodesetptr )
	  {
		  return nodesetptr->nodeNr;
	  }

	  xmlNodePtr get_node( xmlNodeSetPtr nodesetptr, int index )
	  {
		  return nodesetptr->nodeTab[index];
	  }

	  int get_prop_int( const xmlNodePtr nodeptr, const char *propname, int default_val )
	  {
		  xmlChar *_propval = xmlGetProp( nodeptr, (const xmlChar *)propname );
		  if( _propval == NULL )
			  return default_val;
		  int _ret =  atoi( (const char *)_propval );
		  xmlFree( _propval );
		  return _ret;
	  }

	  int get_prop_string(  const xmlNodePtr nodeptr, const char *propname, std::string &output, const char *default_val )
	  {
		  xmlChar *_propval = xmlGetProp( nodeptr, (const xmlChar *)propname );
		  if( _propval == NULL )
		  {
			  if( default_val != NULL && strlen( default_val ) > 0 )
				  output = default_val;
			  else
				  return -1;
		  }
		  else
		  {
			  output = (const char *)_propval;
			  xmlFree( _propval );
		  }
		  return 0;
	  }

	  int get_prop_string(  const xmlNodePtr nodeptr, const char *propname, char *output, const char *default_val )
	  {
		  xmlChar *_propval = xmlGetProp( nodeptr, (const xmlChar *)propname );
		  if( _propval == NULL )
		  {
			  if( default_val != NULL && strlen( default_val ) > 0 )
				  strcpy( output, default_val );
			  else
				  return -1;
		  }
		  else
		  {
			  strcpy( output, (const char *)_propval );
			  xmlFree( _propval );
		  }
		  return 0;
	  }

private:
	xmlDocPtr 				docptr_;
	xmlXPathObjectPtr		xmlobjptr_;
	xmlXPathContextPtr		xmlctxptr_, xmlrootctxptr_;
	xmlNodePtr				rootnode_;
};

#endif //__GW_UTILIY_XML_to_CONFIG_HEADER__
