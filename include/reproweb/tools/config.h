#ifndef INCLUDE_PROMISE_WEB_CONFIG_H_
#define INCLUDE_PROMISE_WEB_CONFIG_H_

#include "reproweb/json/json.h"
#include "priohttp/common.h"

namespace reproweb  {

class Config
{
public:

	Config()
	{}

	Config( const std::string& path)
	{
		load(path);
	}

	Config& load( const std::string& path)
	{
		std::string f = prio::slurp(path);
		json_ = JSON::parse(f);
		return *this;
	}

	Json::Value& json()
	{
		return json_;
	}

	Json::Value& get(const std::string& key)
	{
		return json_[key];
	}

	std::string getString(const std::string& key)
	{
		return json_[key].asString();
	}

	int getInt(const std::string& key)
	{
		return json_[key].asInt();
	}

private:

	Json::Value json_;
};


}


#endif /* INCLUDE_PROMISE_WEB_CONFIG_H_ */
