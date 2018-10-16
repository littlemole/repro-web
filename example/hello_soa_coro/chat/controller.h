#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_CONTROLLER_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_CONTROLLER_DEFINE_

#include "model.h"
#include "view.h"
#include "valid.h"

#include "cryptoneat/cryptoneat.h"
#include "reproweb/ctrl/controller.h"

using namespace reproweb;

class Controller
{ 
public:

	Controller( std::shared_ptr<Model> model, std::shared_ptr<View> view)
		: model_(model), view_(view)
	{}
     
	Async index( Parameter<Input> params,Request& req, Response& res)
	{
		//std::string sid = Valid::session_id(req.headers.cookies());

		Json::Value viewModel = co_await model_->chat(params->sid);

		view_->render_index(req,res,viewModel); 

		co_return;
	}

	Async show_login( Request& req, Response& res)
	{
		view_->render_login(req,res,"");

		co_return;
	}

	Async show_registration( Request& req, Response& res)
	{
		view_->render_registration(req,res,"");		

		co_return;
	}

	Async login( Form<Login> form, Request& req, Response& res)
	{
//		QueryParams qp(req.body());
		//std::string login = Valid::login<LoginEx>(params);
		//std::string pwd   = Valid::passwd<LoginEx>(params);

		std::string sid = co_await model_->login(form->login(),form->hash());

		view_->redirect_to_index(res,sid);

		co_return;
	}

	Async logout( Request& req, Response& res)
	{
		std::string sid = Valid::session_id(req.headers.cookies());

		co_await model_->logout(sid);

		view_->redirect_to_login(res);

		co_return;
	}

	Async register_user( Form<User> user, Request& req, Response& res)
	{
		/*
		QueryParams qp(req.body());
		std::string username   = Valid::username(qp);
		std::string login      = Valid::login<RegistrationEx>(qp);
		std::string pwd        = Valid::passwd<RegistrationEx>(qp);
		std::string avatar_url = Valid::avatar(qp);
		*/

		//User user(username,login,pwd,avatar_url);

		std::string sid = co_await model_->register_user(*user);

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

	Exceptions(std::shared_ptr<View> v)
		: view(v)
	{}

	void on_auth_ex(const AuthEx& ex, prio::Request& req, prio::Response& res)
	{
		view->render_login(req,res,ex.what());
	}		

	void on_login_ex(const LoginEx& ex,prio::Request& req, prio::Response& res)
	{
		view->render_login(req,res,ex.what());
	}	

	void on_register_ex(const RegistrationEx& ex,prio::Request& req, prio::Response& res)
	{
		view->render_registration(req,res,ex.what());
	}	

	void on_std_ex(const std::exception& ex,prio::Request& req, prio::Response& res)
	{
		view->render_error(res,ex);
	}

private:

	std::shared_ptr<View> view;
};


#endif