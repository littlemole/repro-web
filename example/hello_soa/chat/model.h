#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_MODEL_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_MODEL_DEFINE_

#include "entities.h"
#include "service.h"

class Model 
{
public:
	Model( std::shared_ptr<SessionService> session, std::shared_ptr<UserService> user)
		:  sessionService(session), 
		   userService(user)
	{}


	Future<Json::Value> chat( const std::string& sid )
	{
		auto p = promise<Json::Value>();

  		sessionService->get_user_session(sid)
		.then([p](::Session session)
		{
			p.resolve(session.profile());
		})
		.otherwise(reject(p));

		return p.future();
	}

	Future<std::string> login( std::string login, std::string pwd )
	{
		auto p = promise<std::string>();

		userService->login_user(login,pwd)
		.then([this](User user)
		{
			return sessionService->write_user_session(user);
		})
		.then([p](::Session session)
		{
			p.resolve(session.sid());
		})
		.otherwise(reject(p));

		return p.future();
	}

	Future<> logout( const std::string& sid )
	{
		return sessionService->remove_user_session(sid);
	}

	Future<std::string> register_user( User& user )
	{
		auto p = promise<std::string>();

		userService->register_user(user)
		.then([this](User user)
		{
			return sessionService->write_user_session(user);
		})
		.then([p](::Session session)
		{
			p.resolve(session.sid());
		})		
		.otherwise(reject(p));

		return p.future();		
	}

private:

	std::shared_ptr<SessionService>  sessionService;
	std::shared_ptr<UserService> 	 userService;
};

#endif