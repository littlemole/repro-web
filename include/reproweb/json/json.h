#ifndef _MOL_DEF_GUARD_DEFINE_MOD_HTTP_RESPONSE_JSON_DEF_GUARD_
#define _MOL_DEF_GUARD_DEFINE_MOD_HTTP_RESPONSE_JSON_DEF_GUARD_

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

namespace reproweb  {

namespace JSON {

//////////////////////////////////////////////////////////////
	
typedef repro::Ex JsonParseEx;

//////////////////////////////////////////////////////////////

inline Json::Value parse(const std::string& txt)
{
	Json::Value json;
	
	Json::CharReaderBuilder rbuilder;
	std::string errs;
	std::istringstream iss(txt);
	bool ok = Json::parseFromStream(rbuilder, iss, &json, &errs);
	if(!ok)
	{
		throw JsonParseEx(errs);
	}
    return json;
}

//////////////////////////////////////////////////////////////

inline const std::string stringify(Json::Value value)
{
	Json::StreamWriterBuilder wbuilder;
	return Json::writeString(wbuilder, value);

}

inline const std::string flatten(Json::Value value)
{
	Json::StreamWriterBuilder wbuilder;
	wbuilder["commentStyle"] = "None";
	wbuilder["indentation"] = ""; 
	return Json::writeString(wbuilder, value);
}


//////////////////////////////////////////////////////////////


} // end namespace JSON

struct subscription
{
	 subscription( std::function<void(Json::Value)> f) : fun(f) {}

	 std::function<void(Json::Value)> fun;
};


inline bool operator<(const subscription& lhs, const subscription& rhs)
{
	 if ( &lhs == &rhs)
	 {
		 return false;
	 }

	 return &lhs.fun < &rhs.fun;
}

class EventBus
{
public:


	std::string subscribe( const std::string& topic, std::function<void(Json::Value)> observer)
	{
		std::string id = cryptoneat::nonce(32);
		prio::nextTick( [this,id,topic,observer] ()
		{
			subscriptions_[topic].insert(std::make_pair(id, subscription(observer)));
		});
		return id;
	}

	void unsubscribe( const std::string& topic,  const std::string& id)
	{
		prio::nextTick( [this,topic,id] ()
		{
			subscriptions_[topic].erase(id);
		});
	}

	void notify(const std::string& topic, Json::Value value)
	{
		for( auto fun : subscriptions_[topic])
		{
			prio::nextTick( [fun,value]()
			{
				fun.second.fun(value);
			});
		}
	}

	void clear()
	{
		subscriptions_.clear();
	}

private:

	std::map<std::string,std::map<std::string,subscription>> subscriptions_;
};


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

inline Json::Value exToJson(const std::exception& ex)
{
	Json::Value result(Json::objectValue);

	Json::Value error(Json::objectValue);
	error["msg"] = ex.what();
	error["type"] = typeid(ex).name();
	
	result["error"] = error;
	
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

inline void fromJson( int& i, Json::Value& json )
{
	i = json.asInt();
}

template<class T>
inline void fromParam( T& i, const std::string& v  )
{
	std::istringstream iss(v);
	i << v;
}


inline void fromJson( std::string& s, Json::Value& json )
{
	s = json.asString();
}

inline void fromParam( std::string& s, const std::string& v )
{
	s = v;
}


inline void fromJson( Json::Value& member, Json::Value& json )
{
	member = json;
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

#ifdef MOL_ENABLE_UGLY_HELPER_MACROS
#define SERIALIZE(clazz,member) #member, &clazz::member
#endif

class SerializedMemberBase
{
public:

	virtual ~SerializedMemberBase(){}

 	virtual void toJson( void* p, Json::Value &json) = 0;
	virtual void fromJson( void* p, Json::Value& json ) = 0;
	virtual void fromParams( void* p, prio::QueryParams& qp ) = 0;
};


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

	std::string member;
	M T::* mp;
};

template<class T,class M>
SerializedMember<T,M>* serialized_member(const char* m, M T::* p)
{
	return new SerializedMember<T,M>(m,p);
}

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

template<class T>
Serializer<T>& serializer_of(T& t)
{
	static Serializer<T> serializer = t.serialize();
	return serializer;
}

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


template<class T>
void fromParams(T& t, prio::QueryParams& qp)
{
	serializer_of(t).fromParams(t,qp);
}

}

#endif

