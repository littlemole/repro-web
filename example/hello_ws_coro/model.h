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

#include "repo.h"

class Model 
{
public:
	Model( std::shared_ptr<SessionRepository> sessionRepo, std::shared_ptr<UserRepository> userRepo)
		:  sessionRepository(sessionRepo), 
		   userRepository(userRepo)
	{}


	Future<Json::Value> chat( const std::string& sid )
	{
		Session session = co_await sessionRepository->get_user_session(sid);

		co_return session.profile();
	}

	Future<std::string> login( std::string login, std::string pwd )
	{
		User user = co_await userRepository->get_user(login);

		cryptoneat::Password pass;
		bool verified = pass.verify(pwd, user.hash() );

		//std::cout << "valid pwd: " << verified << std::endl;

		if(!verified) 
		{
			throw LoginEx("error.msg.login.failed");
		}

		Session session = co_await sessionRepository->write_user_session(user);

		co_return session.sid();
	}

	Future<> logout( const std::string& sid )
	{
		co_await sessionRepository->remove_user_session(sid);

		co_return;
	}

	Future<std::string> register_user( 
		const std::string& username,
		const std::string& login,
		const std::string& pwd,
		const std::string& avatar_url
		)
	{
		User user = co_await userRepository->register_user(username, login, pwd, avatar_url);

		//std::cout << "NEW USER SUCESS: " << user.username() << std::endl;
		
		Session session= co_await sessionRepository->write_user_session(user);

		co_return session.sid();
	}

private:

	std::shared_ptr<SessionRepository> sessionRepository;
	std::shared_ptr<UserRepository> userRepository;
};

#endif
