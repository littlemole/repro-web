#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_CONTROLLER_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_CONTROLLER_DEFINE_

#include "model.h"
#include "repo.h"

#include "cryptoneat/cryptoneat.h"

using namespace reproweb;

class Controller
{
public:

	Controller( std::shared_ptr<Model> model)
		: model_(model)
	{}

	Future<User> get_user( Request& req, Response& res)
	{
		std::string email = Valid::login(req.path.args().get("email"));

		return model_->get_user(email);
	}

	Future<User> login_user( Request& req, Response& res)
	{
		auto p = promise<User>();

		Json::Value json  = JSON::parse(req.body());

		std::string login = Valid::login(json["login"].asString());
		std::string pwd   = Valid::passwd(json["pwd"].asString());

		std::cout << login << "|" << pwd << std::endl;

		 model_->login_user(login,pwd)
		.then([p](User user)
		{
			p.resolve(user);
		})
		.otherwise([p](const std::exception& ex)
		{
			std::cout << "---------- " << typeid(ex).name() << ex.what() << std::endl;
			p.reject(ex);
		});	
		return p.future();
	}

	Future<User> register_user( Request& req, Response& res)
	{
		Json::Value json = JSON::parse(req.body());

		std::string username   = Valid::username(json["username"].asString());
		std::string login      = Valid::login(json["login"].asString());
		std::string pwd        = Valid::passwd(json["pwd"].asString());
		std::string avatar_url = Valid::avatar(json["avatar_url"].asString());

		User user(username,login,pwd,avatar_url);

		std::cout << "register:" << JSON::stringify(toJson(user)) << std::endl;

		return model_->register_user(user);
	}

private:
	std::shared_ptr<Model> model_;
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