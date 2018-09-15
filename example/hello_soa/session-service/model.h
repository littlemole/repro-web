#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_MODEL_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_MODEL_DEFINE_

#include "entities.h"
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
		auto p = promise<Json::Value>();

		sessionRepository->get_user_session(sid)
		.then([p](Session session)
		{
			p.resolve(session.profile());
		})
		.otherwise(reject(p));

		return p.future();
	}

	Future<std::string> login( std::string login, std::string pwd )
	{
		auto p = promise<std::string>();

		userRepository->get_user(login)
		.then([this,pwd](User user)
		{
			cryptoneat::Password pass;
			bool verified = pass.verify(pwd, user.hash() );

			if(!verified) 
			{
				throw LoginEx("error.msg.login.failed");
			}
			return sessionRepository->write_user_session(user);
		})
		.then([p](Session session)
		{
			p.resolve(session.sid());
		})
		.otherwise(reject(p));

		return p.future();
	}

	Future<> logout( const std::string& sid )
	{
		return sessionRepository->remove_user_session(sid);
	}

	Future<std::string> register_user( 
		const std::string& username,
		const std::string& login,
		const std::string& pwd,
		const std::string& avatar_url
		)
	{
		auto p = promise<std::string>();

		userRepository->register_user(username, login, pwd, avatar_url)
		.then([this](User user)
		{
			return sessionRepository->write_user_session(user);
		})
		.then([p](Session session)
		{
			p.resolve(session.sid());
		})		
		.otherwise(reject(p));

		return p.future();		
	}

private:

	std::shared_ptr<SessionRepository> sessionRepository;
	std::shared_ptr<UserRepository> userRepository;
};

#endif
