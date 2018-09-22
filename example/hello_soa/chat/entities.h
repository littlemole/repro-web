#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_MODEL_ENTITIES_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_MODEL_ENTITIES_DEFINE_


#include "reproweb/tools/config.h"
#include "reproweb/json/json.h"

using namespace prio;
using namespace repro;
using namespace reproweb;

MAKE_REPRO_EX(AuthEx)
MAKE_REPRO_EX(LoginEx)
MAKE_REPRO_EX(RegistrationEx)


class User
{
public:

	User() {}

	User( 
		const std::string& name,
		const std::string& login,
		const std::string& hash,
		const std::string& avatar_url
	)
	  :  name_(name),
		 login_(login),
		 hash_(hash),
		 avatar_url_(avatar_url)
	{}

	std::string username() const 	  { return name_; }
	std::string login() const 	  { return login_; }
	std::string hash() const  	  { return hash_; }
	std::string avatar_url() const  { return avatar_url_; }

	static reproweb::Jsonizer<User>& jsonize()
	{
		static Jsonizer<User> jsonizer {
			"username", 	&User::name_,
			"login", 		&User::login_,
			"pwd", 			&User::hash_,
			"avatar_url", 	&User::avatar_url_
		};
		return jsonizer;
	}	
	
private:
	std::string name_;	
	std::string login_;	
	std::string hash_;	
	std::string avatar_url_;	
};


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

	std::string sid() const  { return sid_; }
	Json::Value profile() const     { return profile_; }

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

#define TO_STR_HELPER(x) #x
#define TO_STR(x) TO_STR_HELPER(x)

class AppConfig : public reproweb::WebAppConfig
{
public:
	AppConfig(std::shared_ptr<diy::Context> ctx)
	  : WebAppConfig("config.json",ctx)
	{
		const char* user = getenv("USER_SERVICE_HOST");
		if(user)
		{
			std::ostringstream oss;
			oss << "https://" << user << ":9877";

			get("user-service") = oss.str();
		}
		std::cout << "USER_SERVICE: " << get("user-service") << std::endl;

		const char* session = getenv("SESSION_SERVICE_HOST");
		if(session)
		{
			std::ostringstream oss;
			oss << "https://" << session << ":9878";

			get("session-service") = oss.str();
		}
		std::cout << "SESSION_SERVICE: " << get("session-service") << std::endl;

		json()["version"] = TO_STR(VERSION);

		std::ostringstream oss1;
		oss1 << getString("session-service") << "/session";
		sessionService_ = oss1.str();		

		std::ostringstream oss2;
		oss2 << getString("user-service") << "/register";
		registrationService_ = oss2.str();		

		std::ostringstream oss3;
		oss3 << getString("user-service") << "/login";
		loginService_ = oss3.str();		
	}

	std::string sessionService(const std::string& sid)
	{
		std::ostringstream oss;
		oss << getString("session-service") << "/session/" << sid;
		return oss.str();
	}

	std::string sessionService()
	{
		return sessionService_;
	}		

	std::string registrationService()
	{
		return registrationService_;
	}	

	std::string loginService()
	{
		return loginService_;
	}		

private:

	std::string sessionService_;
	std::string registrationService_;
	std::string loginService_;
};

#endif
