#ifndef MOL_DOC_EXPAT_DEF
#define MOL_DOC_EXPAT_DEF

#include "dom.h"

namespace reproweb {
namespace xml {

class XMLParser;

class Document : public std::enable_shared_from_this<Document>
{
friend class Element;
public:

	static DocumentPtr create()
	{
		auto doc = std::shared_ptr<Document>(new Document());
		doc->root_->document_ = doc;
		return doc;
	}


	static DocumentPtr parse_str(const std::string& src)
	{
		auto doc = Document::create();
		if(!doc->parse(src))
		{
			throw DOMException();
		}
		return doc;
	}

	virtual ~Document();

    virtual ElementPtr				documentElement();
	virtual void					documentElement(ElementPtr  r);
    virtual Element*				parse( const std::string& src );
	virtual Element*				parse( Element* root_, const std::string& src);

	virtual NodePtr		 			createNode( Node::NodeType t, NodePtr parent, const std::string& name, const std::string& value );
	virtual ElementPtr	 			createElement( const std::string& name );
	virtual ElementPtr	 			createElementNS( const std::string& name, const std::string& ns );
	virtual TextPtr	 				createTextNode( const std::string& value);

	virtual std::string				toString();

	std::string version();
	std::string encoding();
	bool standalone();

	void version(const std::string& v);
	void encoding(const std::string& e);
	void standalone(bool b);

private:

    Document();

	Document(const Document& doc);					//njet
    Document& operator=( Document& p );				//nada

protected:
	void clear();
	void clear( ElementPtr el);
    ElementPtr						root_;

	XMLParser getParser();

	std::string version_;
	std::string encoding_;
	bool standalone_;
};

}}

#endif


