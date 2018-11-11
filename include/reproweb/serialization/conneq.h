#ifndef _MOL_DEF_GUARD_DEFINE_REPROWEB_SERIALIZER_JSON_XML_CONEQ_DEF_GUARD_
#define _MOL_DEF_GUARD_DEFINE_REPROWEB_SERIALIZER_JSON_XML_CONEQ_DEF_GUARD_

#include <reproweb/serialization/xml.h>
#include <reproweb/serialization/json.h>

namespace reproweb {

//////////////////////////////////////////////////////////////

template<class T>
struct entity
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
using async_t = repro::Future<entity<T>>;

template<class T>
auto async_entity()
{
	return repro::promise<entity<T>>();
}


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

template<class T>
class HandlerParam<entity<T>,typename std::enable_if<std::is_class<T>::value>::type>
{
public:

	static entity<T> get(prio::Request& req,  prio::Response& res)
	{
        entity<T> t;

        if(req.headers.content_type() == "application/xml")
        {
            auto doc = xml::Document::parse_str(req.body());
            fromXml(doc,t.value);
        }
        else
        {
            Json::Value json = JSON::parse(req.body());
            fromJson(json,t.value);
        }

        validate(t);
		return t;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////

inline bool wantXml(prio::Request& req,  prio::Response& res)
{
    if ( res.headers.content_type() == "application/xml")
        return true;

    if( req.headers.accept() == "application/xml")
        return true;

    return false;
}

template<class T>
void output_conneq(prio::Request& req,  prio::Response& res,T& t)
{
    if(wantXml(req,res))
    {
        res.contentType("application/xml");
        output_xml(res,t);
    }
    else
    {
        res.contentType("application/json");
        output_json(res,t);
    }
}

//////////////////////////////////////////////////////////////

template<class R,class C, class ... Args>
void invoke_handler(FrontController& fc, prio::Request& req,  prio::Response& res, repro::Future<entity<R>> (C::*fun)(Args...) )
{
	try
	{
		HandlerInvoker<entity<R>(C,Args...)>::invoke(req,res,fun)
		.then([&req,&res](entity<R> r)
		{
			output_conneq(req,res,r.value);
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
Async invoke_coro_handler(FrontController& fc, prio::Request& req,  prio::Response& res, repro::Future<entity<R>> (C::*fun)(Args...) )
{
	try
	{
		entity<R> r = co_await HandlerInvoker<entity<R>(C,Args...)>::invoke(req,res,fun);

		output_conneq(req,res,r.value);
	}
	catch(std::exception& ex)
	{
		fc.handle_exception(ex, req, res);
	}

	co_await prio::nextTick();
	co_return;
}

#endif


///////////////////////////////////////////////////////////////////////////////////////////

}

#endif

