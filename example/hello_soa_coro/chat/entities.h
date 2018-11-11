#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_MODEL_ENTITIES_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_MODEL_ENTITIES_DEFINE_


#include "reproweb/tools/config.h"
#include "reproweb/json/json.h"
#include "reproweb/serialization/json.h"
#include "reproweb/serialization/web.h"
#include "valid.h"

class Input
{
public:

	std::string sid;

	auto meta() const
	{
		return metadata(
			"repro_web_sid", 		&Input::sid
		);
	}	

	void validate()
	{
		static std::regex r("repro_web_sid::[0-9a-f]*");

		if(sid.empty())
			throw AuthEx("no session found");

		valid<AuthEx>( 
			sid, 
			r,
			"invalid session id."
		);			
	}
};

class Login
{
public:

	Login() {}

	Login( 
		const std::string& login,
		const std::string& hash
	)
	  :  login_(login),
		 hash_(hash)
	{}

	std::string login() const 	  		{ return login_; }
	std::string hash() const  	  		{ return hash_; }

	auto meta() const
	{
		return metadata(
			"login", 		&Login::login_,
			"pwd", 			&Login::hash_
		);
	}	

	void validate()
	{
		static std::regex r_email("^[a-zA-Z0-9.!#$%&’*+/=?^_`{|}~-]+@[a-zA-Z0-9-]+(?:\\.[a-zA-Z0-9-]+)*$");
		static std::regex r_pwd(".*");

		if(login_.empty())
			throw LoginEx("error.msg.login.empty");

		valid<LoginEx>(login_, 		r_email, 	"error.msg.login.invalid.email" );
		valid<LoginEx>(hash_, 		r_pwd , 	"error.msg.password.empty");
	}
	
private:
	std::string login_;	
	std::string hash_;	
};


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

	std::string username() const 	  	{ return name_; }
	std::string login() const 	  		{ return login_; }
	std::string hash() const  	  		{ return hash_; }
	std::string avatar_url() const  	{ return avatar_url_; }

	auto meta() const
	{
		return metadata(
			"username", 	&User::name_,
			"login", 		&User::login_,
			"pwd", 			&User::hash_,
			"avatar_url", 	&User::avatar_url_
		);
	}	

	void validate()
	{
		static std::regex r_username("[^<>]*");
		static std::regex r_email("^[a-zA-Z0-9.!#$%&’*+/=?^_`{|}~-]+@[a-zA-Z0-9-]+(?:\\.[a-zA-Z0-9-]+)*$");
		static std::regex r_pwd(".*");
		static std::regex r_url("(http|https)://(\\w+:{0,1}\\w*@)?(\\S+)(:[0-9]+)?(/|/([\\w#!:.?+=&%@!-/]))?");

		if(name_.empty())
			throw RegistrationEx("error.msg.username.empty");

		if(login_.empty())
			throw RegistrationEx("error.msg.login.empty");

		if(avatar_url_.empty())
		{
			avatar_url_ = "https://upload.wikimedia.org/wikipedia/commons/e/e4/Elliot_Grieveson.png";
		}

		valid<RegistrationEx>(name_, 		r_username, "error.msg.username.invalid");
		valid<RegistrationEx>(login_, 		r_email, 	"error.msg.login.invalid.email" );
		valid<RegistrationEx>(hash_, 		r_pwd , 	"error.msg.password.empty");
		valid<RegistrationEx>(avatar_url_, 	r_url, 		"error.msg.avatar.invalid.url" );
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
