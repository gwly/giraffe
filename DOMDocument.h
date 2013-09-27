
#ifndef STEP_DOMDOCUMENT_H
#define STEP_DOMDOCUMENT_H

#ifdef _MSC_VER
#pragma warning( disable : 4503 4355 4786 4290 )
#endif

#include <string>
#include <map>
#include <iostream>
#include <memory>

namespace STEP
{
  /// Interface that represents attribute from underlying XML parser.
  class DOMAttributes
  {
  public:
    typedef std::map<std::string, std::string> map;

    virtual ~DOMAttributes() {}

    virtual bool get( const std::string&, std::string& ) = 0;
    virtual map toMap() = 0;
  };
  typedef std::auto_ptr<DOMAttributes> DOMAttributesPtr;

  /// Interface that represents node from underlying XML parser.
  class DOMNode
  {
  public:
    virtual ~DOMNode() {}

    virtual std::auto_ptr<DOMNode> getFirstChildNode() = 0;
    virtual std::auto_ptr<DOMNode> getNextSiblingNode() = 0;
    virtual std::auto_ptr<DOMAttributes> getAttributes() = 0;
    virtual std::string getName() = 0;
    virtual std::string getText() = 0;
  };
  typedef std::auto_ptr<DOMNode> DOMNodePtr;

  /// Interface that represents document of underlying XML parser.
  class DOMDocument
  {
  public:
    virtual ~DOMDocument() {}

    virtual bool load( std::istream& ) = 0;
    virtual bool load( const std::string& ) = 0;
    virtual bool xml( std::ostream& ) = 0;

    virtual std::auto_ptr<DOMNode> getNode( const std::string& ) = 0;
  };
  typedef std::auto_ptr<DOMDocument> DOMDocumentPtr;
}

#endif
