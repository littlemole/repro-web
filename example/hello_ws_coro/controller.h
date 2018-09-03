#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_CONTROLLER_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_CONTROLLER_DEFINE_

#include "model.h"
#include "view.h"
#include "repo.h"
#include <regex>
#include "reproweb/tools/validation.h"

std::string xmlentities_encode(const std::string& in )
{
    std::ostringstream out;
    size_t p = 0;
    size_t len = in.size();
    while( ( p < len ) )
    {
        switch ( in[p] )
        {
            case '&' :
            {
                out << "&amp;";
                break;
            }
            case '<' :
            {
                out << "&lt;";
                break;
            }
            case '>' :
            {
                out << "&gt;";
                break;
            }
            default :
            {
                out << in[p];
                break;
            }
        }
        p++;
    }
    return out.str();
}

std::string xmlentities_decode( const std::string& str )
{
    std::ostringstream out;
    size_t len = str.size();
    for ( size_t i = 0; i < len; i++ )
    {
        if ( str[i] == '&' )
        {
			if ( str.substr(i,4) == "&lt;" )
            {
				out << "<";
                i+=3;
            }
            else
			if ( str.substr(i,4) == "&gt;" )
            {
                out << ">";
                i+=3;
            }
            else
	        if ( str.substr(i,5) == "&amp;" )
            {
                out << "&";
                i+=4;
            }
		}
        else
        {
			out << str[i];
        }
    }
    return out.str();
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
		view_->render_index(req,res,session.profile());
	}

	void show_login( Request& req, Response& res)
	{
		view_->render_login(req,res,"");		
	}

	void show_registration( Request& req, Response& res)
	{
		view_->render_registration(req,res,"");		
	}

	reproweb::Async login( Request& req, Response& res)
	{
		QueryParams qp(req.body());
		std::string login = get_login<LoginEx>(qp);
		std::string pwd   = get_passwd<LoginEx>(qp);

		User user = co_await userRepository->get_user(login);
		cryptoneat::Password pass;
		bool verified = pass.verify(pwd, user.hash() );

		std::cout << "valid pwd: " << verified << std::endl;

		if(!verified) 
		{
			throw LoginEx("invalid login/password combination");
		}

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
		std::string username   = get_username(qp);
		std::string login      = get_login<RegistrationEx>(qp);
		std::string pwd        = get_passwd<RegistrationEx>(qp);
		std::string avatar_url = get_avatar(qp);

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

		return valid<AuthEx>(
			cookies.get("repro_web_sid").value(), 
			std::regex("repro_web_sid::[0-9a-f]*"),
			"invalid session id."
		);
	}

	static const std::string get_username( QueryParams& params)
	{
		std::string username = params.get("username");

		if(username.empty())
			throw RegistrationEx("user name may not be empty");

		return valid<RegistrationEx>(username, std::regex("[^<>]*") ,"username contains invalid tokens.");
	}

	template<class E>
	static const std::string get_passwd( QueryParams& params)
	{
		std::string pwd = params.get("pwd");

		if(pwd.empty())
			throw E("password may not be empty");

		return valid<E>(pwd, std::regex(".*") , "password shall not be empty");
	}

	template<class E>
	static const std::string get_login( QueryParams& params)
	{
		std::string login = params.get("login");

		if(login.empty())
			throw E("login may not be empty");

		return valid<E>(login, std::regex("^[a-zA-Z0-9.!#$%&’*+/=?^_`{|}~-]+@[a-zA-Z0-9-]+(?:\\.[a-zA-Z0-9-]+)*$"), "login must be valid email address" );
	}

	static const std::string get_avatar( QueryParams& params)
	{
		std::string avatar = params.get("avatar_url");

		if(avatar.empty())
			return "https://upload.wikimedia.org/wikipedia/commons/e/e4/Elliot_Grieveson.png";

		return valid<RegistrationEx>(avatar, std::regex("(http|https)://(\\w+:{0,1}\\w*@)?(\\S+)(:[0-9]+)?(/|/([\\w#!:.?+=&%@!-/]))?"), "avatar url must be emty or a valid htpp/https url." );
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
		view_->render_login(req,res,ex.what());
	}

	void on_registration_failed(const RegistrationEx& ex,Request& req, Response& res)
	{
		std::cout << ex.what() << std::endl;
		view_->render_registration(req,res,ex.what());
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