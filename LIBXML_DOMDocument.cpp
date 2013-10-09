#include "LIBXML_DOMDocument.h"
#include <libxml/xpath.h>
#include <sstream>

namespace STEP
{
  bool LIBXML_DOMAttributes::get( const std::string& name, std::string& value )
  {

    xmlChar* result = xmlGetProp(m_pNode, (const xmlChar*)name.c_str());
    if(result == NULL) return false;
    value = (char*)result;
    return true;


  }

  DOMAttributes::map LIBXML_DOMAttributes::toMap()
  {
    xmlAttr* attr = m_pNode->properties;
    DOMAttributes::map map;
    while( attr != 0 )
    {
      std::string value;
      std::string name;
      if( attr->name ) name = (char*)attr->name;
      get(name, value);
      map[name] = value;
      attr = attr->next;
    }
    return map;
    
  }

  DOMNodePtr LIBXML_DOMNode::getFirstChildNode()
  {
    if( !m_pNode->children ) return DOMNodePtr();
    xmlNodePtr pNode = m_pNode->children;
    if( pNode == NULL ) return DOMNodePtr();
    return DOMNodePtr(new LIBXML_DOMNode(pNode));
  }

  DOMNodePtr LIBXML_DOMNode::getNextSiblingNode()
  {
    if( !m_pNode->next ) return DOMNodePtr();
    xmlNodePtr pNode = m_pNode->next;
    if( pNode == NULL ) return DOMNodePtr();
    return DOMNodePtr(new LIBXML_DOMNode(pNode));

  }

  DOMAttributesPtr LIBXML_DOMNode::getAttributes()
  {
    return DOMAttributesPtr(new LIBXML_DOMAttributes(m_pNode));
    
  }

  std::string LIBXML_DOMNode::getName()
  {
    return m_pNode->name ? (char*)m_pNode->name : "";

  }

  std::string LIBXML_DOMNode::getText()
  {
    return m_pNode->content ? (char*)m_pNode->content : "";

  }

  LIBXML_DOMDocument::LIBXML_DOMDocument() throw( ConfigError )
  : m_pDoc(NULL)
  {
  }

  LIBXML_DOMDocument::~LIBXML_DOMDocument()
  {
    xmlFreeDoc(m_pDoc);
  }

  bool LIBXML_DOMDocument::load( std::istream& stream )
  {

    try
    {
      std::stringstream sstream;
      sstream << stream.rdbuf();
      m_pDoc = xmlParseDoc((xmlChar*)sstream.str().c_str());
      return m_pDoc != NULL;
    }
    catch( ... ) { return false; }


  }

  bool LIBXML_DOMDocument::load( const std::string& url )
  {

    try
    {
      m_pDoc = xmlParseFile(url.c_str());
      return m_pDoc != NULL;
    }
    catch( ... ) { return false; }

  }

  bool LIBXML_DOMDocument::xml( std::ostream& out )
  {
    return false;
 
  }

  DOMNodePtr LIBXML_DOMDocument::getNode( const std::string& XPath )
  {

    xmlXPathContextPtr context = xmlXPathNewContext(m_pDoc);
    xmlXPathObjectPtr xpathObject = xmlXPathEval((xmlChar*)XPath.c_str(), context);

    if( xpathObject == NULL
        || xpathObject->nodesetval == NULL
        || xpathObject->nodesetval->nodeNr != 1 )
    {
      xmlXPathFreeContext(context);
      return DOMNodePtr();
    }

    DOMNodePtr result(new LIBXML_DOMNode(xpathObject->nodesetval->nodeTab[0]));
    xmlXPathFreeContext(context);
    xmlXPathFreeObject(xpathObject);
    return result;

  }
}


