#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_CONTROLLER_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_CONTROLLER_DEFINE_

#include "model.h"
#include "view.h"
#include "repo.h"

using namespace reproweb;

class Controller
{
public:

	Controller( std::shared_ptr<Model> model, std::shared_ptr<View> view)
		: model_(model), view_(view)
	{}

	void index( Request& req, Response& res)
	{
		auto session = req_session(req);
		if(!session->authenticated)
		{
			throw AuthEx();
		}

		Json::Value viewModel = session->data;

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

	Async login( Form<Login> loginForm, Request& req, Response& res)
	{
		User user = co_await model_->login(*loginForm);

		auto session = req_session(req);
		session->authenticated = true;
		session->data = toJson(user);

		view_->redirect_to_index(res);
	
		co_return;		
	}

	void logout( Request& req, Response& res)
	{
		invalidate_session(req);
		view_->redirect_to_login(res);
	}

	Async register_user( Form<User> userForm, Request& req, Response& res)
	{
		User user = co_await model_->register_user(*userForm);

		auto session = req_session(req);
		session->authenticated = true;
		session->data = toJson(user);

		view_->redirect_to_index(res);

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