#ifndef _MOL_DEF_GUARD_DEFINE_REPROWEB_SERIALIZER_JSON_XML_CONEQ_DEF_GUARD_
#define _MOL_DEF_GUARD_DEFINE_REPROWEB_SERIALIZER_JSON_XML_CONEQ_DEF_GUARD_

#include <reproweb/serialization/xml.h>
#include <reproweb/serialization/json.h>

namespace reproweb {


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

template<class T>
class HandlerParam<T,typename std::enable_if<std::is_class<T>::value>::type>
{
public:

	static T get(prio::Request& req,  prio::Response& res)
	{
        T t;

        if(req.headers.content_type() == "application/xml")
        {
            auto doc = xml::Document::parse_str(req.body());
            fromXml(doc,t);
        }
        else
        {
            Json::Value json = JSON::parse(req.body());
            fromJson(json,t);
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

    if( req.headers.accept() == "applicaton/xml")
        return true;

    return false;
}

template<class T>
void output_conneq(prio::Request& req,  prio::Response& res,T& t)
{
    if(wantXml(req,res))
    {
        res.contentType("applicaton/xml");
        output_xml(res,t);
    }
    else
    {
        res.contentType("applicaton/json");
        output_json(res,t);
    }
}

//////////////////////////////////////////////////////////////

template<class R,class C, class ... Args>
void invoke_handler(FrontController& fc, prio::Request& req,  prio::Response& res, repro::Future<R> (C::*fun)(Args...) )
{
	try
	{
		HandlerInvoker<R(C,Args...)>::invoke(req,res,fun)
		.then([&res](R r)
		{
			output_conneq((res,r);
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
Async invoke_coro_handler(FrontController& fc, prio::Request& req,  prio::Response& res, repro::Future<R> (C::*fun)(Args...) )
{
	try
	{
		R r = co_await HandlerInvoker<R(C,Args...)>::invoke(req,res,fun);

		output_conneq(req,res,r);
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

