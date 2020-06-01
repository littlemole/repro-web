#ifndef _MOL_DEF_GUARD_DEFINE_REPROWEB_SERIALIZER_JSON_DEF_GUARD_
#define _MOL_DEF_GUARD_DEFINE_REPROWEB_SERIALIZER_JSON_DEF_GUARD_

#include <reproweb/json/json.h>
#include <reproweb/serialization/parameter.h>
#include <metacpp/json.h>
#include "reproweb/ctrl/front_controller.h"

namespace reproweb {


///////////////////////////////////////////////////////////////////////////////////////////

template<class T>
struct json_t
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
using async_json_t = repro::Future<json_t<T>>;

template<class T>
auto json_promise()
{
	return repro::promise<json_t<T>>();
}

///////////////////////////////////////////////////////////////////////////////////////////
/*
inline void toJson( const char* name, Json::Value& from, Json::Value& to)
{
	to[name] = from;
}

inline void toJson( const char* name,const Json::Value& from, Json::Value& to)
{
	to[name] = from;
}

inline void toJson( const char* name, const std::string& from, Json::Value& to)
{
    to[name] = from;
}

inline void toJson( const char* name, const long long from, Json::Value& to)
{
	to[name] = (Json::UInt64)from;
}


template<class T >
void toJson( const char* name, const T& from, Json::Value& to, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr)
{
	to[name] = from;
}

template<class T>
void toJson(const char* name, const T& from, Json::Value& to, typename std::enable_if<std::is_class<T>::value>::type* = nullptr);

template<class T>
void toJson(const char* name, const std::vector<T>& from, Json::Value& to)
{
    Json::Value result(Json::arrayValue);

    for ( auto& i : from)
    {
		Json::Value item(Json::objectValue);
		toJson(name,i,item);
        result.append(item[name]);
    }

    to[name] = result;
}

inline Json::Value exToJson(const std::exception& ex)
{
	Json::Value result(Json::objectValue);
	Json::Value err(Json::objectValue);

	err["type"] = typeid(ex).name();
	err["msg"] = ex.what();

	result["error"] = err;
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

inline void fromJson( const Json::Value& from, double& t)
{
    t = from.asDouble();
}

inline void fromJson( const Json::Value& from, float& t)
{
    t = from.asFloat();
}

inline void fromJson( const Json::Value& from, bool& t)
{
    t = from.asBool();
}

inline void fromJson( const Json::Value& from, long long& t)
{
    t = from.asInt64();
}

inline void fromJson( const Json::Value& from, char& t)
{
    t = from.asInt();
}

inline void fromJson( const Json::Value& from, Json::Value& to)
{
    to = from;
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

		toJson(name,from,json);
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
void toJson(const char* n, const T& from, Json::Value& to, typename std::enable_if<std::is_class<T>::value>::type*)
{
    const auto& m = meta_of(from);

	auto result = Json::Value(Json::objectValue);

	auto visitor = [&from,&to,n]( const char* name, auto& m)
	{	
		toJson(name,m.get(from),to[n]);
	};
	m.visit(from,visitor);
}

template<class T>
Json::Value toJson(const T& t)
{
    Json::Value result(Json::objectValue);

    const auto& m = meta_of(t);

	if(m.entity)
	{
		toJson(m.entity,t,result);
	} 
	else
	{
		auto visitor = [&t, &result]( const char* name, auto& m)
		{	
			toJson(name,m.get(t),result);
		};
		m.visit(t,visitor);
	}

    return result;
}

inline Json::Value toJson(const Json::Value& t)
{
    return t;
}

template<class T>
Json::Value toJson(const std::vector<T>& t)
{
    Json::Value result(Json::arrayValue);

	for ( auto& i : t)
	{
		Json::Value item(Json::objectValue);
		toJson("dummy",i,item);
		result.append(item["dummy"]);
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
			auto visitor = [&member, &t]( const char* name, auto& m)
			{	
				std::remove_reference_t<typename std::remove_reference_t<decltype(m)>::value_t> value;
				fromJson(member[name],value);
				m.set(t,value);
			};
			m.visit(t,visitor);			
			return;
		}
	}

	auto visitor = [&from, &t]( const char* name, auto& m)
	{	
		std::remove_reference_t<typename std::remove_reference_t<decltype(m)>::value_t> value;
		fromJson(from[name],value);
		m.set(t,value);
	};
	m.visit(t,visitor);
}

template<class T>
void fromJson(const std::string& from, T& t)
{
	Json::Value json = JSON::parse(from);
	fromJson(json,t);
}
*/
//////////////////////////////////////////////////////////////

template<class T>
class HandlerParam<json_t<T>>
{
public:

	static json_t<T> get(prio::Request& req,  prio::Response& res)
	{	
		Json::Value json = JSON::parse(req.body());

		json_t<T> t;
		meta::fromJson(json,t.value);

		std::cout << JSON::stringify(meta::toJson(t.value)) << std::endl;
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
	output_json(res, meta::toJson(t) );
}


template<class T>
void output_json(prio::Response& res, json_t<T>& t)
{
	output_json(res, meta::toJson(t.value) );
}

//////////////////////////////////////////////////////////////

#ifndef _RESUMABLE_FUNCTIONS_SUPPORTED

template<class R,class C, class ... Args>
Async invoke_handler(FrontController& fc, prio::Request& req,  prio::Response& res, repro::Future<json_t<R>> (C::*fun)(Args...) )
{
	auto p = repro::promise();

	try
	{
		C& c = prepare_controller<C>(req);
		(c.*fun)(HandlerParam<Args>::get(req,res)...)			
		.then([p,&res](json_t<R> r)
		{
			output_json(res,r);
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
Async invoke_handler(FrontController& fc, prio::Request& req,  prio::Response& res, repro::Future<json_t<R>> (C::*fun)(Args...) )
{
	try
	{
		std::cout << req.body() << std::endl;
		C& c = prepare_controller<C>(req);
		json_t<R> r = co_await (c.*fun)(HandlerParam<Args>::get(req,res)...);		
		output_json(res,r);
	}
	catch(std::exception& ex)
	{
		fc.handle_exception(ex, req, res);
	}

	(void)(co_await prio::nextTick());
	co_return;
}

#endif


#ifndef _RESUMABLE_FUNCTIONS_SUPPORTED

template<class C, class ... Args>
Async invoke_handler(FrontController& fc, prio::Request& req,  prio::Response& res, repro::Future<Json::Value> (C::*fun)(Args...) )
{
	auto p = repro::promise();

	try
	{
		C& c = prepare_controller<C>(req);
		(c.*fun)(HandlerParam<Args>::get(req,res)...)			
		.then([p,&res](Json::Value r)
		{
			output_json(res,r);
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

template<class C, class ... Args>
Async invoke_handler(FrontController& fc, prio::Request& req,  prio::Response& res, repro::Future<Json::Value> (C::*fun)(Args...) )
{
	try
	{
		C& c = prepare_controller<C>(req);
		Json::Value r = co_await (c.*fun)(HandlerParam<Args>::get(req,res)...);	
		output_json(res,r);		
	}
	catch(std::exception& ex)
	{
		fc.handle_exception(ex, req, res);
	}

	(void)(co_await prio::nextTick());
	co_return;
}

#endif

///////////////////////////////////////////////////////////////////////////////////////////

}

#endif

