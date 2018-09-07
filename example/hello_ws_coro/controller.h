#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_CONTROLLER_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_CONTROLLER_DEFINE_

#include "model.h"
#include "view.h"
#include "repo.h"
#include "valid.h"

using namespace reproweb;

class Controller
{
public:

	Controller( std::shared_ptr<Model> model, std::shared_ptr<View> view)
		: model_(model), view_(view)
	{}

	Async index( Request& req, Response& res)
	{
		std::string sid = Valid::session_id(req.headers.cookies());

		Json::Value viewModel = co_await model_->chat(sid);

		view_->render_index(req,res,viewModel);

		co_return;		
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
		std::string login = Valid::login<LoginEx>(qp);
		std::string pwd   = Valid::passwd<LoginEx>(qp);

		std::string sid = co_await model_->login(login,pwd);

		view_->redirect_to_index(res,sid);
	
		co_return;		
	}

	Async logout( Request& req, Response& res)
	{
		const std::string sid = Valid::session_id(req.headers.cookies());

		co_await model_->logout(sid);

		view_->redirect_to_login(res);

		co_return;		
	}

	Async register_user( Request& req, Response& res)
	{
		QueryParams qp(req.body());
		std::string username   = Valid::username(qp);
		std::string login      = Valid::login<RegistrationEx>(qp);
		std::string pwd        = Valid::passwd<RegistrationEx>(qp);
		std::string avatar_url = Valid::avatar(qp);

		std::string sid = co_await model_->register_user(username,login,pwd,avatar_url);

		view_->redirect_to_index(res,sid);

		co_return;		
	}

private:

	std::shared_ptr<Model> model_;
	std::shared_ptr<View> view_;
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
		std::cout << "LoginEx: " << ex.what() << std::endl;
		view_->render_login(req,res,ex.what());
	}

	void on_registration_failed(const RegistrationEx& ex,Request& req, Response& res)
	{
		std::cout << "RegistrationEx: " << ex.what() << std::endl;
		view_->render_registration(req,res,ex.what());
	}

	void on_std_ex(const std::exception& ex,Request& req, Response& res)
	{
		std::cout << typeid(ex).name() << ":" << ex.what() << std::endl;
		view_->render_error(ex,res);
	}

private:

	std::shared_ptr<View> view_;
};


#endif