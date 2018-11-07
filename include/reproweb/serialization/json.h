#ifndef _MOL_DEF_GUARD_DEFINE_REPROWEB_SERIALIZER_JSON_DEF_GUARD_
#define _MOL_DEF_GUARD_DEFINE_REPROWEB_SERIALIZER_JSON_DEF_GUARD_

//#include "reprocpp/ex.h"

//#include <functional>
//#include <set>
//#include <priocpp/api.h>
//#include "reproweb/ctrl/front_controller.h"
//#include <cryptoneat/cryptoneat.h>
//#include <priohttp/queryparams.h>
//#include <priohttp/arg.h>
//#include <priohttp/response.h>
#include <reproweb/json/json.h>
#include <reproweb/serialization/meta.h>
#include <reproweb/serialization/parameter.h>
#include "reproweb/ctrl/front_controller.h"

namespace reproweb {

///////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


inline Json::Value toJson( Json::Value& json)
{
	return json;
}

inline Json::Value toJson(  const std::string& t)
{
    Json::Value result(t);
    return result;
}


inline Json::Value toJson( const int& t)
{
    Json::Value result(t);
    return result;
}

inline Json::Value toJson( prio::Headers& headers)
{
	Json::Value result(Json::objectValue);

	for( auto& h : headers.raw() )
	{
		result[h.first] = headers.get(h.first);
	}

	return result;
}

inline Json::Value toJson( const prio::HeaderValue& header)
{
	Json::Value result(Json::objectValue);

	std::map<std::string,std::string> h = header.params();

	for ( auto& m : h)
	{
		result[m.first] = toJson(m.second);
	}

	return result;
}


inline Json::Value toJson(  const prio::HeaderValues& headers)
{
	Json::Value result(Json::objectValue);

	unsigned int size = headers.size();
	for ( unsigned int i = 0; i < size; i++)
	{
		result[headers[i].main()] = toJson(headers[i]);
	}

	return result;
}

inline Json::Value toJson( prio::QueryParams& qp)
{
	Json::Value result(Json::objectValue);

	for( auto& k : qp.keys() )
	{
		result[k] = qp.get(k);
	}

	return result;
}

inline Json::Value toJson( prio::Args& args)
{
	Json::Value result(Json::objectValue);

	for( auto& k : args.keys() )
	{
		result[k] = args.get(k);
	}

	return result;
}


inline Json::Value toJson(  const prio::Cookie& cookie)
{
	Json::Value result(Json::objectValue);

	result["name"] = cookie.name();
	result["value"] = cookie.value();
	result["expires"] = cookie.expires();
	result["maxAge"] = cookie.maxAge();
	result["domain"] = cookie.domain();
	result["path"] = cookie.path();
	result["isSecure"] = cookie.isSecure();

	return result;
}

template<class T>
Json::Value toJson(const T& t);

template<class T>
Json::Value toJson(const std::vector<T>& t)
{
    Json::Value result(Json::arrayValue);

    for ( auto& i : t)
    {
        result.append(toJson(i));
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

inline void fromJson( const Json::Value& from, std::string& t)
{
    t = from.asString();
}


inline void fromJson( const Json::Value& from, int& t)
{
    t = from.asInt();
}

inline void fromJson( prio::Cookie& cookie, Json::Value& json )
{
	cookie.name(json["name"].asString());
	cookie.value(json["value"].asString());
	cookie.expires(json["expires"].asString());
	cookie.maxAge(json["maxAge"].asInt());
	cookie.domain(json["domain"].asString());
	cookie.path(json["path"].asString());

	if(json["isSecure"].asBool())
	{
		cookie.isSecure();
	}
}


template<class T>
void fromJson(const Json::Value&, T& t);

template<class T>
void fromJson(const Json::Value& from, std::vector<T>& v)
{
	v.clear();
    unsigned int size = from.size();
    for ( unsigned int i = 0; i < size; i++)
    {
        T t;
        fromJson( from[i], t);
        v.push_back(std::move(t));
    }
}

///////////////////////////////////////////////////////////////////////////////////////////

class JsonSerializer
{
public:

    template<class T>
    static void serialize( const char* name, const T& from, void* to ) 
    {
        Json::Value& json = *( (Json::Value*)to);

        json[name] = toJson(from);
    }


    template<class T>
    static void deserialize( const char* name, const void* from, T& to) 
    {
        const Json::Value& json = *( (const Json::Value*)from);

        fromJson( json[name], to);
    }
    
};

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////



template<class T>
Json::Value toJson(const T& t)
{
    const auto& m = meta_of(t);

    Json::Value result(Json::objectValue);

	if(m.entity)
	{
		result[m.entity] = Json::Value(Json::objectValue);
	    m. template serialize<JsonSerializer>(t,&(result[m.entity]));
	}
	else
	{
	    m. template serialize<JsonSerializer>(t,&result);
	}

    return result;
}


///////////////////////////////////////////////////////////////////////////////////////////


template<class T>
void fromJson(const Json::Value& from, T& t)
{
    const auto& m = meta_of(t);

	if(m.entity)
	{
		if ( from.isMember(m.entity))
		{
			Json::Value member = from[m.entity];
			m. template deserialize<JsonSerializer>(&member,t);
		}
		return;
	}

	m. template deserialize<JsonSerializer>(&from,t);
}

//////////////////////////////////////////////////////////////

template<class T>
class HandlerParam<Entity<T>>
{
public:

	static Entity<T> get(prio::Request& req,  prio::Response& res)
	{
		Json::Value json = JSON::parse(req.body());

		Entity<T> t;
		fromJson(json,t.value);
		validate(t.value);

		return t;
	}
};

//////////////////////////////////////////////////////////////

template<>
class HandlerParam<Json::Value>
{
public:

	static Json::Value get(prio::Request& req,  prio::Response& res)
	{
		Json::Value json = JSON::parse(req.body());

		return json;
	}
};

///////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

inline void output_json(prio::Response& res,Json::Value json)
{
	res
	.body(JSON::flatten(json))
	.contentType("application/json")
	.ok()
	.flush();
}

template<class T>
void output_json(prio::Response& res, T& t)
{
	output_json(res, toJson(t) );
}


template<class T>
void output_json(prio::Response& res, Entity<T>& t)
{
	output_json(res, toJson(t.value) );
}

//////////////////////////////////////////////////////////////

template<class R,class C, class ... Args>
void invoke_handler(FrontController& fc, prio::Request& req,  prio::Response& res, repro::Future<Entity<R>> (C::*fun)(Args...) )
{
	try
	{
		HandlerInvoker<Entity<R>(C,Args...)>::invoke(req,res,fun)
		.then([&res](Entity<R> r)
		{
			output_json(res,r);
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


template<class C, class ... Args>
void invoke_handler(FrontController& fc, prio::Request& req,  prio::Response& res, repro::Future<Json::Value> (C::*fun)(Args...) )
{
	try
	{
		HandlerInvoker<Json::Value(C,Args...)>::invoke(req,res,fun)
		.then([&res](Json::Value r)
		{
			output_json(res,r);
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
Async invoke_coro_handler(FrontController& fc, prio::Request& req,  prio::Response& res, repro::Future<Json::Value> (C::*fun)(Args...) )
{
	try
	{
		Json::Value r = co_await HandlerInvoker<Entity<R>(C,Args...)>::invoke(req,res,fun);

		output_json(res,r);
	}
	catch(std::exception& ex)
	{
		fc.handle_exception(ex, req, res);
	}

	co_await prio::nextTick();
	co_return;
}

template<class R,class C, class ... Args>
Async invoke_coro_handler(FrontController& fc, prio::Request& req,  prio::Response& res, repro::Future<Entity<R>> (C::*fun)(Args...) )
{
	try
	{
		Entity<R> r = co_await HandlerInvoker<Entity<R>(C,Args...)>::invoke(req,res,fun);

		output_json(res,r);
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

