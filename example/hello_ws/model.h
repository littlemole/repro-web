#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_MODEL_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_MODEL_DEFINE_

#include <string>
#include <memory>

#include "reproweb/tools/config.h"
#include "cryptoneat/cryptoneat.h"



class AuthEx : public repro::Ex 
{
public:
	AuthEx() {}
	AuthEx(const std::string& s) : Ex(s) {}
};


class RegistrationEx : public repro::Ex 
{
public:
	RegistrationEx() {}
	RegistrationEx(const std::string& s) : Ex(s) {}
};



class User
{
public:

	User( 
		const std::string& name,
		const std::string& login,
		const std::string& hash,
		const std::string& avatar_url
	)
		: name_(name),
		 login_(login),
		 hash_(hash),
		 avatar_url_(avatar_url)
	{}

	const std::string username() const 	  { return name_; }
	const std::string login() const 	  { return login_; }
	const std::string hash() const  	  { return hash_; }
	const std::string avatar_url() const  { return avatar_url_; }

	Json::Value toJson() const
	{
		Json::Value result(Json::objectValue);
		result["username"] = name_;
		result["login"] = login_;
		result["avatar_url"] = avatar_url_;
		return result;
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
	Session(Json::Value profile) 
		:sid_(make_session_id()), profile_(profile)
	{}

	Session(const std::string& sid,Json::Value profile) 
		:sid_(sid), profile_(profile)
	{}

	const std::string& sid() const  { return sid_; }
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

#endif