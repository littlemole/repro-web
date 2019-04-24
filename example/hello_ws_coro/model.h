#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_MODEL_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_MODEL_DEFINE_

#include <string>
#include <memory>

#include "entities.h"
#include "repo.h"


#define TO_STR_HELPER(x) #x
#define TO_STR(x) TO_STR_HELPER(x)

class AppConfig : public Config
{
public:
	AppConfig(std::shared_ptr<FrontController> fc)
	  : Config("config.json")
	{
		const char* redis = getenv("REDIS_HOST");
		if(redis)
		{
			std::ostringstream oss;
			oss << "redis://" << redis << ":6379";

			get("redis") = oss.str();
		}
		std::cout << "REDIS: " << get("redis") << std::endl;

		json()["version"] = TO_STR(VERSION);
	}
};

class Model 
{
public:
	Model( std::shared_ptr<SessionRepository> sessionRepo, std::shared_ptr<UserRepository> userRepo)
		:  sessionRepository(sessionRepo), 
		   userRepository(userRepo)
	{}


	Future<Json::Value> chat( const std::string& sid )
	{
		::Session session = co_await sessionRepository->get_user_session(sid);

		co_return session.profile();
	}

	Future<std::string> login( Login creds )
	{
		User user = co_await userRepository->get_user( creds.login() );

		cryptoneat::Password pass;
		bool verified = pass.verify( creds.hash(), user.hash() );
		if(!verified) 
		{
			throw LoginEx("error.msg.login.failed");
		}

		::Session session = co_await sessionRepository->write_user_session(user);

		co_return session.sid();
	}

	Future<> logout( const std::string& sid )
	{
		co_await sessionRepository->remove_user_session(sid);

		co_return;
	}

	Future<std::string> register_user( User user )
	{
		User result = co_await userRepository->register_user(
			user.username(), 
			user.login(), 
			user.hash(),
			user.avatar_url()
		);

		::Session session = co_await sessionRepository->write_user_session(result);

		co_return session.sid();
	}

private:

	std::shared_ptr<SessionRepository> sessionRepository;
	std::shared_ptr<UserRepository> userRepository;
};

#endif
