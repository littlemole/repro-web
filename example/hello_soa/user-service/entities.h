#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_MODEL_ENTITIES_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_MODEL_ENTITIES_DEFINE_

#include <string>
#include <memory>

#include "reproweb/tools/config.h"
#include "reproweb/tools/validation.h"

using namespace reproweb;
using namespace repro;
using namespace prio;


class AuthEx : public repro::Ex 
{
public:
	AuthEx() {}
	AuthEx(const std::string& s) : Ex(s) {}
};

class LoginEx : public repro::Ex 
{
public:
	LoginEx() {}
	LoginEx(const std::string& s) : Ex(s) {}
};

class RegistrationEx : public repro::Ex 
{
public:
	RegistrationEx() {}
	RegistrationEx(const std::string& s) : Ex(s) {}
};


class Valid
{
public:

	static const std::string username( const std::string& name)
	{
		static std::regex r("[^<>]*");

		if(name.empty())
			throw RegistrationEx("error.msg.username.empty");

		return valid<RegistrationEx>(name, r, "error.msg.username.invalid");
	}

	template<class E>
	static const std::string passwd( const std::string& pwd)
	{
		static std::regex r(".*");

		return valid<E>(pwd, r , "error.msg.password.empty");
	}

	template<class E>
	static const std::string login( const std::string& email)
	{
		static std::regex r("^[a-zA-Z0-9.!#$%&’*+/=?^_`{|}~-]+@[a-zA-Z0-9-]+(?:\\.[a-zA-Z0-9-]+)*$");

		if(email.empty())
			throw E("error.msg.login.empty");

		return valid<E>(email, r, "error.msg.login.invalid.email" );
	}

	static const std::string avatar( const std::string& url)
	{
		static std::regex r("(http|https)://(\\w+:{0,1}\\w*@)?(\\S+)(:[0-9]+)?(/|/([\\w#!:.?+=&%@!-/]))?");

		if(url.empty())
			return "https://upload.wikimedia.org/wikipedia/commons/e/e4/Elliot_Grieveson.png";

		return valid<RegistrationEx>(url, r, "error.msg.avatar.invalid.url" );
	}	
   
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

	std::string username() const 	  { return name_; }
	std::string login() const 	  { return login_; }
	std::string hash() const  	  { return hash_; }
	std::string avatar_url() const  { return avatar_url_; }


	void validate()
	{
		Valid::username(name_);
		Valid::login<LoginEx>(login_);
		Valid::passwd<LoginEx>(hash_);
		Valid::avatar(avatar_url_);
	}

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



class AppConfig : public reproweb::Config
{
public:
	AppConfig()
	  : Config("config.json")
	{}
};

#endif
