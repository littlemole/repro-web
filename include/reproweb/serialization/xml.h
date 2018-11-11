#ifndef _MOL_DEF_GUARD_DEFINE_REPROWEB_SERIALIZER_XML_XXX_DEF_GUARD_
#define _MOL_DEF_GUARD_DEFINE_REPROWEB_SERIALIZER_XML_XXX_DEF_GUARD_


#include <reproweb/traits.h>
#include <reproweb/xml/document.h>
#include <reproweb/serialization/meta.h>
#include <reproweb/serialization/parameter.h>
#include "reproweb/ctrl/front_controller.h"


namespace reproweb {

//////////////////////////////////////////////////////////////

template<class T>
struct xml_t
{
	T value;

	T* operator->()
	{
		return &value;
	}

	T& operator*()
	{
		return value;
	}
};


template<class T>
using async_xml_t = repro::Future<xml_t<T>>;


template<class T>
auto xml_promise()
{
	return repro::promise<xml_t<T>>();
}


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


template<class T>
void toXml( const T& from, xml::ElementPtr to);

template<class T>
void toXml( const std::vector<T>& from, xml::ElementPtr to);


inline void toXml( const std::string& from, xml::ElementPtr to)
{
	xml::TextPtr txt = to->ownerDocument()->createTextNode(from);
	to->appendChild(txt);
}

inline void toXml( const int& from, xml::ElementPtr to)
{
	std::ostringstream oss;
	oss << from;
	toXml(oss.str(),to);
}

inline void toXml( const prio::HeaderValues& from, xml::ElementPtr to)
{
	size_t size = from.size();
	for( size_t i = 0; i < size; i++)
	{
		std::string s = from[i].main();
		toXml(s,to);
	}
}

inline void toXml( const char* n,  std::string from, xml::ElementPtr to)
{
	xml::ElementPtr el = to->ownerDocument()->createElement(n);
	to->appendChild(el);
	if(!from.empty())
	{
		xml::TextPtr txt = to->ownerDocument()->createTextNode(from);
		el->appendChild(txt);
	}
}

inline void toXml( const char* n,  int from, xml::ElementPtr to)
{
	std::ostringstream oss;
	oss << from;

	std::string tmp = oss.str();

	xml::ElementPtr el = to->ownerDocument()->createElement(n);
	to->appendChild(el);

	if(!tmp.empty())
	{
		xml::TextPtr txt = to->ownerDocument()->createTextNode(oss.str());
		el->appendChild(txt);
	}
}

inline void toXml( const prio::Cookie& cookie, xml::ElementPtr el)
{
	toXml("name",cookie.name(),el);
	toXml("value",cookie.value(),el);
	toXml("expires",cookie.expires(),el);
	toXml("maxAge",cookie.maxAge(),el);
	toXml("domain",cookie.domain(),el);
	toXml("path",cookie.path(),el);
	toXml("isSecure",cookie.isSecure(),el);
}


inline void fromXml( xml::ElementPtr from, std::string& to)
{
	to = from->innerXml();
}

inline void fromXml( xml::ElementPtr from, int& to)
{
	std::istringstream iss(from->innerXml());
	iss >> to;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


template<class T>
void fromXml( xml::ElementPtr from, T& to);

template<class T>
void fromXml( xml::ElementPtr from, std::vector<T>&  to);

///////////////////////////////////////////////////////////////////////////////////////////

class XmlSerializer
{
public:

    static void serialize( const char* name, const std::string& from, void* to ) 
    {
		xml::ElementPtr xmlTo = *((xml::ElementPtr*) to);

		if(name[0] == '@')
		{
			xmlTo->setAttribute(name+1,from);
			return;
		}

		xml::ElementPtr el = xmlTo->ownerDocument()->createElement(name);
		xmlTo->appendChild(el);
		toXml(from,el);
    }

    static void serialize( const char* name, const int& from, void* to ) 
    {
		xml::ElementPtr xmlTo = *((xml::ElementPtr*) to);

		if(name[0] == '@')
		{
			std::ostringstream oss;
			oss << from;
			xmlTo->setAttribute(name+1,oss.str());
			return;
		}

		xml::ElementPtr el = xmlTo->ownerDocument()->createElement(name);
		xmlTo->appendChild(el);
		toXml(from,el);
    }

    template<class T>
    static void serialize( const char* name, const T& from, void* to ) 
    {
		xml::ElementPtr xmlTo = *((xml::ElementPtr*) to);

		xml::ElementPtr el = xmlTo->ownerDocument()->createElement(name);
		xmlTo->appendChild(el);
		toXml(from,el);
    }


    template<class T>
    static void serialize( const char* name, const std::vector<T>& from, void* to ) 
    {
		xml::ElementPtr xmlTo = *((xml::ElementPtr*) to);

		for( auto& f : from)
		{
			xml::ElementPtr el = xmlTo->ownerDocument()->createElement(name);
			xmlTo->appendChild(el);
			toXml(f,el);
		}
    }

    static void deserialize( const char* name, const void* from, int& to) 
    {
		xml::ElementPtr xmlFrom = *((xml::ElementPtr*) from);

		if(name[0] == '@')
		{
			std::istringstream iss(xmlFrom->attr(name+1));
			iss >> to;
			return;
		}

		xml::ElementPtr el = xmlFrom->childNodes()->getChildByName(name);
		if(el)
		{
			fromXml(el,to);
		}
    }

    static void deserialize( const char* name, const void* from, std::string& to) 
    {
		xml::ElementPtr xmlFrom = *((xml::ElementPtr*) from);

		if(name[0] == '@')
		{
			to = xmlFrom->attr(name+1);
			return;
		}

		xml::ElementPtr el = xmlFrom->childNodes()->getChildByName(name);
		if(el)
		{
			fromXml(el,to);
		}
    }

    template<class T>
    static void deserialize( const char* name, const void* from, T& to) 
    {
		xml::ElementPtr xmlFrom = *((xml::ElementPtr*) from);

		xml::ElementPtr el = xmlFrom->childNodes()->getChildByName(name);
		if(el)
		{
			fromXml(el,to);
		}
    }
	
    template<class T>
    static void deserialize( const char* name, const void* from, std::vector<T>& to) 
    {
		xml::ElementPtr xmlFrom = *((xml::ElementPtr*) from);

		to.clear();

		xml::NodeListPtr items = xmlFrom->childNodes()->getChildrenByName(name);

		int size = items->length();
		for( int i = 0; i < size; i++)
		{
			T t;
			fromXml( std::dynamic_pointer_cast<xml::Element>(items->item(i)), t);
			to.push_back(std::move(t));	
		}
    }
    
};

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

template<class T>
void toXml( const T& from, xml::ElementPtr to)
{
	const auto& m = meta_of(from);

	if(m.entity)
	{
		xml::ElementPtr el = to->ownerDocument()->createElement(m.entity);
		to->appendChild(el);		
	    m. template serialize<XmlSerializer>(from,&el);
	}
	else
	{
	    m. template serialize<XmlSerializer>(from,&to);
	}
}


template<class T>
void toXml( const std::vector<T>& from, xml::ElementPtr to)
{
	/*
	const auto& m = meta_of(from[0]);

	for( auto& f : from )
	{

	    m. template serialize<XmlSerializer>(&from,to);
	}
	*/
}

///////////////////////////////////////////////////////////////////////////////////////////

template<class T>
void fromXml( xml::ElementPtr from, T& to)
{
	const auto& m = meta_of(to);

	if(m.entity)
	{
		xml::ElementPtr el = from->childNodes()->getChildByName(m.entity);
		if(el)
		{
			m. template deserialize<XmlSerializer>(&el,to);
			return;
		}
	}
   	m. template deserialize<XmlSerializer>(&from,to);
}


template<class T>
void fromXml( xml::ElementPtr from, std::vector<T>&  to)
{
	/*
	const auto& m = meta_of(from[0]);

	for( auto& f : from )
	{

	    m. template serialize<XmlSerializer>(&from,to);
	}
	*/
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


template<class T>
std::shared_ptr<xml::Document> toXml(T& t)
{
	auto doc = xml::Document::create();
	toXml(t,doc->documentElement());

	return doc;
}


template<class T>
void fromXml(xml::DocumentPtr doc,T& t )
{
	fromXml(doc->documentElement(),t);
}


//////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////

template<class T>
class HandlerParam<xml_t<T>>
{
public:

	static xml_t<T> get(prio::Request& req,  prio::Response& res)
	{
		auto doc = xml::Document::parse_str(req.body());

		xml_t<T> t;
		fromXml(doc,t.value);
		validate(t.value);

		return t;
	}
};

//////////////////////////////////////////////////////////////


inline void output_xml(prio::Response& res,const std::string& xml)
{
	res
	.body(xml)
	.contentType("application/xml")
	.ok()
	.flush();
}

template<class T>
void output_xml(prio::Response& res, T& t)
{
	auto doc = toXml(t);
	output_xml(res, doc->toString() );
}


template<class T>
void output_xml(prio::Response& res, xml_t<T>& t)
{
	auto doc = toXml(t.value);
	output_xml(res, doc->toString() );
}


//////////////////////////////////////////////////////////////


template<class R,class C, class ... Args>
void invoke_handler(FrontController& fc, prio::Request& req,  prio::Response& res, repro::Future<xml_t<R>> (C::*fun)(Args...) )
{
	try
	{
		HandlerInvoker<xml_t<R>(C,Args...)>::invoke(req,res,fun)
		.then([&res](xml_t<R> r)
		{
			output_xml(res,r);
		})
		.otherwise([&fc,&req,&res](const std::exception& ex)
		{
			fc.handle_exception(ex, req, res);
		});		
	}
	catch(std::exception& ex)
	{
		fc.handle_exception(ex, req, res);
	}
}

//////////////////////////////////////////////////////////////

#ifdef _RESUMABLE_FUNCTIONS_SUPPORTED

template<class R,class C, class ... Args>
Async invoke_coro_handler(FrontController& fc, prio::Request& req,  prio::Response& res, repro::Future<xml_t<R>> (C::*fun)(Args...) )
{
	try
	{
		xml_t<R> r = co_await HandlerInvoker<xml_t<R>(C,Args...)>::invoke(req,res,fun);

		output_xml(res,r);
	}
	catch(std::exception& ex)
	{
		fc.handle_exception(ex, req, res);
	}

	co_await prio::nextTick();
	co_return;
}
 
#endif

}

#endif

