#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_CONTROLLER_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_CONTROLLER_DEFINE_

#include "model.h"
#include "view.h"

#include "cryptoneat/cryptoneat.h"
#include "reproweb/ctrl/controller.h"

using namespace reproweb;

class Controller
{ 
public:

	Controller( std::shared_ptr<Model> model, std::shared_ptr<View> view)
		: model_(model), view_(view)
	{}
     
	Future<std::string> index( Parameter<SessionCookie> params,Request& req)
	{
		Json::Value session = co_await model_->chat(params->sid);

		co_return co_await view_->render_index(req,session["user"]); 
	}

	Future<std::string> show_login( Request& req, Response& res)
	{
		co_return co_await view_->render_login(req,"");
	}

	Future<std::string> show_registration( Request& req, Response& res)
	{
		co_return co_await view_->render_registration(req,"");
	}

	Async login( Form<Login> credentials, Response& res)
	{
		std::string sid = co_await model_->login(*credentials);

		view_->redirect_to_index(res,sid);

		co_return;
	}

	Async logout( Parameter<SessionCookie> params, Response& res)
	{
		co_await model_->logout(params->sid);

		view_->redirect_to_login(res);

		co_return;
	}

	Async register_user( Form<User> user, Response& res)
	{
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
private:

	auto render(prio::Response& res)
	{
		return [&res](std::string content)
		{
			res.ok().body(content).contentType("text/html").flush();
		};
	}

public:

	Exceptions(std::shared_ptr<View> v)
		: view(v)
	{}

	void on_auth_ex(const AuthEx& ex, prio::Request& req, prio::Response& res)
	{
		view->render_login(req,"").then(render(res));
	}		

	void on_login_ex(const LoginEx& ex,prio::Request& req, prio::Response& res)
	{
		view->render_login(req,ex.what()).then(render(res));
	}	

	void on_register_ex(const RegistrationEx& ex,prio::Request& req, prio::Response& res)
	{
		view->render_registration(req,ex.what()).then(render(res));
	}	

	void on_std_ex(const std::exception& ex,prio::Request& req, prio::Response& res)
	{
		view->render_error(res,ex);
	}

private:

	std::shared_ptr<View> view;
};


#endif