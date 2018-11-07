#ifndef MOL_DOM_PAT_DEF
#define MOL_DOM_PAT_DEF

#include <map>
#include <vector>
#include <list>
#include <string>
#include <memory>
#include "reprocpp/ex.h"

namespace reproweb {
namespace xml {

///////////////////////////////////////////////////////////////////////////////

class Node;
class Attr;
class Text;
class Element;
class Document;
class NodeList;
class NamedNodeMap;

std::string xmlentities_encode(const std::string& input);
std::string xmlentities_decode(const std::string& input);

std::string xmlattr_encode(const std::string& input);
std::string xmlattr_decode(const std::string& input);

typedef std::shared_ptr<Node> NodePtr;
typedef std::shared_ptr<Attr> AttrPtr;
typedef std::shared_ptr<Node> NodePtr;
typedef std::shared_ptr<Text> TextPtr;
typedef std::shared_ptr<Element> ElementPtr;
typedef std::shared_ptr<Document> DocumentPtr;
typedef std::shared_ptr<NodeList> NodeListPtr;
typedef std::shared_ptr<NamedNodeMap> NamedNodeMapPtr;

///////////////////////////////////////////////////////////////////////////////

class DOMException : public repro::ReproEx<DOMException> {};

///////////////////////////////////////////////////////////////////////////////

class NamedNodeMap
{
friend class Document;
public:
	NamedNodeMap(Node* p);
    ~NamedNodeMap();

    int length();
	AttrPtr item(const std::string& index, const std::string& ns);
    AttrPtr item(const std::string& index);
    AttrPtr item(int index);
    AttrPtr operator[](const std::string& index);
    void add(const std::string& name, const std::string& value );
    void add(const std::string& name);
    void erase( const std::string& name );
    void erase( int index );
    void clear ();

private:
	NamedNodeMap(const NamedNodeMap&);				//njet
	NamedNodeMap& operator=(const NamedNodeMap&);	//njet
	
    std::list<AttrPtr>                  attributes_;
    Node*           					parent_;
};

///////////////////////////////////////////////////////////////////////////////

class NodeList
{
friend class Document;
friend class Node;
friend class Element;
public:
	NodeList(){};
    NodeList(const NodeList& n);
    virtual ~NodeList();

    void clear();
    NodePtr item(int index);
    int length() const;
    NodePtr operator[](int index)	;
    void add(NodePtr n);
    void erase(NodePtr n );

	ElementPtr			 getChildByName(const std::string& name );
	NodeListPtr			 getChildrenByName(const std::string& name );
	ElementPtr			 getChildByNameNS(const std::string& name, const std::string& ns );
	NodeListPtr			 getChildrenByNameNS(const std::string& name, const std::string& ns );


	NodeList& operator=(const NodeList&);

private:

    std::vector<NodePtr> nodes_;
};

///////////////////////////////////////////////////////////////////////////////

class Node : public std::enable_shared_from_this<Node> {
friend class Document;
friend class Nodelist;
friend class NamedNodeMap;

public:

	virtual ~Node() {};
    enum NodeType { UNDEFINED = 0,
					ELEMENT   = 1,
                    ATTRIBUTE = 2,
                    TEXT      = 3,
                    CDATA     = 4,	
                    ENTITYREF = 5,	// not implemented
					ENTITY	  = 6,	// not implemented
                    PI		  = 7,	// not implemented
					COMMENT   = 8,
					DOCUMENT  = 9,	// not implemented
					DOCTYPE   = 10,	// not implemented
					DOCFRAG   = 11,	// not implemented
					NOTATION  = 12,	// not implemented
					SCRIPT    = 13  // mol extension for HTML parsing
					};

	virtual Node* cloneNode();

    // DOM methods
    virtual NodeType	nodeType();
    virtual void		nodeType( NodeType t);
    virtual const		std::string&	nodeName();
    virtual void		nodeName(const std::string& s );
    virtual 			std::string	nodeValue();
    virtual void		nodeValue(const std::string& s );

	template<class T>
	T get()
	{
		T t;
		std::istringstream iss(nodeValue());
		iss >> t;
		return t;
	}

	//namespace support
	virtual std::string	prefix();
	virtual std::string	localname();
	virtual std::string	namespaceURI();

	// NS extensions
	virtual std::string  getNSfromPrefix(const std::string& prefix);
	virtual	std::string  getPrefixFromNS(const std::string& ns);
	virtual std::string  defaultNamespace();

	// DOM lookup
    virtual DocumentPtr	 ownerDocument();
	virtual NodePtr		 parentNode();

	// children_
	virtual	void		clear();
    virtual	bool		hasChildNodes();
    virtual NodePtr		nextSibling();
    virtual NodePtr		previousSibling();
    virtual NodePtr		firstChild()	;
    virtual NodePtr		lastChild();
    virtual NodeListPtr	childNodes();

    virtual void		appendChild( NodePtr n );
    virtual void		removeChild( NodePtr n );
    virtual void		replaceChild( NodePtr oldElement, NodePtr newElement );
    virtual void		insertBefore( NodePtr beforeElement, NodePtr n );
	virtual void		insertAfter( NodePtr afterElement, NodePtr n );

    // Attributes
	virtual bool			hasAttributes();
    virtual NamedNodeMapPtr	attributes();

	// has endtag?
    virtual bool	  	 isAlone(); 
	virtual void	  	 isAlone(bool b); 

protected:
    // Construction/Destruction
	Node  () :  attribs_(new NamedNodeMap(this)), children_(new NodeList) {	init(); };
	Node  (DocumentPtr pD, NodePtr parent_, Node::NodeType t, const std::string& name, const std::string& value);
	Node& operator=( Node& n );

    void init();
	void getElementsByTagNameWalker(const std::string& tag, NodeListPtr nodes_ );
	void getElementsByTagNameWalkerQ(const std::string& tag, const std::string& ns , NodeListPtr nodes_ );

	NodeType		type_;
    std::string		nodename_;
    std::string		nodevalue_;

	std::weak_ptr<Node>			parent_;
    std::weak_ptr<Document>		document_;
	NamedNodeMapPtr	attribs_;
    NodeListPtr		children_;
    bool			isalone_;
};

///////////////////////////////////////////////////////////////////////////////

class Attr : public Node
{
friend class NamedNodeMap;
public:
    Attr();
    Attr( const std::string& name, const std::string& value );
    virtual ~Attr();

	virtual Node*  cloneNode();
    Attr& operator=( const Attr& a );
	bool specified();

	ElementPtr ownerElement();

    std::string text();
    virtual 			std::string	nodeValue();
    virtual void		nodeValue(const std::string& s );


    char quote;
};

///////////////////////////////////////////////////////////////////////////////

class Element : public Node
{
friend class Document;
friend class NodeList;
public:
    virtual				~Element ();
    Element&			 operator=( Element& n );

	virtual				 Node* cloneNode();
    AttrPtr				 getAttribute(int index);
    AttrPtr				 getAttribute( const std::string& name );
    AttrPtr				 getAttributeNS( const std::string& name, const std::string& ns );
    void				 setAttribute(const std::string& key, const std::string& value );
    void				 setAttributeNS(const std::string& key, const std::string& ns, const std::string& value );
    void				 removeAttribute(const std::string& a );
	std::string			 attr(const std::string& key);

	//helpers
	ElementPtr			 getElementById( const std::string& id );
	ElementPtr			 getElementById( const std::string& key, const std::string& id );
	ElementPtr			 getElementByName( const std::string& name );
    NodeListPtr			 getElementsByTagName(const std::string& tag);
	NodeListPtr			 getElementsByTagNameQ(const std::string& tag, const std::string& ns );
    ElementPtr			 getElementByTagName(const std::string& tag);
	ElementPtr			 getElementByTagNameQ(const std::string& tag, const std::string& ns );

	virtual std::string  getNSfromPrefix(const std::string& prefix);
	virtual	std::string  getPrefixFromNS(const std::string& ns);
	virtual std::string  defaultNamespace();

    // text helpers
    std::string			 atts();
    std::string			 startTag();
    std::string			 endTag();

	// modifiers
    std::string innerXml();
    void		innerXml(const std::string& s);
    std::string outerXml();
    void		outerXml(const std::string& s);

protected:
  	// Construction/Destruction
    Element  ();
	Element  (DocumentPtr pD, const std::string& name);
	Element  (DocumentPtr pD, ElementPtr parent_, const std::string& name);
	Element  (DocumentPtr pD, ElementPtr parent_, Node::NodeType t, const std::string& name);

};

///////////////////////////////////////////////////////////////////////////////

class Text : public Node
{
	friend class Document;
public:
	virtual				~Text () {};

protected:
  	// Construction/Destruction
    Text  ();
	Text  (DocumentPtr pD, const std::string& value);
	Text  (DocumentPtr pD, ElementPtr parent_, const std::string& value);	
};

///////////////////////////////////////////////////////////////////////////////

}} // end namespace mole

#endif

