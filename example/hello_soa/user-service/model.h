#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_MODEL_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_MODEL_DEFINE_

#include "entities.h"
#include "repo.h"

class Model 
{
public:
	Model( std::shared_ptr<UserRepository> userRepo)
		:  userRepository(userRepo)
	{}


	Future<User> get_user( const std::string& email )
	{
		return userRepository->get_user(email);
	}

	Future<User> login_user( std::string login, std::string pwd )
	{
		auto p = promise<User>();

		userRepository->get_user(login)
		.then([p,pwd](User user)
		{
			cryptoneat::Password pass;
			bool verified = pass.verify(pwd, user.hash() );

			if(!verified) 
			{
				throw LoginEx("error.msg.login.failed");
			}

			p.resolve(user);
		})
		.otherwise(reject(p));

		return p.future();
	}

	Future<User> register_user( User& user )
	{
		return userRepository->register_user(user);
	}

private:

	std::shared_ptr<UserRepository> userRepository;
};

#endif
