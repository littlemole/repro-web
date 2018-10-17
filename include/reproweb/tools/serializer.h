#ifndef _MOL_DEF_GUARD_DEFINE_REPROWEB_SERIALIZER_DEF_GUARD_
#define _MOL_DEF_GUARD_DEFINE_REPROWEB_SERIALIZER_DEF_GUARD_

#ifndef _WIN32
#include <json/json.h>
#else
#include <json/json.h>
#endif
#include "reprocpp/ex.h"

#include <functional>
#include <set>
#include <priocpp/api.h>
#include <cryptoneat/cryptoneat.h>
#include <priohttp/queryparams.h>
#include <priohttp/arg.h>
#include <priohttp/request.h>
#include <reproweb/json/json.h>

namespace reproweb {

///////////////////////////////////////////////////////////////////////////////////////////


inline Json::Value toJson(int i)
{
	return Json::Value(i);
}

inline Json::Value toJson(long i)
{
	return Json::Value(i);
}


inline Json::Value toJson(double i)
{
	return Json::Value(i);
}

inline Json::Value toJson(float i)
{
	return Json::Value(i);
}

inline Json::Value toJson( const std::string& s)
{
	return Json::Value(s);
}


inline Json::Value toJson( Json::Value& json)
{
	return json;
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

inline Json::Value toJson( prio::Headers& headers)
{
	Json::Value result(Json::objectValue);

	for( auto& h : headers.raw() )
	{
		result[h.first] = headers.get(h.first);
	}

	return result;
}


inline Json::Value toJson( prio::Cookie& cookie)
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
Json::Value toJson( std::vector<T>& v)
{
	Json::Value json(Json::arrayValue);

	for ( unsigned int i = 0; i < v.size(); i++)
	{
		json[i] = toJson(v[i]);
	}

	return json;
}


inline Json::Value exToJson(const std::exception& ex)
{
	Json::Value result(Json::objectValue);

	Json::Value error(Json::objectValue);
	error["msg"] = ex.what();
	error["type"] = typeid(ex).name();
	
	result["error"] = error;
	
	return result;
}

///////////////////////////////////////////////////////////////////////////////////////////

inline void fromJson( int& i, Json::Value& json )
{
	i = json.asInt();
}

inline void fromJson( long& i, Json::Value& json )
{
	i = json.asInt();
}


inline void fromJson( double& i, Json::Value& json )
{
	i = json.asDouble();
}


inline void fromJson( float& i, Json::Value& json )
{
	i = json.asFloat();
}


inline void fromJson( std::string& s, Json::Value& json )
{
	s = json.asString();
}


inline void fromJson( Json::Value& member, Json::Value& json )
{
	member = json;
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

inline void fromJson( prio::Headers& headers, Json::Value& json )
{
	auto members = json.getMemberNames();
	for ( auto& m : members )
	{
		headers.set( m, json[m].asString());
	}
}


inline void fromJson( prio::QueryParams& qp, Json::Value& json )
{
	auto members = json.getMemberNames();
	for ( auto& m : members )
	{
		qp.set( m, json[m].asString());
	}
}


inline void fromJson( prio::Args& args, Json::Value& json )
{
	prio::patharguments_t pargs;
	auto members = json.getMemberNames();
	for ( auto& m : members )
	{
		pargs.push_back( std::make_pair(m, json[m].asString()) );
	}
	args = prio::Args(pargs);
}

/*
inline void fromJson( Json::Value& member, const std::string& value )
{
	member = JSON::parse(value);
}

*/

template<class T>
void fromJson( std::vector<T>& v, Json::Value& json )
{
	unsigned int size = json.size();
	for ( unsigned int i = 0; i < size; i++)
	{
		T t;
		fromJson(t,json[i]);
		v.push_back( std::move(t) );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////


inline void fromParam( int& i, const std::string& s  )
{
	std::istringstream iss(s);
	iss >> i;
}

inline void fromParam( long& i, const std::string& s  )
{
	std::istringstream iss(s);
	iss >> i;
}

inline void fromParam( double& i, const std::string& s  )
{
	std::istringstream iss(s);
	iss >> i;
}

inline void fromParam( float& i, const std::string& s  )
{
	std::istringstream iss(s);
	iss >> i;
}


inline void fromParam( Json::Value& member, const std::string value )
{
	member = JSON::parse(value);
}


inline void fromParam( std::string& s, const std::string& v )
{
	s = v;
}

inline void fromParam( std::string& s, const prio::HeaderValues& v )
{
	s = v.value().main();
}


inline void fromParam( std::string& s, const prio::HeaderValue& v )
{
	s = v.main();
}

inline void fromParam( int& t, const prio::Cookie& c )
{
    fromParam(t,c.value());
}

inline void fromParam( long& t, const prio::Cookie& c )
{
    fromParam(t,c.value());
}

inline void fromParam( double& t, const prio::Cookie& c )
{
    fromParam(t,c.value());
}

inline void fromParam( float& t, const prio::Cookie& c )
{
    fromParam(t,c.value());
}

inline void fromParam( std::string& c, const prio::Cookie& v )
{
	c = v.value();
}


inline void fromParam( prio::Cookie& c, const prio::Cookie& v )
{
	c = v;
}

inline void fromParam( prio::Cookie& c, const std::string& v )
{
	c.value(v);
}


inline void fromParam( prio::Cookie& c, const prio::HeaderValues& v )
{
	c.value(v.value().main());
}


template<class T>
void fromParam( std::vector<T>& v, const std::string& value )
{
	
	auto values = prio::split( value, ',' );

	unsigned int size = values.size();
	for ( unsigned int i = 0; i < size; i++)
	{
		T t;
		fromParam(t,values[i]);
		v.push_back( std::move(t) );
	}
}


template<class T>
void fromParam( std::vector<T>& v, const prio::Cookie& cookie )
{
	
	auto values = prio::split( cookie.value(), ',' );

	unsigned int size = values.size();
	for ( unsigned int i = 0; i < size; i++)
	{
		T t;
		fromParam(t,values[i]);
		v.push_back( std::move(t) );
	}
}


template<class T>
void fromParam( std::vector<T>& v, const prio::HeaderValues& values  )
{
	unsigned int size = values.size();
	for ( unsigned int i = 0; i < size; i++)
	{
		T t;
		fromParam(t,values[i]);
		v.push_back( std::move(t) );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////



#ifdef MOL_ENABLE_UGLY_HELPER_MACROS
#define SERIALIZE(clazz,member) #member, &clazz::member
#endif

///////////////////////////////////////////////////////////////////////////////////////////


class SerializedMemberBase
{
public:

	virtual ~SerializedMemberBase(){}

 	virtual void toJson( void* p, Json::Value &json) = 0;
	virtual void fromJson( void* p, Json::Value& json ) = 0;
	virtual void fromParams( void* p, prio::QueryParams& qp ) = 0;
	virtual void fromPath( void* p, prio::Args& args ) = 0;
	virtual void fromRequest( void* p, prio::Request& req ) = 0;
};

///////////////////////////////////////////////////////////////////////////////////////////



template<class T, class M>
class SerializedMember : public SerializedMemberBase
{
public:

	SerializedMember(const char* m, M T::*p)
		: member(m), mp(p)
	{}

 	void toJson( void* t, Json::Value &json)
	{
		json[member] = reproweb::toJson( ((T*)t)->*mp);
	}

	void fromJson( void* t, Json::Value& json )
	{
		if(json.isMember(member))
		{
			 reproweb::fromJson( ((T*)t)->*mp, json[member] );
		}
	}

	void fromParams( void* t, prio::QueryParams& qp )
	{
		if(qp.exists(member))
		{
			 reproweb::fromParam( ((T*)t)->*mp, qp.get(member) );
		}
	}

	void fromPath( void* t, prio::Args& args )
	{
		if(args.exists(member))
		{
			 reproweb::fromParam( ((T*)t)->*mp, args.get(member) );
		}
	}

	void fromRequest( void* t, prio::Request& req )
	{
		std::string val = req[member];

		// path param

		prio::Args args = getPathArgs(req);
		if ( args.exists(member) )
		{
			reproweb::fromParam( ((T*)t)->*mp, args.get(member) );
			return;
		}

		// query param
		prio::QueryParams qp = getQueryParams(req);
		if (qp.exists(member))
		{
			reproweb::fromParam( ((T*)t)->*mp, qp.get(member) );
			return;
		}

		// cookie value

		const prio::Cookies& c = req.headers.cookies();

		if(c.exists(member))
		{
			reproweb::fromParam( ((T*)t)->*mp, c.get(member) );
			return;
		}

		// header

		if(req.headers.exists(member))
		{
			reproweb::fromParam( ((T*)t)->*mp, req.headers.values(member) );
			return;
		}
	}	

	std::string member;

private:

	M T::* mp;


	prio::QueryParams getQueryParams(prio::Request& req )
	{
		static const char* key = "__queryparams";
		if ( !req.attributes.exists(key) )
		{
			req.attributes.set(key, req.path.queryParams());
		}

		return req.attributes.attr<prio::QueryParams>(key);
	}

	prio::Args getPathArgs(prio::Request& req )
	{
		static const char* key = "__pathargs";
		if ( !req.attributes.exists(key) )
		{
			req.attributes.set(key, req.path.args() );
		}

		return req.attributes.attr<prio::Args>(key);
	}		
};



template<class T,class M>
SerializedMember<T,M>* serialized_member(const char* m, M T::* p)
{
	return new SerializedMember<T,M>(m,p);
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////




template<class T>
class Serializer
{
public:
	template<class ...Args>
	Serializer(Args... args)
	{
		serialize(args...);
	}

 	Json::Value toJson( T& t)
	{
		Json::Value json(Json::objectValue);

		for( auto& m : members)
		{
			m->toJson(&t,json);
		}

		return json;
	}

	void fromJson( T& t, Json::Value& json )
	{
		for( auto& m : members)
		{
			m->fromJson(&t,json);
		}
	}


	void fromParams( T& t, prio::QueryParams& qp )
	{
		for( auto& m : members)
		{
			m->fromParams(&t,qp);
		}
	}	


	void fromPath( T& t, prio::Args& args )
	{
		for( auto& m : members)
		{
			m->fromPath(&t,args);
		}
	}		

	void fromRequest( T& t, prio::Request& req )
	{
		for( auto& m : members)
		{
			m->fromRequest(&t,req);
		}
	}	

protected:

	std::vector<std::unique_ptr<SerializedMemberBase>> members;

private:

	void serialize()
	{
		// terminator
	}

	template<class M, class ...Args>
	void serialize(const char* member, M mp, Args ... args )
	{
		auto jmp = serialized_member(member,mp);
		members.push_back(std::unique_ptr<SerializedMemberBase>(jmp));
		serialize(args...);
	}
};

///////////////////////////////////////////////////////////////////////////////////////////



template<class T, typename std::enable_if<has_serializer<T>::value>::type* = nullptr> 
Serializer<T>& serializer_of(T& t) 
{
     static Serializer<T> serializer = t.serialize();
     return serializer;
}


template<class T, typename std::enable_if<!has_serializer<T>::value>::type* = nullptr>
Serializer<T>& serializer_of(T& t)
{
     static Serializer<T> serializer = serialize(t);
     return serializer;
}


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


template<class T>
Json::Value toJson(T& t)
{
	return serializer_of(t).toJson(t);
}

template<class T>
void fromJson(T& t, Json::Value& json)
{
	serializer_of(t).fromJson(t,json);
}

///////////////////////////////////////////////////////////////////////////////////////////

template<class T>
void fromParams(T& t, prio::QueryParams& qp)
{
	serializer_of(t).fromParams(t,qp);
}

///////////////////////////////////////////////////////////////////////////////////////////

template<class T>
void fromPath(T& t, prio::Args& args)
{
	serializer_of(t).fromPath(t,args);
}

///////////////////////////////////////////////////////////////////////////////////////////

template<class T>
void fromRequest(T& t, prio::Request& req)
{
	serializer_of(t).fromRequest(t,req);
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

}

#endif

