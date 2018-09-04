#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_CONTROLLER_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_CONTROLLER_DEFINE_

#include "model.h"
#include "view.h"
#include "repo.h"
#include <regex>
#include "reproweb/tools/validation.h"


class Controller
{
public:

	Controller( std::shared_ptr<Model> model, std::shared_ptr<View> view)
		: model_(model), view_(view)
	{}

	Async index( Request& req, Response& res)
	{
		const std::string sid = get_session_id(req.headers.cookies());

		Json::Value viewModel = co_await model_->chat(sid);

		view_->render_index(req,res,viewModel);
	}

	void show_login( Request& req, Response& res)
	{
		view_->render_login(req,res,"");		
	}

	void show_registration( Request& req, Response& res)
	{
		view_->render_registration(req,res,"");		
	}

	Async login( Request& req, Response& res)
	{

		QueryParams qp(req.body());
		std::string login = get_login<LoginEx>(qp);
		std::string pwd   = get_passwd<LoginEx>(qp);

		std::string sid = co_await model_->login(login,pwd);

		view_->redirect_to_index(res,sid);
	}

	Async logout( Request& req, Response& res)
	{
		const std::string sid = get_session_id(req.headers.cookies());

		co_await model_->logout(sid);

		view_->redirect_to_login(res);
	}

	Async register_user( Request& req, Response& res)
	{
		QueryParams qp(req.body());
		std::string username   = get_username(qp);
		std::string login      = get_login<RegistrationEx>(qp);
		std::string pwd        = get_passwd<RegistrationEx>(qp);
		std::string avatar_url = get_avatar(qp);

		std::string sid = co_await model_->register_user(username,login,pwd,avatar_url);

		view_->redirect_to_index(res,sid);
	}

private:

	std::shared_ptr<Model> model_;
	std::shared_ptr<View> view_;


	static const std::string get_session_id(const Cookies& cookies)
	{
		static std::regex r("repro_web_sid::[0-9a-f]*");

		if(!cookies.exists("repro_web_sid"))
		{
			throw AuthEx("no session found");
		}

		return valid<AuthEx>( 
			cookies.get("repro_web_sid").value(), 
			r,
			"invalid session id."
		);
	}

	static const std::string get_username( QueryParams& params)
	{
		static std::regex r("[^<>]*");
		
		std::string username = params.get("username");

		if(username.empty())
			throw RegistrationEx("error.msg.username.empty");

		return valid<RegistrationEx>(username, r, "error.msg.username.invalid");
	}

	template<class E>
	static const std::string get_passwd( QueryParams& params)
	{
		static std::regex r(".*");

		std::string pwd = params.get("pwd");

		return valid<E>(pwd, r , "error.msg.password.empty");
	}

	template<class E>
	static const std::string get_login( QueryParams& params)
	{
		static std::regex r("^[a-zA-Z0-9.!#$%&’*+/=?^_`{|}~-]+@[a-zA-Z0-9-]+(?:\\.[a-zA-Z0-9-]+)*$");

		std::string login = params.get("login");

		if(login.empty())
			throw E("error.msg.login.empty");

		return valid<E>(login, r, "error.msg.login.invalid.email" );
	}

	static const std::string get_avatar( QueryParams& params)
	{
		static std::regex r("(http|https)://(\\w+:{0,1}\\w*@)?(\\S+)(:[0-9]+)?(/|/([\\w#!:.?+=&%@!-/]))?");

		std::string avatar = params.get("avatar_url");

		if(avatar.empty())
			return "https://upload.wikimedia.org/wikipedia/commons/e/e4/Elliot_Grieveson.png";

		return valid<RegistrationEx>(avatar, r, "error.msg.avatar.invalid.url" );
	}	
};


class Exceptions
{
public:

	Exceptions(std::shared_ptr<View> view)
		: view_(view)
	{}

	void on_auth_failed(const AuthEx& ex,Request& req, Response& res)
	{
		std::cout << "AuthEx: " << ex.what() << std::endl;
		view_->redirect_to_login(res);
	}

	void on_login_failed(const LoginEx& ex,Request& req, Response& res)
	{
		std::cout << ex.what() << std::endl;
		view_->render_login(req,res,ex.what());
	}

	void on_registration_failed(const RegistrationEx& ex,Request& req, Response& res)
	{
		std::cout << ex.what() << std::endl;
		view_->render_registration(req,res,ex.what());
	}

	void on_std_ex(const std::exception& ex,Request& req, Response& res)
	{
		std::cout << typeid(ex).name() << ":" << ex.what() << std::endl;
		view_->redirect_to_login(res);
	}

private:

	std::shared_ptr<View> view_;
};


#endif