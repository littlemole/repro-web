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

	void index( Request& req, Response& res)
	{
		std::string sid = Valid::session_id(req.headers.cookies());

		model_->chat(sid)
		.then([this,&req,&res](Json::Value viewModel)
		{
			view_->render_index(req,res,viewModel);
		})
		.otherwise([this,&res](const std::exception& ex)
		{
			view_->redirect_to_login(res);
		});
	}

	void show_login( Request& req, Response& res)
	{
		view_->render_login(req,res,"");
	}

	void show_registration( Request& req, Response& res)
	{
		view_->render_registration(req,res,"");		
	}

	void login( FormParams params, Request& req, Response& res)
	{
		std::string login = Valid::login<LoginEx>(params);
		std::string pwd   = Valid::passwd<LoginEx>(params);

		model_->login(login,pwd)
		.then([this,&res](std::string sid)
		{
			view_->redirect_to_index(res,sid);
		})
		.otherwise([this,&req,&res](const std::exception& ex)
		{
			std::cout << "login: " << ex.what() << std::endl;
			view_->render_login(req,res,ex.what());
		});
	}

	void logout( Request& req, Response& res)
	{
		std::string sid = Valid::session_id(req.headers.cookies());

		model_->logout(sid)
		.then([this,&res]()
		{
			view_->redirect_to_login(res);
		})
		.otherwise([this,&res](const std::exception& ex)
		{
			view_->render_error(ex,res);
		});	
	}

	void register_user( FormParams params, Request& req, Response& res)
	{
		std::string username   = Valid::username(params);
		std::string login      = Valid::login<RegistrationEx>(params);
		std::string pwd        = Valid::passwd<RegistrationEx>(params);
		std::string avatar_url = Valid::avatar(params);

		User user(username,login,pwd,avatar_url);

		model_->register_user(user)
		.then([this,&res](std::string sid)
		{
			view_->redirect_to_index(res,sid);
		})
		.otherwise([this,&req,&res](const std::exception& ex)
		{
			view_->render_registration(req,res,ex.what());
		});
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
		view->render_error(ex,res);
	}

private:

	std::shared_ptr<View> view;
};


#endif