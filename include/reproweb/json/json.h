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
#include <priohttp/arg.h>
#include <priohttp/request.h>
#include <reproweb/traits.h>

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



//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////


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



template<class F>
repro::Future<> forEach( std::shared_ptr<Json::Value> json, unsigned int index, F f )
{
	auto p = repro::promise<>();

	if ( index == json->size() )
	{
		prio::nextTick([p]()
		{
			p.resolve();
		});
		
		return p.future();
	}

	int step = index;
	step++;

	f((*json)[index])
	.then([json,step,f]()
	{
		return forEach(json,step,f);
	})
	.then([p]()
	{
		p.resolve();
	})
	.otherwise(prio::reject(p));

	return p.future();
}


template<class F>
repro::Future<> forEach(Json::Value json, F f )
{
	auto container = std::make_shared<Json::Value>(json);
	int i = 0;

	return forEach( container, i, f);
}


inline Json::Value sort(const Json::Value& arrayIn, const std::string member)
{
	std::vector<Json::Value> v;

	for( unsigned int i = 0; i < arrayIn.size(); i++)
	{
		v.push_back(arrayIn[i]);
	}

	std::sort( 
		v.begin(), 
		v.end(), 
		[member]( const Json::Value& lhs, const Json::Value& rhs ) 
		{
			return lhs[member].asInt() < rhs[member].asInt();
		}
	);

	Json::Value result = Json::Value(Json::arrayValue);
	for ( auto& item : v ) 
	{
		result.append(item);
	}
	return result;
}

}

#endif

