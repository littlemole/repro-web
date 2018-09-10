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
	Json::FastWriter writer;
	return writer.write( value );
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

inline Json::Value toJson(const std::string& s)
{
	return Json::Value(s);
}

template<class T>
Json::Value toJson(const std::vector<T>& v)
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

inline void fromJson( std::string& s, Json::Value& json )
{
	s = json.asString();
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



#define TO_JSON(clazz,member) #member, &clazz::member
//#define FROM_JSON(obj,member,json) fromJson(obj.member,json[ #member ])




class JsonMemberBase
{
public:

	virtual ~JsonMemberBase(){}

 	virtual void toJson( void* p, Json::Value &json) = 0;
	virtual void fromJson( void* p, Json::Value& json ) = 0;
};


template<class T, class M>
class JsonMember : public JsonMemberBase
{
public:

	JsonMember(const char* m, M T::*p)
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

	std::string member;
	M T::* mp;
};

template<class T,class M>
JsonMember<T,M>* json_member(const char* m, M T::* p)
{
	return new JsonMember<T,M>(m,p);
}

template<class T>
class Jsonizer
{
public:
	template<class ...Args>
	Jsonizer(Args... args)
	{
		jsonize(args...);
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

protected:

	std::vector<std::unique_ptr<JsonMemberBase>> members;

private:

	void jsonize()
	{
		// terminator
	}

	template<class M, class ...Args>
	void jsonize(const char* member, M mp, Args ... args )
	{
		auto jmp = json_member(member,mp);
		members.push_back(std::unique_ptr<JsonMemberBase>(jmp));
		jsonize(args...);
	}
};

template<class T>
Json::Value toJson(T& t)
{
	return t.jsonize().toJson(t);
}

template<class T>
void fromJson(T& t, Json::Value& json)
{
	t.jsonize().fromJson(t,json);
}

}

#endif

