#ifndef _MOL_DEF_GUARD_DEFINE_REPROWEB_SERIALIZER_XML_XXX_DEF_GUARD_
#define _MOL_DEF_GUARD_DEFINE_REPROWEB_SERIALIZER_XML_XXX_DEF_GUARD_


#include <reproweb/xml/document.h>
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
void toXml( const T& from, xml::ElementPtr to, typename std::enable_if<std::is_class<T>::value>::type* = nullptr);

template<class T>
void toXml( const std::vector<T>& from, xml::ElementPtr to);


inline void toXml( const char* n,  std::string from, xml::ElementPtr to)
{
	if(!n) return;

	if(n[0] == '@')
	{
		to->setAttribute(n+1,from);
		return;
	}	

	xml::ElementPtr el = to->ownerDocument()->createElement(n);
	if(el)
	{
		to->appendChild(el);
	}
	if(!from.empty())
	{
		xml::TextPtr txt = to->ownerDocument()->createTextNode(from);
		if(txt)
		{
			el->appendChild(txt);
		}
	}
}

template<class T>
void toXml( const char* n,  T from, xml::ElementPtr to, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr)
{
	if(!n) return;

	if(n[0] == '@')
	{
		std::ostringstream oss;
		oss << from;
		to->setAttribute(n+1,oss.str());
		return;
	}	

	std::ostringstream oss;
	oss << from;

	std::string tmp = oss.str();

	xml::ElementPtr el = to->ownerDocument()->createElement(n);
	if(el)
	{
		to->appendChild(el);
	}

	if(!tmp.empty())
	{
		xml::TextPtr txt = to->ownerDocument()->createTextNode(oss.str());
		if(txt)
		{
			el->appendChild(txt);
		}
	}
}

template<class T>
void toXml( const char* n, const T& from, xml::ElementPtr to, typename std::enable_if<std::is_class<T>::value>::type* = nullptr );

template<class T>
void toXml( const char* n, const std::vector<T>& from, xml::ElementPtr to )
{
	for( auto& f : from)
	{
		toXml(n,f,to);				
	}
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


inline void fromXml( const char* name, xml::ElementPtr from, std::string& to)
{
	//to = from->innerXml();

	if(!name)
	{
		to = from->innerXml();
		return;
	}

	if(name[0] == '@')
	{
		to = from->attr(name+1);
		return;
	}

	xml::ElementPtr el = from->childNodes()->getChildByName(name);
	if(el)
	{
		to = el->innerXml();
	}	
}

template<class T>
void fromXml(const char* name, xml::ElementPtr from, T& to, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr)
{
	if(!name)
	{
		std::istringstream iss(from->innerXml());
		iss >> to;
		return;
	}

	if(name[0] == '@')
	{
		std::istringstream iss(from->attr(name+1));
		iss >> to;
		return;
	}

	xml::ElementPtr el = from->childNodes()->getChildByName(name);
	if(el)
	{
		std::istringstream iss(from->innerXml());
		iss >> to;
	}		
}



template<class T>
void fromXml( const char* name, xml::ElementPtr from, T& to, typename std::enable_if<std::is_class<T>::value>::type* = nullptr);

template<class T>
void fromXml( const char* name, xml::ElementPtr from, std::vector<T>&  to);

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


template<class T>
void toXml( const char* n, const T& from, xml::ElementPtr to, typename std::enable_if<std::is_class<T>::value>::type*  )
{
	const auto& m = meta_of(from);
	
	if(n)
	{
		xml::ElementPtr el = to->ownerDocument()->createElement(n);
		to->appendChild(el);		

		auto visitor = [&from, &el]( const char* name, auto& m)
		{	
			toXml(name, m.get(from), el);
		};
		m.visit(from,visitor);		   
	}
	else
	{
		auto visitor = [&from, &to]( const char* name, auto& m)
		{	
			toXml(name, m.get(from), to);
		};
		m.visit(from,visitor);		
	}
}
template<class T>
void toXml( const T& from, xml::ElementPtr to, typename std::enable_if<std::is_class<T>::value>::type* )
{
	const auto& m = meta_of(from);

	toXml( m.entity, from, to);
}



///////////////////////////////////////////////////////////////////////////////////////////

template<class T>
void fromXml( const char* name, xml::ElementPtr from, T& to, typename std::enable_if<std::is_class<T>::value>::type* )
{
	const auto& m = meta_of(to);

	auto el = from;

	if(name)
	{
		el = from->childNodes()->getChildByName(name);
	}

	auto visitor = [&el, &to]( const char* name, auto& m)
	{	
		using value_t = std::remove_reference_t<typename std::remove_reference_t<decltype(m)>::value_t>;
		value_t value;

		fromXml(name,el,value);
		m.set(to,value);
	};
	m.visit(to,visitor);	
}

template<class T>
void fromXml( xml::ElementPtr from, T& to, typename std::enable_if<std::is_class<T>::value>::type* = nullptr )
{
	const auto& m = meta_of(to);

	if(m.entity)
	{
		fromXml(m.entity,from,to);
		return;
	}

	auto visitor = [&from, &to]( const char* name, auto& m)
	{	
		using value_t = std::remove_reference_t<typename std::remove_reference_t<decltype(m)>::value_t>;
		value_t value;

		fromXml(name,from,value);
		m.set(to,value);
	};
	m.visit(to,visitor);	

}

template<class T>
void fromXml( const char* name, xml::ElementPtr from, std::vector<T>&  to)
{
	to.clear();

	xml::NodeListPtr items = from->childNodes()->getChildrenByName(name);

	int size = items->length();
	for( int i = 0; i < size; i++)
	{
		T t;
		fromXml( 0, std::dynamic_pointer_cast<xml::Element>(items->item(i)), t);
		to.push_back(std::move(t));	
	}
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



template<class T>
void fromXml(const std::string& xml, T& t )
{
	auto doc = xml::Document::parse_str(xml);
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

#ifndef _RESUMABLE_FUNCTIONS_SUPPORTED

template<class R,class C, class ... Args>
Async invoke_handler(FrontController& fc, prio::Request& req,  prio::Response& res, repro::Future<xml_t<R>> (C::*fun)(Args...) )
{
	auto p = repro::promise();

	try
	{
		C& c = prepare_controller<C>(req);
		(c.*fun)(HandlerParam<Args>::get(req,res)...)			
		.then([p,&res](xml_t<R> r)
		{
			output_xml(res,r);
			p.resolve();
		})
		.otherwise([p,&fc,&req,&res](const std::exception& ex)
		{
			fc.handle_exception(ex, req, res);
			p.resolve();
		});		
	}
	catch(std::exception& ex)
	{
		fc.handle_exception(ex, req, res);
		prio::nextTick([p]()
		{
			p.resolve();
		});
	}

	return p.future();
}

#else

template<class R,class C, class ... Args>
Async invoke_handler(FrontController& fc, prio::Request& req,  prio::Response& res, repro::Future<xml_t<R>> (C::*fun)(Args...) )
{
	try
	{
		C& c = prepare_controller<C>(req);
		xml_t<R> r = (c.*fun)(HandlerParam<Args>::get(req,res)...);	
		output_xml(res,r);
	}
	catch(std::exception& ex)
	{
		fc.handle_exception(ex, req, res);
	}

	(void)(co_await prio::nextTick());
	co_return;
}

#endif

}

#endif

