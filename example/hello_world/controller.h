#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_CONTROLLER_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_CONTROLLER_DEFINE_

#include "model.h"
#include "view.h"
#include "repo.h"


class Controller
{
public:

	Controller( 
		std::shared_ptr<View> view,
		std::shared_ptr<SessionRepository> sessionRepo, 
		std::shared_ptr<UserRepository> userRepo )
		: view_(view),
		  sessionRepository(sessionRepo), 
		  userRepository(userRepo)
	{}

	void index( Request& req, Response& res)
	{
		const std::string session_id = get_session_id(req.headers.cookies());
		if(session_id.empty())
		{
			view_->redirect_to_login(res);
			return;
		}

		sessionRepository->get_user_session(session_id)
		.then( [this,&res](Session session)
		{
			view_->render_index(res,session.profile());

		})
		.otherwise([this,&res](const std::exception& ex)
		{
			view_->redirect_to_login(res);
		});
		
	}

	void show_login( Request& req, Response& res)
	{
		view_->render_login(res,"");		
	}

	void show_registration( Request& req, Response& res)
	{
		view_->render_registration(res,"");		
	}

	void login( Request& req, Response& res)
	{
		QueryParams qp(req.body());
		std::string login = qp.get("login");
		std::string pwd   = qp.get("pwd");

		userRepository->get_user(login)
		.then( [this,pwd](User user)
		{
			cryptoneat::Password pass;
			bool verified = pass.verify(pwd, user.hash() );

			std::cout << "valid pwd: " << verified << std::endl;

			if(!verified) throw repro::Ex("invalid login/password combination");

			return sessionRepository->write_user_session(user);
		})
		.then( [this,&res](Session session)
		{
			view_->redirect_to_index(res,session.sid());
		})
		.otherwise( [this,&res](const std::exception& ex)
		{
			view_->render_login(res,ex.what());
		});
	}

	void logout( Request& req, Response& res)
	{
		const std::string session_id = get_session_id(req.headers.cookies());
		if(session_id.empty())
		{
			view_->redirect_to_login(res);
			return;
		}

		sessionRepository->remove_user_session(session_id)
		.then([this,&res]()
		{
			view_->redirect_to_login(res);
		})
		.otherwise([this,&res](const std::exception& ex)
		{
			view_->redirect_to_login(res);
		});
	}

	void register_user( Request& req, Response& res)
	{
		QueryParams qp(req.body());
		std::string username   = qp.get("username");
		std::string login      = qp.get("login");
		std::string pwd        = qp.get("pwd");
		std::string avatar_url = qp.get("avatar_url");

		userRepository->register_user(username,login,pwd,avatar_url)
		.then( [this](User user)
		{
			std::cout << "NEW USER SUCESS: " << user.username() << std::endl;
			return sessionRepository->write_user_session(user);
		})
		.then( [this,&res](Session session)
		{
			view_->redirect_to_index(res,session.sid());
		})
		.otherwise( [this,&res](const std::exception& ex)
		{
			view_->render_registration(res,ex.what());
		});		
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