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


}

#endif

