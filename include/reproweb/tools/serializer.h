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
#include <reproweb/xml/document.h>

namespace reproweb {

///////////////////////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

template<class T, class M>
class Member
{
public:
    const char* name;
    M T::* member;
};

///////////////////////////////////////////////////////////////////////////////////////////

template<class T, class M>
Member<T,M> member( const char* n, M T::* m)
{
    return Member<T,M> {n,m};
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

template<class T, class M>
class GetterSetter
{
public:

    typedef M (T::*getter_t)();
    typedef void (T::*setter_t)(M);

    GetterSetter( const char* n, getter_t g, setter_t s)
        : name(n), getter(g), setter(s)
    {}

    GetterSetter( const char* n, getter_t g)
        : name(n), getter(g), setter(nullptr)
    {}    

    GetterSetter( const char* n,  setter_t s)
        : name(n), getter(nullptr), setter(s)
    {}    

    const char* name;
    getter_t getter;
    setter_t setter;
};

///////////////////////////////////////////////////////////////////////////////////////////

template<class T, class M>
class GetterSetterConst
{
public:

    typedef M (T::*getter_t)() const;
    typedef void (T::*setter_t)(M);

    GetterSetterConst( const char* n, getter_t g, setter_t s)
        : name(n), getter(g), setter(s)
    {}

    GetterSetterConst( const char* n, getter_t g)
        : name(n), getter(g), setter(nullptr)
    {}

    GetterSetterConst( const char* n,  setter_t s)
        : name(n), getter(nullptr), setter(s)
    {}   

    const char* name;
    getter_t getter;
    setter_t setter;
};

///////////////////////////////////////////////////////////////////////////////////////////

template<class T, class M>
GetterSetter<T,M> getter_setter( const char* n, M (T::*g)(), void (T::*s)(M) )
{
    return GetterSetter<T,M>(n,g,s);
}

template<class T,class M>
GetterSetter<T,M> getter_setter( const char* n, M (T::*g)() )
{
    return GetterSetter<T,M>(n,g);
}

template<class T, class M>
GetterSetterConst<T,M> getter_setter( const char* n, void (T::*s)(M) )
{
    return GetterSetterConst<T,M>(n,s);
}

template<class T, class M>
GetterSetterConst<T,M> getter_setter( const char* n, M (T::*g)() const, void (T::*s)(M) )
{
    return GetterSetterConst<T,M>(n,g,s);
}


template<class T, class M>
GetterSetterConst<T,M> getter_setter( const char* n, M (T::*g)() const )
{
    return GetterSetterConst<T,M>(n,g);
}


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

template<class T>
class MetaData;


///////////////////////////////////////////////////////////////////////////////////////////

template<class T, class M>
class MetaData<T(const char*, M )>
{
public:

    MetaData( const char* n, M m )
        : name(n), member(m)
    {
    }

    const char* entity = 0;
    const char* name;
    M member;

    template<class S>
    void serialize( const T& from, void* to ) const
    {
        S::serialize( name, from.*member, to );
    }


    template<class S>
    void deserialize( const void* from, T& to ) const
    {
        S::deserialize( name, from, to.*member );
    }

    MetaData<T(const char*, M )>& operator[](const char* n)
    {
        entity = n;
		return *this;
    }
};

///////////////////////////////////////////////////////////////////////////////////////////


template<class T, class M, class ... Args>
class MetaData<T(const char*, M, Args... )> : public MetaData<T(Args ...)>
{
public:

    MetaData( const char* n, M m, Args ... args )
        : MetaData<T(Args ...)>(args...), name(n), member(m)
    {
    }

    const char* entity = 0;
    const char* name;
    M member;

    template<class S>
    void serialize( const T& from, void* to ) const
    {
        S::serialize( name, from.*member, to);
        MetaData<T(Args...)>:: template serialize<S>(from,to);
    }


    template<class S>
    void deserialize(const void* from, T& to ) const
    {
        S::deserialize( name, from, to.*member);
        MetaData<T(Args...)>:: template deserialize<S>(from,to);
    }

    MetaData<T(const char*, M, Args... )>& operator[](const char* n)
    {
        entity = n;
		return *this;
    }
};

///////////////////////////////////////////////////////////////////////////////////////////

template<class T, class M, class ... Args>
class MetaData<T( Member<T,M>, Args... )> : public MetaData<T(Args ...)>
{
public:

    MetaData( const Member<T,M>& m, Args ... args )
        : MetaData<T(Args ...)>(args...), member(m)
    {
    }

    const char* entity = 0;
    Member<T,M> member;

    template<class S>
    void serialize( const T& from, void* to ) const
    {
        S::serialize( member.name, from.*(member.member), to);
        MetaData<T(Args...)>:: template serialize<S>(from,to);
    }


    template<class S>
    void deserialize( const void* from, T& to ) const
    {
        S::deserialize( member.name, from, to.*(member.member));
        MetaData<T(Args...)>:: template deserialize<S>(from,to);
    }

    MetaData<T( Member<T,M>, Args... )>& operator[](const char* n)
    {
        entity = n;
		return *this;
    }    
};

///////////////////////////////////////////////////////////////////////////////////////////

template<class T, class M, class ... Args>
class MetaData<T( GetterSetter<T,M>, Args... )> : public MetaData<T(Args ...)>
{
public:

    MetaData( const GetterSetter<T,M>& m, Args ... args )
        : MetaData<T(Args ...)>(args...), getterSetter(m)
    {
    }

    const char* entity = 0;
    GetterSetter<T,M> getterSetter;

    template<class S>
    void serialize( const T& from, void* to ) const
    {
        if(getterSetter.getter)
        {
            std::remove_const_t<std::remove_reference_t<M>> m = (from.*getterSetter.getter)();

            S::serialize( getterSetter.name, m, to);
        }
        MetaData<T(Args...)>:: template serialize<S>(from,to);
    }


    template<class S>
    void deserialize( const void* from, T& to ) const
    {
        if(getterSetter.setter)
        {

            std::remove_const_t<std::remove_reference_t<M>> m;
            S::deserialize( getterSetter.name, from, m);
            (to.*getterSetter.setter)(m);
        }
        MetaData<T(Args...)>:: template deserialize<S>(from,to);
    }

    MetaData<T( GetterSetter<T,M>, Args... )>& operator[](const char* n)
    {
        entity = n;
		return *this;
    }     
};

///////////////////////////////////////////////////////////////////////////////////////////

template<class T, class M, class ... Args>
class MetaData<T( GetterSetterConst<T,M>, Args... )> : public MetaData<T(Args ...)>
{
public:

    MetaData( const GetterSetterConst<T,M>& m, Args ... args )
        : MetaData<T(Args ...)>(args...), getterSetter(m)
    {
    }

    const char* entity = 0;
    GetterSetterConst<T,M> getterSetter;

    template<class S>
    void serialize( const T& from, void* to ) const
    {
        if(getterSetter.getter)
        {

            std::remove_const_t<std::remove_reference_t<M>> m = (from.*getterSetter.getter)();

            S::serialize( getterSetter.name, m, to);
        }
        MetaData<T(Args...)>:: template serialize<S>(from,to);
    }


    template<class S>
    void deserialize( const void* from, T& to ) const
    {
        if(getterSetter.setter)
        {

            std::remove_const_t<std::remove_reference_t<M>> m;
            S::deserialize( getterSetter.name, from, m);
            (to.*getterSetter.setter)(m);
        }
        MetaData<T(Args...)>:: template deserialize<S>(from,to);
    }

    MetaData<T( GetterSetterConst<T,M>, Args... )>& operator[](const char* n)
    {
        entity = n;
		return *this;
    }     
};

///////////////////////////////////////////////////////////////////////////////////////////


template<class T, class M, class ... Args>
auto metadata( const char* name, M T::*m, Args&& ... args)
{
    return MetaData<T(const char* , M T::*, typename std::decay<Args>::type ...)>( name, m, std::forward<Args&&>(args)...);
}

template<class T, class M, class ... Args>
auto metadata( Member<T,M>&& member, Args&& ... args)
{
    return MetaData<T( Member<T,M>, typename std::decay<Args>::type ...)>( member, std::forward<Args&&>(args)...);
}


template<class T, class M, class ... Args>
auto metadata( GetterSetter<T,M>&& getterSetter, Args&& ... args)
{
    return MetaData<T( GetterSetter<T,M>, typename std::decay<Args>::type ...)>( getterSetter, std::forward<Args&&>(args)...);
}

template<class T, class M, class ... Args>
auto metadata( GetterSetterConst<T,M>&& getterSetter, Args&& ... args)
{
    return MetaData<T( GetterSetterConst<T,M>, typename std::decay<Args>::type ...)>( getterSetter, std::forward<Args&&>(args)...);
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

template<class T>
auto meta_of(const T& t, typename std::enable_if<std::is_class<T>::value>::type* = nullptr) 
{
    typedef decltype( meta(std::declval<T>()) ) meta_t;
    static meta_t m = meta(t);

    return m;
}

///////////////////////////////////////////////////////////////////////////////////////////

template<class T>
auto meta_of(const std::vector<T>& t)
{
    typedef decltype( meta(std::declval<T>()) ) meta_t;
    static meta_t m = meta(t[0]);

    return m;
}

///////////////////////////////////////////////////////////////////////////////////////////

template<class T>
class RootEntity
{
public:

    T entity;

    T* operator->()
    {
        return &entity;
    }

	void validate()
	{
		validate(entity);
	}	
};

template<class T>
MetaData<RootEntity<T>(const char*, T RootEntity<T>::*)> meta(const RootEntity<T>& t );

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


const char* entity_name(const std::string& s)
{
    return "string";
}


template<class T>
const char* entity_name(const T& t, typename std::enable_if<std::is_class<T>::value>::type* = nullptr)
{
    const auto& m = meta_of(t);

    return m.entity ? m.entity : "entity";
}


template<class T>
const char* entity_name(const RootEntity<T>& t, typename std::enable_if<std::is_class<T>::value>::type* = nullptr)
{
    const auto& m = meta_of(t);

    return m.entity ? m.entity : "root";
}


template<class T>
const char* entity_name(const T& t, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr)
{
    return "number";
}

template<class T>
const char* entity_name(const RootEntity<T>& t, typename std::enable_if<!std::is_class<T>::value>::type* = nullptr)
{
    return entity_name(t.entity );
}


template<class T>
const char* entity_name(const std::vector<T>& t)
{
    return entity_name(t[0]);
}

template<class T>
const char* entity_name(const std::vector<RootEntity<T>>& t)
{
    return entity_name(t[0].entity);
}

///////////////////////////////////////////////////////////////////////////////////////////

template<class T>
MetaData<RootEntity<T>(const char*, T RootEntity<T>::*)> meta(const RootEntity<T>& t )
{
    return metadata (
        entity_name(t.entity), &RootEntity<T>::entity
    );
}

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
   // static T dummy;
    //auto m = meta(t);
    const auto& m = meta_of(t);

    Json::Value result(Json::objectValue);

    m. template serialize<JsonSerializer>(t,&result);

    return result;
}


///////////////////////////////////////////////////////////////////////////////////////////


template<class T>
void fromJson(const Json::Value& from, T& t)
{
    //auto m = meta(t);
    const auto& m = meta_of(t);

    m. template deserialize<JsonSerializer>(&from,t);
}



///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

inline void fromParams(const std::string& from, int& to)
{
	std::istringstream iss(from);
	iss >> to;
}

inline void fromParams(const std::string& from, std::string& to)
{
	to = from;
}

inline void fromParams(const std::string& from, prio::HeaderValues& to)
{

}

inline void fromParams(const std::string& from, prio::Cookie& to)
{

}

inline void fromParams( const prio::Cookie& from, prio::Cookie& to )
{
	to = from;
}


inline void fromParams( const prio::Cookie& from, std::string& to )
{
	to = from.value();
}



inline void fromParams( const prio::Cookie& from, prio::HeaderValues& to )
{
	// no op
}


inline void fromParams( const prio::HeaderValues& from, std::string& to )
{
	to = from.value().main();
}


inline void fromParams( const prio::HeaderValues& from, prio::HeaderValues& to )
{
	to = from;
}


inline void fromParams( const prio::HeaderValues& from, prio::Cookie& to )
{
	// no op
}


template<class T>
void fromParams( prio::QueryParams& qp, T& t);

template<class T>
void fromParams( prio::QueryParams& qp, std::vector<T>& t);

class ParamsSerializer
{
public:

    template<class T>
    static void deserialize( const char* name, const void* from, T& to) 
    {
		prio::QueryParams& qp = *( (prio::QueryParams*)from);

		std::string val = qp.get(name);

		fromParams(val,to);
    }
    
    template<class T>
    static void deserialize( const char* name, const void* from, std::vector<T>& to) 
    {
		prio::QueryParams& qp = *( (prio::QueryParams*)from);

		std::string val = qp.get(name);
		auto v = prio::split(val,",");
		for ( auto& i : v)
		{
			T t;
			fromParams(i,t);
			to.push_back(std::move(t));
		}
    }
};

template<class T>
void fromParams( prio::QueryParams& qp, T& t)
{
	const auto& m = meta_of(t);

    m. template deserialize<ParamsSerializer>(&qp,t);
}

template<class T>
void fromParams( prio::QueryParams& qp, std::vector<T>& t)
{
	const auto& m = meta_of(t);

    m. template deserialize<ParamsSerializer>(&qp,t);
}


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

template<class T>
void fromRequest( prio::Request& req, T& t);

class RequestSerializer
{
public:


    template<class T>
    static void deserialize( const char* member, const void* from, T& to) 
    {
		prio::Request& req = *( (prio::Request*)from);

		// path param

		prio::Args args = getPathArgs(req);
		if ( args.exists(member) )
		{
			reproweb::fromParams( args.get(member), to );
			return;
		}

		// query param
		prio::QueryParams qp = getQueryParams(req);
		if (qp.exists(member))
		{
			reproweb::fromParams( qp.get(member), to );
			return;
		}

		// cookie value

		const prio::Cookies& c = req.headers.cookies();

		if(c.exists(member))
		{
			reproweb::fromParams( c.get(member), to );
			return;
		}

		// header

		if(req.headers.exists(member))
		{
			reproweb::fromParams( req.headers.values(member), to );
			return;
		}
    }
    

	static prio::QueryParams getQueryParams(prio::Request& req )
	{
		static const char* key = "__queryparams";
		if ( !req.attributes.exists(key) )
		{
			req.attributes.set(key, req.path.queryParams());
		}

		return req.attributes.attr<prio::QueryParams>(key);
	}

	static prio::Args getPathArgs(prio::Request& req )
	{
		static const char* key = "__pathargs";
		if ( !req.attributes.exists(key) )
		{
			req.attributes.set(key, req.path.args() );
		}

		return req.attributes.attr<prio::Args>(key);
	}		
};

template<class T>
void fromRequest( prio::Request& req, T& t)
{
    const auto& m = meta_of(t);

    m. template deserialize<RequestSerializer>(&req,t);
}

}

#endif

