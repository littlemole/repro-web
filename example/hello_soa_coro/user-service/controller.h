#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_CONTROLLER_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_CONTROLLER_DEFINE_

#include "repo.h"
#include "cryptoneat/cryptoneat.h"
#include "reproweb/serialization/conneq.h"

using namespace reproweb;

class Controller
{
public:

	Controller( std::shared_ptr<UserRepository> repo)
		: userRepository(repo)
	{}

	async_t<User> get_user( Parameter<Email> email, Request& req, Response& res)
	{
		User user = co_await userRepository->get_user(email->value);

		co_return scrub(user);
	}

	async_t<User> login_user( entity<Login> login, Request& req, Response& res)
	{
		try 
		{
			User user = co_await userRepository->get_user(login->login());

			cryptoneat::Password pass;
			bool verified = pass.verify(login->hash(), user.hash() );

			if(!verified) 
			{
				throw LoginEx("error.msg.login.failed");
			}

			co_return scrub(user);
		}
		catch(const std::exception& ex)
		{
			std::cout << ex.what() << std::endl;
			throw;
		}
	}

	async_t<User> register_user( entity<User> user, Request& req, Response& res)
	{
		co_await userRepository->register_user(*user);

		co_return scrub(*user);
	}

private:
	std::shared_ptr<UserRepository> userRepository;


	static entity<User> scrub(const User& user) 
	{
		return entity<User> { User(user.username(),user.login(),"",user.avatar_url()) };
	}


};



class Exceptions
{
public:

	Exceptions()
	{}

	void on_user_not_found_ex(const UserNotFoundEx& ex,Request& req, Response& res)
	{
				std::cout << "UNF " << typeid(ex).name() << ":" << ex.what() << std::endl;

		render_error(ex,res.not_found());
	}		

	void on_bad_request_ex(const BadRequestEx& ex,Request& req, Response& res)
	{
		render_error(ex,res.bad_request());
	}	

	void on_login_ex(const LoginEx& ex,Request& req, Response& res)
	{
				std::cout << "lgx " << typeid(ex).name() << ":" << ex.what() << std::endl;

		render_error(ex,res.forbidden());
	}	

	void on_login_already_taken_ex(const LoginAlreadyTakenEx& ex,Request& req, Response& res)
	{
		render_error(ex,res.forbidden());
	}	

	void on_register_ex(const RegistrationEx& ex,Request& req, Response& res)
	{
		render_error(ex,res.bad_request());
	}	

	void on_std_ex(const std::exception& ex,Request& req, Response& res)
	{
				std::cout << "sex " << typeid(ex).name() << ":" << ex.what() << std::endl;

		render_error(ex,res.error());
	}

private:

	template<class E>
	void render_error(const E& ex, Response& res)
	{
		std::cout << typeid(ex).name() << ":" << ex.what() << std::endl;

		Json::Value json = exToJson(ex);

		std::cout << JSON::stringify(json) << std::endl;

		res
		.body(JSON::flatten(json))
		.contentType("application/json")
		.error()
		.flush();
	}
};


#endif