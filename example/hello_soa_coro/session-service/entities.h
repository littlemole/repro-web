#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_MODEL_ENTITIES_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_MODEL_ENTITIES_DEFINE_

#include "reproweb/tools/config.h"
#include "reproweb/json/json.h"
//#include "valid.h"
#include "reproweb/tools/validation.h"

using namespace reproweb;
using namespace repro;
using namespace prio;


class NoSessionEx : public repro::ReproEx<NoSessionEx> 
{
public:
	NoSessionEx() {}
	NoSessionEx(const std::string& s) : repro::ReproEx<NoSessionEx> (s) {}
};


class Input
{
public:

	std::string sid;

	reproweb::Serializer<Input> serialize()
	{
		return {
			"sid", 		&Input::sid
		};
	}	

	void validate()
	{
		static std::regex r("repro_web_sid::[0-9a-f]*");

		if(sid.empty())
			throw NoSessionEx("no session found");

		valid<NoSessionEx>( 
			sid, 
			r,
			"invalid session id."
		);			
	}
};

class User
{
public:

	User() {}

	User( 
		const std::string& name,
		const std::string& login,
		const std::string& avatar_url
	)
	  :  name_(name),
		 login_(login),
		 avatar_url_(avatar_url)
	{}

	std::string username() const 	{ return name_; }
	std::string login() const 	  	{ return login_; }
	std::string avatar_url() const  { return avatar_url_; }

	reproweb::Serializer<User> serialize();
	
private:
	std::string name_;	
	std::string login_;	
	std::string avatar_url_;	
};
 

reproweb::Serializer<User> User::serialize()
{
	return {
		"login", 		&User::login_,
		"username", 	&User::name_,
		"avatar_url", 	&User::avatar_url_
	};
}

class Session
{
public:
	Session()
	{}

	Session(Json::Value profile) 
		:sid_(make_session_id()), profile_(profile)
	{}

	Session(const std::string& sid,Json::Value profile) 
		:sid_(sid), profile_(profile)
	{}

	std::string sid() const  	{ return sid_; }
	Json::Value profile() const { return profile_; }

	reproweb::Serializer<Session> serialize()
	{
		return {
			"sid", 		&Session::sid_,
			"profile", 	&Session::profile_
		};
	}		

private:
	std::string sid_;
	Json::Value profile_;	

	static std::string make_session_id()
	{
		std::string sid = "repro_web_sid::";
		sid += cryptoneat::toHex(cryptoneat::nonce(64));
		return sid;
	}
};


class AppConfig : public reproweb::WebAppConfig
{
public:
	AppConfig(std::shared_ptr<diy::Context> ctx)
	  : WebAppConfig("config.json",ctx)
	{
		const char* redis = getenv("REDIS_HOST");
		if(redis)
		{
			std::ostringstream oss;
			oss << "redis://" << redis << ":6379";

			get("redis") = oss.str();
		}
		std::cout << "REDIS: " << get("redis") << std::endl;
	}
};

#endif
