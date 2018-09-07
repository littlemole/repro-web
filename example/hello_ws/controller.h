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

	void index( Request& req, Response& res)
	{
		try
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
		catch(const std::exception& ex)
		{
			view_->redirect_to_login(res);
		}
	}

	void show_login( Request& req, Response& res)
	{
		view_->render_login(req,res,"");
	}

	void show_registration( Request& req, Response& res)
	{
		view_->render_registration(req,res,"");		
	}

	void login( Request& req, Response& res)
	{
		try
		{
			QueryParams qp(req.body());
			std::string login = Valid::login<LoginEx>(qp);
			std::string pwd   = Valid::passwd<LoginEx>(qp);

			model_->login(login,pwd)
			.then([this,&res](std::string sid)
			{
				view_->redirect_to_index(res,sid);
			})
			.otherwise([this,&req,&res](const std::exception& ex)
			{
				view_->render_login(req,res,ex.what());
			});
		}
		catch(const std::exception& ex)
		{
			view_->render_login(req,res,ex.what());
		}		
	}

	void logout( Request& req, Response& res)
	{
		try
		{
			const std::string sid = Valid::session_id(req.headers.cookies());

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
		catch(const std::exception& ex)
		{
			view_->redirect_to_login(res);
		}		
	}

	void register_user( Request& req, Response& res)
	{
		try
		{
			QueryParams qp(req.body());
			std::string username   = Valid::username(qp);
			std::string login      = Valid::login<RegistrationEx>(qp);
			std::string pwd        = Valid::passwd<RegistrationEx>(qp);
			std::string avatar_url = Valid::avatar(qp);

			model_->register_user(username,login,pwd,avatar_url)
			.then([this,&res](std::string sid)
			{
				view_->redirect_to_index(res,sid);
			})
			.otherwise([this,&req,&res](const std::exception& ex)
			{
				view_->render_registration(req,res,ex.what());
			});
		}
		catch(const std::exception& ex)
		{
			view_->render_registration(req,res,ex.what());
		}		
	}

private:

	std::shared_ptr<Model> model_;
	std::shared_ptr<View> view_;
};



#endif