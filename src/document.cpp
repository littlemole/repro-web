#include "reproweb/xml/document.h"
#include "reproweb/xml/patex.h"

namespace reproweb {
namespace xml {



std::string trim(const std::string& input)
{
	size_t start = input.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";

    size_t end = input.find_last_not_of(" \t\r\n");
    if (end == std::string::npos) end = input.size() - 1;

    return input.substr(start, end - start + 1);
}

class XMLParser : public Expat
{
//	friend class mol::XMLDocument;

//private:
public:
    XMLParser();
    ~XMLParser();
    Element* parse( DocumentPtr doc_, const std::string& input );
    Element* parse(DocumentPtr doc_, Element* root, const std::string& input);

	// handlers

	virtual void character (const XML_Char *s, int len);
	virtual void start(const XML_Char* el, const XML_Char **attr);
	virtual void end(const XML_Char* el);


	Element*				parent_;
	DocumentPtr				doc_;

};


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// XMLParser Impl
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

XMLParser::XMLParser()
{}

XMLParser::~XMLParser()
{
}

//////////////////////////////////////////////////////////////////////
// parse function
//////////////////////////////////////////////////////////////////////
Element* XMLParser::parse( DocumentPtr doc, const std::string& input )
{
   return parse( doc, doc->documentElement().get(), input );
}

//////////////////////////////////////////////////////////////////////
// workhorse
//////////////////////////////////////////////////////////////////////

Element* XMLParser::parse( DocumentPtr d, Element* root, const std::string& input )
{
	doc_    = d;
	parent_ = root;
	if ( !Expat::parse( input.c_str(), (int)(input.size()) ) )
	{

		return 0;
	}		
	return root;
}

void XMLParser::character (const XML_Char *s, int len)
{	
	std::string t = trim ( std::string( s,len) );
	if ( t.size() == 0 )
		return;

	NodePtr c = parent_->lastChild();
	if ( c && ( c->nodeType() == Node::TEXT ) )
	{
		std::string txt = parent_->lastChild()->nodeValue();
		parent_->lastChild()->nodeValue(txt + t);
	}
	else
	{
		TextPtr t = doc_->createTextNode( std::string(s,len) );
		parent_->appendChild(t);
	}
}

void XMLParser::start(const XML_Char* el, const XML_Char **attr)
{
	ElementPtr e = doc_->createElement( el );
	parent_->appendChild(e);
	const XML_Char** c = attr;
	while ( *c )
	{
		std::string key((char*)(*c));
		std::string val((char*)(*(c+1)));

		e->setAttribute( key,val );
		c+=2;
	}
	parent_ = e.get();
}

void XMLParser::end(const XML_Char* el)
{
	if ( parent_->hasChildNodes() )
		parent_->isAlone(false);
	parent_ = (Element*)(parent_->parentNode().get());
}


//////////////////////////////////////////////////////////////////////
//Document impl
//////////////////////////////////////////////////////////////////////



Document::Document()
	: root_(ElementPtr( new Element()))
{
	//root_.document_ = this;
	//root_.parent_   = 0;
	root_->nodeType(Node::ELEMENT);

//	encoding      = "UTF-8";
//	version       = "1.0";
//	standalone    = "no";
}

Document::~Document()
{
}

XMLParser Document::getParser()
{
	 XMLParser parser;
	return parser;
}


ElementPtr  Document::documentElement()
{
    return root_;
}

void  Document::documentElement(ElementPtr r)
{
	root_ = r;
}

std::string Document::toString()
{
	//std::string ret = "<?xml version=\"" + version + "\" encoding=\"" + encoding + "\" standalone=\"" + standalone + "\" ?>\r\n";
	return root_->innerXml();
}

Element* Document::parse( const std::string& src )
{
    clear();
    return getParser().parse(shared_from_this(),src);
}

Element* Document::parse( Element* root, const std::string& src )
{
    return getParser().parse(shared_from_this(),root,src);
}


NodePtr Document::createNode( Node::NodeType t, NodePtr parent, const std::string& name, const std::string& value )
{
    if ( (name.size() == 0) && (value.size() == 0) )
	{
        return NodePtr();
	}
	return NodePtr(new Node( shared_from_this(), parent, t, name, value));
}

ElementPtr Document::createElement( const std::string& name)
{
    if ( name.size() == 0 )
	{
        return ElementPtr();
	}
	return ElementPtr(new Element( shared_from_this(), name ));
}

ElementPtr Document::createElementNS( const std::string& name, const std::string& ns)
{
    if ( name.size() == 0 )
	{
        return ElementPtr();
	}

	ElementPtr e = ElementPtr(new Element( shared_from_this(), name ));
	e->setAttribute("xmlns",ns);
	return e;
}

TextPtr Document::createTextNode( const std::string& value)
{
    if ( value.size() == 0 )
	{
        return TextPtr();
	}

	return TextPtr(new Text( shared_from_this(), value ));
}

void Document::clear()
{
	root_->clear();
}

void Document::clear( ElementPtr el)
{
	el->childNodes()->nodes_.clear();
}

}}

