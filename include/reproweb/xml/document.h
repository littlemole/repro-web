#ifndef MOL_DOC_EXPAT_DEF
#define MOL_DOC_EXPAT_DEF

#include "dom.h"

namespace reproweb {
namespace xml {

class XMLParser;

class Document
{
friend class Element;
public:
    Document();
	virtual ~Document();

    virtual Element*				documentElement();
	virtual void					documentElement(Element*  r);
    virtual Element*				parse( const std::string& src );
	virtual Element*				parse( Element* root_, const std::string& src);

	virtual Node*		 			createNode( Node::NodeType t, Node* parent, const std::string& name, const std::string& value );
	virtual Element*	 			createElement( const std::string& name );
	virtual Element*	 			createElementNS( const std::string& name, const std::string& ns );
	virtual Text*	 				createTextNode( const std::string& value);

	virtual std::string				toString();
private:
	Document(const Document& doc);					//njet
    Document& operator=( Document& p );				//nada

protected:
	void clear();
	void clear( Element* el);
    Element						root_;

	XMLParser getParser();
};

}}

#endif


