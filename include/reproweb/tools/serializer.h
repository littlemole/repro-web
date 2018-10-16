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

inline Json::Value toJson( std::string s)
{
	return Json::Value(s);
}


inline Json::Value toJson( Json::Value json)
{
	return json;
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

inline void fromJson( std::string& s, Json::Value& json )
{
	s = json.asString();
}


inline void fromJson( Json::Value& member, Json::Value& json )
{
	member = json;
}


inline void fromJson( Json::Value& member, const std::string& value )
{
	member = JSON::parse(value);
}



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


template<class T>
inline void fromParam( T& i, const std::string& v  )
{
	std::istringstream iss(v);
	i << v;
}

inline void fromParam( Json::Value& member, const std::string value )
{
	member = JSON::parse(value);
}


inline void fromParam( std::string& s, const std::string& v )
{
	s = v;
}

inline void fromParam( std::string& s, const prio::Cookie& v )
{
	s = v.value();
}


inline void fromParam( prio::Cookie& c, const prio::Cookie& v )
{
	c = v;
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
		reproweb::fromParam( ((T*)t)->*mp, val );

		// path param
/*
		prio::Args args = req.path.args();
		if ( args.exists(s) )
		{
			reproweb::fromParam( ((T*)t)->*mp, args.get(s) );
		}

		// query param
		prio::QueryParams req.qp(path.queryParams());
		if (qp.exists(s))
		{
			reproweb::fromParam( ((T*)t)->*mp, qp.get(s) );
		}

		// cookie value

		const prio::Cookies& c = req.headers.cookies();

		if(c.exists(s))
		{
			reproweb::fromParam( ((T*)t)->*mp, c.get(s) );
		}

		// header

		if(req.headers.exists(s))
		{
			reproweb::fromParam( ((T*)t)->*mp, req.headers.values(s) );
			//return req.headers.values(s).value().main();
		}

		//reproweb::fromParam( ((T*)t)->*mp, val );

		*/
	}	

	std::string member;
	M T::* mp;
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

