#include "reproweb/xml/dom.h"
#include <sstream>
#include "reproweb/xml/document.h"

namespace reproweb {
namespace xml {


std::string xmlentities_encode(const std::string& in )
{
    std::ostringstream out;
    size_t p = 0;
    size_t len = in.size();
    while( ( p < len ) )
    {
        switch ( in[p] )
        {
            case '&' :
            {
                out << "&amp;";
                break;
            }
            case '<' :
            {
                out << "&lt;";
                break;
            }
            case '>' :
            {
                out << "&gt;";
                break;
            }
            default :
            {
                out << in[p];
                break;
            }
        }
        p++;
    }
    return out.str();
}

std::string xmlentities_decode( const std::string& str )
{
    std::ostringstream out;
    size_t len = str.size();
    for ( size_t i = 0; i < len; i++ )
    {
        if ( str[i] == '&' )
        {
			if ( str.substr(i,4) == "&lt;" )
            {
				out << "<";
                i+=3;
            }
            else
			if ( str.substr(i,4) == "&gt;" )
            {
                out << ">";
                i+=3;
            }
            else
	        if ( str.substr(i,5) == "&amp;" )
            {
                out << "&";
                i+=4;
            }
		}
        else
        {
			out << str[i];
        }
    }
    return out.str();
}



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// Node
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

Node::Node  (DocumentPtr pD,  NodePtr p, Node::NodeType t, const std::string& name, const std::string& value)
    : attribs_(new NamedNodeMap(this))
{
    init();
    parent_    = p;
    document_  = pD;
    nodename_  = name;
    nodevalue_ = xmlentities_encode(value);
    type_      = t;
}

Node* Node::cloneNode()
{
    Node* n = new Node;
    *n = *this;
    return n;
}

Node& Node::operator=( Node& n )
{
    clear();

    // NO! Doc stays intact
    //_pDoc		= n._pDoc;
    //_parent	= n._parent;
    nodevalue_  = n.nodevalue_;
    type_	   = n.type_;
    nodename_   = n.nodename_;
    isalone_	   = n.isalone_;

    for ( int i = 0; i < n.childNodes()->length(); i++)
    {
        Node* c = n.childNodes()->item(i)->cloneNode();
        appendChild(NodePtr(c));
    }
    for ( int i = 0; i < n.attributes()->length(); i++)
    {
        NodePtr a = n.attributes()->item(i);
        attributes()->add( a->nodeName(), a->nodeValue() );
    }
    return *this;
}

// erases children_
void Node::clear()
{
    children_->clear();
}

void Node::init()
{
    isalone_	= true;
    document_.reset();
    parent_.reset();
    nodename_	= "";
    nodevalue_	= "";

    //attribs_ = std::make_shared<NamedNodeMap>();
    //children_ = std::make_shared<NodeList>();
}

DocumentPtr  Node::ownerDocument()
{
    return document_.lock();
}



const std::string& Node::nodeName()
{
    return nodename_;
}

void Node::nodeName(const std::string& s )
{
    nodename_ = s;
}

std::string Node::nodeValue()
{
    return xmlentities_decode(nodevalue_);
}

void Node::nodeValue(const std::string& s )
{
    nodevalue_ = xmlentities_encode(s);
}

Node::NodeType Node::nodeType()
{
    return type_;
}

void Node::nodeType(NodeType t)
{
    type_ = t;
}

bool Node::isAlone()
{
    return isalone_;
}

void Node::isAlone(bool b)
{
    isalone_ = b;
}

std::string	Node::prefix()
{
    size_t pos = nodename_.find(":");
    if ( (pos != std::string::npos ) && ( pos > 0) )
        return nodename_.substr(0,pos);
    return "";
}

std::string	Node::localname()
{
    size_t pos = nodename_.find(":");
    if ( (pos != std::string::npos ) && ( pos > 0) )
        return nodename_.substr(pos+1);
    return nodename_;
}

std::string	Node::namespaceURI()
{
    if ( prefix() == "" )
        return defaultNamespace();

    return getNSfromPrefix(prefix());
}

NodePtr  Node::parentNode()
{
    return parent_.lock();
}

std::string  Node::getNSfromPrefix(const std::string& prefix)
{
    NodePtr parent_ = parentNode();
    if ( parent_ )
        return parent_->getNSfromPrefix(prefix);

    return "";
}

std::string  Node::getPrefixFromNS(const std::string& ns)
{
    NodePtr parent_ = parentNode();
    if ( parent_ )
        return parent_->getPrefixFromNS(ns);

    return "";
}
std::string  Node::defaultNamespace()
{
    NodePtr parent_ = parentNode();
    if ( parent_ )
        return parent_->defaultNamespace();

    return "";
}



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// Attr
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


Attr::Attr()
{
    type_		 = Node::ATTRIBUTE;
    nodename_    = "";
    nodevalue_	 = "";
    quote        = 0;
}

Attr::Attr( const std::string& name, const std::string& value )
{
    type_		 = Node::ATTRIBUTE;
    nodename_    = name;
    nodevalue_	 = xmlentities_encode(value);
    quote        = 0;
}

Attr::~Attr()
{}

Node* Attr::cloneNode()
{
    Attr* attribute = new Attr;
    *attribute = *this;
    return attribute;
}


Attr& Attr::operator=( const Attr& a )
{
    nodename_    = a.nodename_;
    nodevalue_	 = a.nodevalue_;
    type_		 = a.type_;
    quote        = a.quote;
    return *this;
}

std::string Attr::text()
{
    std::string ret = nodename_;
    if ( (nodevalue_ != "") || quote )
    {
        ret += "=";
    }
    if ( quote )
        ret += quote;
    ret += nodevalue_;
    if ( quote )
        ret += quote;
    return ret;
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// NamedNodeMap
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

NamedNodeMap::NamedNodeMap(Node* p) : parent_(p) {};

NamedNodeMap::~NamedNodeMap()
{
    clear();
}

void NamedNodeMap::clear ()
{
    /*
    for ( std::list<AttrPtr>::iterator it = attributes_.begin();
        it != attributes_.end(); it++)
    {
        delete *it;
    }
    */
    attributes_.clear();
}

AttrPtr NamedNodeMap::item( const std::string& index, const std::string& ns )
{
    for ( std::list<AttrPtr>::iterator it = attributes_.begin();
        it != attributes_.end(); it++)
    {
        if ( (*it)->localname() == index )
            if ( parent_->getNSfromPrefix((*it)->prefix()) == ns )
                return (*it);
    }
    return AttrPtr();
}

AttrPtr NamedNodeMap::item(const std::string& index)
{
    for ( std::list<AttrPtr>::iterator it = attributes_.begin();
        it != attributes_.end(); it++)
    {
        if ( (*it)->nodeName() == index )
            return (*it);
    }
    return AttrPtr();
}

AttrPtr NamedNodeMap::item(int index)
{
    int i = 0;
    for ( std::list<AttrPtr>::iterator it = attributes_.begin();
        it != attributes_.end(); it++)
    {
        if ( i == index )
            return (*it);
        i++;
    }
    return AttrPtr();
}

int NamedNodeMap::length()
{
    return (int)attributes_.size();
}

AttrPtr NamedNodeMap::operator[](const std::string& index)
{
    return item(index);
}

void NamedNodeMap::add(const std::string& name, const std::string& value )
{
    AttrPtr att = AttrPtr(new Attr(name,value));
    attributes_.push_back(att);
}

void NamedNodeMap::add(const std::string& name)
{
    add( name, "");
}

void NamedNodeMap::erase( const std::string& name )
{
    for ( std::list<AttrPtr>::iterator it = attributes_.begin();
        it != attributes_.end(); it++)
    {
        if ( (*it)->nodeName() == name )
        {
            attributes_.erase(it);
            return;
        }
    }
}

void NamedNodeMap::erase( int index )
{
    int i = 0;
    for ( std::list<AttrPtr>::iterator it = attributes_.begin();
        it != attributes_.end(); it++)
    {
        if ( i == index )
        {
            attributes_.erase(it);
            return;
        }
        i++;
    }
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// Element
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


Element::Element  () 
{
    init(); 
}

Element::Element  (DocumentPtr pD, const std::string& name)
{
    init();
    document_  = pD;
    nodename_  = name;
    nodevalue_ = "";
    type_      = Node::ELEMENT;
}

Element::Element  (DocumentPtr pD,  ElementPtr p, const std::string& name)
{
    init();
    parent_    = p;
    document_  = pD;
    nodename_  = name;
    nodevalue_ = "";
    type_      = Node::ELEMENT;
}

Element::Element  (DocumentPtr pD,  ElementPtr p, Node::NodeType t, const std::string& name)
{
    init();
    parent_    = p;
    document_  = pD;
    nodename_  = name;
    nodevalue_ = "";
    type_      = t;
}

Element::~Element ()
{
}

Node* Element::cloneNode()
{
    Element* element = new Element;
    *element = *this;
    return element;
}

Element& Element::operator=(  Element& n )
{
    clear();

    // NO! Doc stays intact
    //_pDoc		= n._pDoc;
    //_parent	= n._parent;
    nodevalue_  = n.nodevalue_;
    type_	   = n.type_;
    nodename_   = n.nodename_;
    isalone_	   = n.isalone_;

    for ( int i = 0; i < n.childNodes()->length(); i++)
    {
        Node* c = n.childNodes()->item(i)->cloneNode();
        appendChild(NodePtr(c));
    }
    for ( int i = 0; i < n.attributes()->length(); i++)
    {
        NodePtr a = n.attributes()->item(i);
        attributes()->add( a->nodeName(), a->nodeValue() );
    }
    return *this;
}


bool Node::hasChildNodes()
{
    return children_->length() > 0;
}

bool Node::hasAttributes()
{
    return attribs_->length() > 0;
}

NodePtr  Node::nextSibling()
{
    NodePtr p = parent_.lock();    

    if (!p)
        return NodePtr();


    NodeListPtr nodes_ = p->childNodes();
    if ( !nodes_ )
        return NodePtr();

    int len = nodes_->length();
    for ( int i = 0; i < len; i++)
    {
        if ( nodes_->item(i).get() == this )
        {
            if ( i+1 < len )
            {
                return nodes_->item(i+1);
            }
        }
    }
    return NodePtr();
}

NodePtr  Node::previousSibling()
{
    NodePtr p = parent_.lock();    

    if (!p)
        return NodePtr();

    NodeListPtr nodes_ = p->childNodes();

    if (!nodes_)
        return NodePtr();

    if ( nodes_->item(0).get() == this )
        return NodePtr();

    int len = nodes_->length();
    for ( int i = 1; i < len; i++)
    {
        if ( nodes_->item(i).get() == this )
        {
            return nodes_->item(i-1);
        }
    }
    return NodePtr();
}

NodePtr  Node::firstChild()
{
    if ( children_->length() == 0 )
        return NodePtr();
        
    return children_->item(0);
}

NodePtr  Node::lastChild()
{
    if ( children_->length() == 0 )
        return NodePtr();	

    return children_->item(children_->length()-1);
}

NodeListPtr Node::childNodes()
{
    return children_;
}

void Node::appendChild( NodePtr n )
{

    if ( n->nodeType() == Node::UNDEFINED )
        return;

    n->document_  = document_;
    n->parent_    = shared_from_this();
    children_->add(n);
}

void Node::removeChild(  NodePtr n )
{
    children_->erase(n);
}

void Node::replaceChild( NodePtr oldElement, NodePtr newElement )
{
    int len = children_->length();
    for ( int i = 0; i < len ; i++ )
    {
        if ( children_->item(i).get() == oldElement.get() )
        {
            *oldElement = *newElement;
        }
    }
}

void Node::insertBefore( NodePtr beforeElement, NodePtr n )
{
    std::vector<NodePtr>::iterator element = children_->nodes_.begin();
    std::vector<NodePtr>::iterator it = element;
    it++;

    for ( it; it != children_->nodes_.end(); it++ )
    {
        if ( (*it).get() == beforeElement.get() )
        {
            n->document_ = document_;
            n->parent_   = shared_from_this();
            children_->nodes_.insert(it, n );
            return;
        }
        element++;
    }
}

void Node::insertAfter( NodePtr afterElement, NodePtr n )
{
    std::vector<NodePtr>::iterator it = children_->nodes_.begin();
    for ( it; it != children_->nodes_.end(); it++ )
    {
        if ( (*it).get() == afterElement.get() )
        {
            it++;
            n->document_ = document_;
            n->parent_   = shared_from_this();
            children_->nodes_.insert(it, n );
            return;
        }
    }
}
void  Element::removeAttribute(const std::string& a )
{
    attribs_->erase(a);
}

void Element::setAttribute(const std::string& key, const std::string& value )
{
    AttrPtr att = attribs_->item(key);
    if ( !att )
	{
        attribs_->add(key,value);		
	}
    else
    {
        att->nodeValue(value);
    }

	attribs_->item(key)->quote = '"';
}

std::string Element::attr(const std::string& key)
{
    AttrPtr att = attribs_->item(key);
    if ( !att )
        return "";
    else
        return att->nodeValue();
}

NamedNodeMapPtr Node::attributes()
{
    return attribs_;
}

AttrPtr Element::getAttribute(int index)
{
    return attribs_->item(index);
}

AttrPtr Element::getAttribute( const std::string& name )
{
    return attribs_->item(name);
}

std::string Element::atts()
{
    std::string ret = "";
    int len = this->attributes()->length();
    for ( int i = 0; i< len; i++ )
    {
        AttrPtr att = this->attributes()->item(i);
        ret += " " + att->text();
    }
    return ret;
}

std::string Element::innerXml()
{
    std::string ret = "";
    int len = children_->length();
    for ( int i = 0 ; i < len; i++ )
    {
        NodePtr n = children_->item(i);
        if ( n->nodeType() == Node::ELEMENT )
        {
            Element* e = (Element*)(n.get());
            ret += e->outerXml();
        }
        else
        {
            ret += n->nodeValue();
        }
    }
    return ret;
}

void Element::innerXml(const std::string& s)
{
    auto d = document_.lock();
    if ( d )
    {
        children_->clear();
        Element* root = d->parse( this, s);
    }
}

std::string Element::outerXml()
{
    std::string ret = "";
    if ( (type_ == Node::ELEMENT) )//|| (type_ == Node::SCRIPT) )
    {
        ret += "<" + nodename_;
        std::string as = atts();
        if ( as != "" )
        {
            ret += as;
        }
        bool b = true;
        if ( nodename_.size() > 0 && nodename_[0] == '/' )
            b = false;
        if ( (!hasChildNodes()) && (isAlone()) && b )//&& (document_->doctype() != mol::HTML) )
            ret += " /";
        ret += ">";
    }
    if ( (type_ == Node::TEXT)    || (type_ == Node::SCRIPT) || 
        (type_ == Node::COMMENT) || (type_ == Node::CDATA) )
    {
        ret += nodevalue_ ;
    }
    int len = children_->length();
    for ( int i = 0; i < len; i++)
    {
        ret += ((Element*)(children_->item(i).get()))->outerXml();
    }
    if ( hasChildNodes() || (!isAlone()) )
    {
        ret += "</" + nodename_ + ">";
    }
    return ret;
}

void Element::outerXml(const std::string& s)
{
    auto d = document_.lock();
    if ( d )
    {
        clear();
        Element* root = d->parse( this, s);

        auto p = parent_.lock();

        for ( int i = 0; i < children_->length(); i++ )
        {
            ((Element*)(p.get()))->insertBefore(shared_from_this(),children_->item(i));
        }

        ((Element*)(p.get()))->removeChild(shared_from_this());
    }
}

std::string	Element::startTag()
{
    std::string ret = "<" + nodename_;
    ret += atts() + ">";
    return ret;
}

std::string	Element::endTag()
{
    std::string ret = "";
    if ( isAlone() )
        return ret;
    ret = "</"+nodename_+">";
    return ret;
}

ElementPtr  Element::getElementById(const std::string& id )
{
    return getElementById("id",id);
}

ElementPtr  Element::getElementById(const std::string& idKey, const std::string& idValue )
{
    int l = children_->length();
    for ( int i = 0; i < l; i++ )
    {
        Element* n = (Element*)(children_->item(i).get());
        AttrPtr att = n->getAttribute(idKey);
        if (att)
        {
            if ( att->nodeValue() == idValue )
                return std::dynamic_pointer_cast<Element>(children_->item(i));
        }
    }
    for ( int i = 0; i < l; i++ )
    {
        NodePtr n =  children_->item(i);
        if ( n->nodeType() == Node::ELEMENT )
        {
            Element* el =  (Element*)(n.get());
            ElementPtr e = el->getElementById(idKey, idValue);
            if (e)
                return e;
        }
    }
    return ElementPtr();	
}

ElementPtr  Element::getElementByName(const std::string& name)
{
    return getElementById("name",name);
}

NodeListPtr Element::getElementsByTagName(const std::string& tag)
{
    auto nodes_ = std::make_shared<NodeList>();
    nodes_->bDetach_ =true;
    getElementsByTagNameWalker(tag,nodes_);
    return nodes_;
}

NodeListPtr Element::getElementsByTagNameQ(const std::string& tag, const std::string& ns )
{
    auto nodes_ = std::make_shared<NodeList>();
    nodes_->bDetach_ =true;
    getElementsByTagNameWalkerQ(tag,ns,nodes_);
    return nodes_;
}

ElementPtr  Element::getElementByTagName(const std::string& tag)
{
    auto nodes_ = std::make_shared<NodeList>();
    //NodeList nodes_;
    nodes_->bDetach_ =true;
    getElementsByTagNameWalker(tag,nodes_);
    if ( nodes_->length() > 0 )
        return std::dynamic_pointer_cast<Element>(nodes_->item(0));

    return ElementPtr();
}

ElementPtr Element::getElementByTagNameQ(const std::string& tag, const std::string& ns )
{
    auto nodes_ = std::make_shared<NodeList>();
    //NodeList nodes_;
    nodes_->bDetach_ =true;
    getElementsByTagNameWalkerQ(tag,ns,nodes_);
    if ( nodes_->length() > 0 )
        return std::dynamic_pointer_cast<Element>(nodes_->item(0));

    return ElementPtr();
}
void  Node::getElementsByTagNameWalker(const std::string& tag, NodeListPtr nodes_ )
{
    int l = children_->length();
    for ( int i = 0; i < l; i++ )
    {
        NodePtr n = children_->item(i);
        if ( n->nodeType() == Node::ELEMENT || n->nodeType() == Node::SCRIPT  )
        {
            Element* e = (Element*)(n.get());
            if ( e->nodeName() == tag )
                nodes_->add(n);
            if ( e->hasChildNodes() )
                e->getElementsByTagNameWalker(tag,nodes_);
        }
    }
}

void  Node::getElementsByTagNameWalkerQ(const std::string& tag, const std::string& ns, NodeListPtr nodes_ )
{
    int l = children_->length();
    for ( int i = 0; i < l; i++ )
    {
        NodePtr n = children_->item(i);
        if ( n->nodeType() == Node::ELEMENT )
        {
            Element* e = (Element*)(n.get());
            std::string name = e->localname();
            if ( (e->localname() == tag) && (e->namespaceURI() == ns) )
                nodes_->add(n);
            if ( e->hasChildNodes() )
                e->getElementsByTagNameWalkerQ(tag,ns,nodes_);
        }
    }
}

std::string  Element::getNSfromPrefix(const std::string& prefix)
{
    std::string lookup = "xmlns";
    if ( prefix != "" )
    {
        lookup += ":";
        lookup += prefix;
    }

    AttrPtr att = getAttribute(lookup);
    if ( att )
    {
        return att->nodeValue();
    }
    NodePtr parent_ = parentNode();
    if ( parent_ )
        return parent_->getNSfromPrefix(prefix);

    return "";
}

std::string  Element::getPrefixFromNS(const std::string& ns)
{
    for ( int i = 0; i < attribs_->length(); i++)
    {
        AttrPtr att = attribs_->item(i);
        std::string name =  att->nodeName();
        if ( name.size() > 5 )
            if ( name.substr(0,6) == "xmlns:" )
            {
                if ( att->nodeValue() == ns )
                {
                    return name.substr(6);
                }
            }
    }

    NodePtr parent_ = parentNode();
    if ( parent_ )
        return parent_->getPrefixFromNS(ns);
    return "";
}

std::string  Element::defaultNamespace()
{
    AttrPtr att = getAttribute("xmlns");
    if ( att )
    {
        return att->nodeValue();
    }

    NodePtr parent_ = parentNode();
    if ( parent_ )
        return parent_->defaultNamespace();
    return "";
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// Node Collection
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

NodeList::NodeList(const NodeList& n)
    : bDetach_ (false)
{
    *this = n;
}

NodeList::~NodeList()
{
    if ( bDetach_ )
        return;
    clear();
}

NodeList& NodeList::operator=(const NodeList& nl) 
{
    nodes_   = nl.nodes_;
	bDetach_ = nl.bDetach_;	
	return *this;
}

void NodeList::clear()
{
    /*
    for ( std::vector<NodePtr>::iterator it = nodes_.begin();
        it != nodes_.end(); it++)
    {
        delete *it;
    }
    */
    nodes_.clear();
}


NodePtr NodeList::item(int index)
{
    return nodes_[index];
}

int   NodeList::length() const
{
    return (int)nodes_.size();
}

NodePtr NodeList::operator[](int index)
{
    return item(index);
}

void  NodeList::add   (NodePtr n)
{
    nodes_.push_back(n);
}

void NodeList::erase(NodePtr n )
{
    for ( std::vector<NodePtr>::iterator it = nodes_.begin(); it != nodes_.end(); it++ )
    {
        if ( (*it).get() == n.get() )
        {
            nodes_.erase(it);
            return;
        }
    }
}

ElementPtr  NodeList::getChildByName(const std::string& name )
{
    int l = length();
    for ( int i = 0; i < l; i++ )
    {
        NodePtr n = item(i);
        if ( n->localname() == name )
        {
            if ( n->nodeType() == Node::ELEMENT )
                return std::dynamic_pointer_cast<Element>(n);
        }
    }
    return ElementPtr();
}

NodeListPtr  NodeList::getChildrenByName(const std::string& name )
{
    auto nodes_ = std::make_shared<NodeList>();
//    NodeList nodes_;
    nodes_->bDetach_ =true;
    int l = length();
    for ( int i = 0; i < l; i++ )
    {
        NodePtr n = item(i);
        if ( n->nodeType() == Node::ELEMENT )
        {
            if ( n->localname() == name )
                nodes_->add(n);
        }
    }
    return nodes_;
}

ElementPtr  NodeList::getChildByNameNS(const std::string& name, const std::string& ns )
{
    int l = length();
    for ( int i = 0; i < l; i++ )
    {
        NodePtr n = item(i);
        if ( (n->localname() == name) && (n->getNSfromPrefix(n->prefix()) == ns) )
        {
            if ( n->nodeType() == Node::ELEMENT )
                return std::dynamic_pointer_cast<Element>(n);
        }
    }
    return ElementPtr();
}

NodeListPtr  NodeList::getChildrenByNameNS(const std::string& name, const std::string& ns )
{
    auto nodes_ = std::make_shared<NodeList>();
    //NodeList nodes_;
    nodes_->bDetach_ =true;
    int l = length();
    for ( int i = 0; i < l; i++ )
    {
        NodePtr n = item(i);
        if ( n->nodeType() == Node::ELEMENT )
        {
            if ( (n->localname() == name) && (n->getNSfromPrefix(n->prefix()) == ns) )
                nodes_->add(n);
        }
    }
    return nodes_;
}


//////////////////////////////////////////////////////////////////////
//  TEXT
//////////////////////////////////////////////////////////////////////

// Construction/Destruction
Text::Text  ()
{
    type_  = Node::TEXT;
    nodevalue_ = "";
}

Text::Text  (DocumentPtr pD, const std::string& value)
{
    type_  = Node::TEXT;
    document_  = pD;
    nodevalue_ = xmlentities_encode(value);
}

Text::Text  (DocumentPtr pD, ElementPtr parent_, const std::string& value)
{
    type_  = Node::TEXT;
    document_  = pD;
    parent_    = parent_;
    nodevalue_ = xmlentities_encode(value);
}


}} //end namespace mol
