#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_CONTROLLER_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_CONTROLLER_DEFINE_

#include "model.h"
#include "view.h"
#include "repo.h"


class ExampleController
{
public:

	ExampleController( 
		std::shared_ptr<View> view,
		std::shared_ptr<SessionRepository> sessionRepo, 
		std::shared_ptr<UserRepository> userRepo )
		: view_(view),
		  sessionRepository(sessionRepo), 
		  userRepository(userRepo)
	{}

	reproweb::Async index( Request& req, Response& res)
	{
		const std::string session_id = get_session_id(req.headers.cookies());
		if(session_id.empty())
		{
			view_->redirect_to_login(res);
			co_return;
		}

		try
		{
			Session session = co_await sessionRepository->get_user_session(session_id);
			view_->render_index(res,session.profile());
		}
		catch(const std::exception& ex)
		{
			view_->redirect_to_login(res);
		}		
	}

	void show_login( Request& req, Response& res)
	{
		view_->render_login(res,"");		
	}

	void show_registration( Request& req, Response& res)
	{
		view_->render_registration(res,"");		
	}

	reproweb::Async login( Request& req, Response& res)
	{
		try
		{
			QueryParams qp(req.body());
			std::string login = qp.get("login");
			std::string pwd   = qp.get("pwd");

			User user = co_await userRepository->get_user(login);
			cryptoneat::Password pass;
			bool verified = pass.verify(pwd, user.hash() );

			std::cout << "valid pwd: " << verified << std::endl;

			if(!verified) throw repro::Ex("invalid login/password combination");

			Session session = co_await sessionRepository->write_user_session(user);

			view_->redirect_to_index(res,session.sid());
		}
		catch(const std::exception& ex)
		{
			view_->render_login(res,ex.what());
		}
	}

	reproweb::Async logout( Request& req, Response& res)
	{
		const std::string session_id = get_session_id(req.headers.cookies());
		if(session_id.empty())
		{
			view_->redirect_to_login(res);
			co_return;
		}

		co_await sessionRepository->remove_user_session(session_id);

		view_->redirect_to_login(res);
	}

	reproweb::Async register_user( Request& req, Response& res)
	{
		try
		{
			QueryParams qp(req.body());
			std::string username   = qp.get("username");
			std::string login      = qp.get("login");
			std::string pwd        = qp.get("pwd");
			std::string avatar_url = qp.get("avatar_url");

			User user = co_await userRepository->register_user(username,login,pwd,avatar_url);

			std::cout << "NEW USER SUCESS: " << user.username() << std::endl;
			
			Session session = co_await sessionRepository->write_user_session(user);

			view_->redirect_to_index(res,session.sid());
		}
		catch(const std::exception& ex)
		{
			view_->render_registration(res,ex.what());
		}		
	}

private:

	std::shared_ptr<View> view_;
	std::shared_ptr<SessionRepository> sessionRepository;
	std::shared_ptr<UserRepository> userRepository;

	static const std::string get_session_id(const Cookies& cookies)
	{
		if(!cookies.exists("repro_web_sid"))
		{
			return "";
		}

		return cookies.get("repro_web_sid").value();
	}
};

#endif