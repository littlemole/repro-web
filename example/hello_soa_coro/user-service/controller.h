#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_CONTROLLER_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_CONTROLLER_DEFINE_

#include "repo.h"
#include "cryptoneat/cryptoneat.h"

using namespace reproweb;

class Controller
{
public:

	Controller( std::shared_ptr<UserRepository> repo)
		: userRepository(repo)
	{}

	Future<User> get_user( Request& req, Response& res)
	{
		std::string email = Valid::login(req.path.args().get("email"));

		User user = co_await userRepository->get_user(email);

		co_return scrub(user);
	}

	Future<User> login_user( Entity<Login> login, Request& req, Response& res)
	{
		User user = co_await userRepository->get_user(login.value.login());

		cryptoneat::Password pass;
		bool verified = pass.verify(login.value.hash(), user.hash() );

		if(!verified) 
		{
			throw LoginEx("error.msg.login.failed");
		}

		co_return scrub(user);
	}

	Future<User> register_user( Entity<User> user, Request& req, Response& res)
	{
		co_await userRepository->register_user(user.value);

		co_return user.value;
	}

private:
	std::shared_ptr<UserRepository> userRepository;

	static User scrub(const User& user) 
	{
		return User(user.username(),user.login(),"",user.avatar_url());
	}
};



class Exceptions
{
public:

	Exceptions()
	{}

	void on_user_not_found_ex(const UserNotFoundEx& ex,Request& req, Response& res)
	{
		render_error(ex,res.not_found());
	}		

	void on_bad_request_ex(const BadRequestEx& ex,Request& req, Response& res)
	{
		render_error(ex,res.bad_request());
	}	

	void on_login_ex(const LoginEx& ex,Request& req, Response& res)
	{
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
		render_error(ex,res.error());
	}

private:

	template<class E>
	void render_error(const E& ex, Response& res)
	{
		std::cout << typeid(ex).name() << ":" << ex.what() << std::endl;

		Json::Value json = exToJson(ex);

		res
		.body(JSON::flatten(json))
		.contentType("application/json")
		.flush();
	}
};


#endif