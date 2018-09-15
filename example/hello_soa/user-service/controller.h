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
		std::string email;

		// todo handle ex in fc. just throw!
		try
		{
			email = Valid::login<LoginEx>(req.path.args().get("email"));
		}
		catch(const std::exception& ex)
		{
			auto p = promise<User>();

			LoginEx lex(ex.what());
			nextTick([p,lex]()
			{
				p.reject(lex);
			});
			return p.future();
		}

		return model_->get_user(email);
	}

	Future<User> login_user( Request& req, Response& res)
	{
		auto p = promise<User>();

		std::string login;
		std::string pwd;

		try
		{
			Json::Value json = JSON::parse(req.body());
			login = Valid::login<LoginEx>(json["login"].asString());
			pwd   = Valid::passwd<LoginEx>(json["pwd"].asString());
		}
		catch(const std::exception& ex)
		{
			auto p = promise<User>();

			LoginEx lex(ex.what());
			nextTick([p,lex]()
			{
				p.reject(lex);
			});
			return p.future();
		}		

		return model_->login_user(login,pwd);
	}

	Future<User> register_user( Request& req, Response& res)
	{
		std::string username;
		std::string login;
		std::string pwd;
		std::string avatar_url;

		try
		{
			Json::Value json = JSON::parse(req.body());
			username   = Valid::username(json["username"].asString());
			login      = Valid::login<RegistrationEx>(json["login"].asString());
			pwd        = Valid::passwd<RegistrationEx>(json["pwd"].asString());
			avatar_url = Valid::avatar(json["avatar_url"].asString());
		}
		catch(const std::exception& ex)
		{
			auto p = promise<User>();

			LoginEx lex(ex.what());
			nextTick([p,lex]()
			{
				p.reject(lex);
			});
			return p.future();
		}		

		User user(username,login,pwd,avatar_url);

		return model_->register_user(user);
	}

private:

	std::shared_ptr<Model> model_;

};



#endif