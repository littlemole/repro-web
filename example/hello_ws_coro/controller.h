#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_CONTROLLER_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_CONTROLLER_DEFINE_

#include "model.h"
#include "view.h"
#include "repo.h"
#include <regex>

class ValidationEx : public repro::Ex 
{
public:
	ValidationEx() {}
	ValidationEx(const std::string& s) : Ex(s) {}
};


class validator
{
public:
	validator(const std::regex & r)
		: r_(r)
	{}

	std::string getString(const std::string& tainted)
	{
		std::smatch match;
		 if ( !std::regex_match(tainted,match,r_) )
		 {
			 throw ValidationEx("validation failed");
		 }

		 return match[0];
	}

	int getInteger(const std::string& tainted)
	{
		std::smatch match;
		if ( !std::regex_match(tainted,match,r_) )
		{
			throw ValidationEx("validation failed");
		}

		std::istringstream iss(match[0]);
		int i;
		iss >> i;		 
		return i;
	}

	double getDouble(const std::string& tainted)
	{
		std::smatch match;
		if ( !std::regex_match(tainted,match,r_) )
		{
			throw ValidationEx("validation failed");
		}

		std::istringstream iss(match[0]);
		double d;
		iss >> d;		 
		return d;
	}

private:
	std::regex r_;	
};

inline std::string valid(const std::string& s, const std::regex& r)
{
	validator v(r);
	return v.getString(s);
}

inline std::string valid_int(const std::string& s)
{
	validator v(std::regex("[0-9]*"));
	return v.getString(s);
}

inline std::string valid_double(const std::string& s)
{
	validator v(std::regex("[0-9\\.]*"));
	return v.getString(s);
}

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

		Session session = co_await sessionRepository->get_user_session(session_id);
		view_->render_index(res,session.profile());
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

	reproweb::Async logout( Request& req, Response& res)
	{
		const std::string session_id = get_session_id(req.headers.cookies());

		co_await sessionRepository->remove_user_session(session_id);

		view_->redirect_to_login(res);
	}

	reproweb::Async register_user( Request& req, Response& res)
	{
		QueryParams qp(req.body());
		std::string username   = qp.get("username");
		std::string login      = qp.get("login");
		std::string pwd        = qp.get("pwd");
		std::string avatar_url = qp.get("avatar_url");

		User user;
		user = co_await userRepository->register_user(username, login, pwd, avatar_url);

		std::cout << "NEW USER SUCESS: " << user.username() << std::endl;
		
		Session session= co_await sessionRepository->write_user_session(user);

		view_->redirect_to_index(res,session.sid());
	}

private:

	std::shared_ptr<View> view_;
	std::shared_ptr<SessionRepository> sessionRepository;
	std::shared_ptr<UserRepository> userRepository;

	static const std::string get_session_id(const Cookies& cookies)
	{
		if(!cookies.exists("repro_web_sid"))
		{
			throw AuthEx("no session found");
		}

		return valid(
			cookies.get("repro_web_sid").value(), 
			std::regex("repro_web_sid::[0-9a-f]*")
		);
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
		std::cout << ex.what() << std::endl;
		view_->redirect_to_login(res);
	}

	void on_login_failed(const LoginEx& ex,Request& req, Response& res)
	{
		std::cout << ex.what() << std::endl;
		view_->render_login(res,ex.what());
	}

	void on_registration_failed(const RegistrationEx& ex,Request& req, Response& res)
	{
		std::cout << ex.what() << std::endl;
		view_->render_registration(res,ex.what());
	}

	void on_std_ex(const std::exception& ex,Request& req, Response& res)
	{
		std::cout << ex.what() << std::endl;
		view_->redirect_to_login(res);
	}

private:

	std::shared_ptr<View> view_;
};


#endif