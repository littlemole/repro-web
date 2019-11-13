#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_REPO_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_REPO_DEFINE_

#include "reproweb/ctrl/session.h"
#include "reproweb/json/service.h"
#include "entities.h"

using namespace prio;
using namespace repro;
using namespace reproweb;


class SessionService
{
public:

	SessionService(std::shared_ptr<AppConfig> conf)
		: config(conf)
	{}

	Future<reproweb::Session> get_user_session( std::string sid)
	{
		auto p = promise<reproweb::Session>();

		reproweb::JSON::Rest::url( config->sessionEndpoint(), sid )  
		.insecure()
		.get()
		.then([p,sid](Json::Value json)
		{
			Session session;
			fromJson(json,session);
			p.resolve(session);
		})
		.otherwise([p,sid](const std::exception& ex)
		{
			std::cout << typeid(ex).name() << ":" << ex.what() << std::endl;
			p.reject(AuthEx(ex.what()));
		});

		return p.future();
	}

	Future<> write_user_session(reproweb::Session& session)
	{
		auto p = promise<>();

		reproweb::JSON::Rest::url( config->sessionEndpoint() )
		.insecure()
		.post( session )
		.then([p](Json::Value json)
		{
			p.resolve();
		})
		.otherwise( [p](const std::exception& ex)
		{
			std::cout << typeid(ex).name() << ":" << ex.what() << std::endl;
			p.reject(AuthEx(ex.what()));
		}); 

		return p.future();
	}

	Future<> remove_user_session( std::string sid)
	{
		auto p = promise<>();

		reproweb::JSON::Rest::url( config->sessionEndpoint(), sid )
		.insecure()
		.remove( )
		.then([p]()
		{
			p.resolve();
		})
		.otherwise( [p](const std::exception& ex)
		{
			std::cout << typeid(ex).name() << ":" << ex.what() << std::endl;
			p.resolve();
		}); 

		return p.future();
	}

private:

	std::shared_ptr<AppConfig> config;
};


class UserService
{
public:

	UserService(std::shared_ptr<AppConfig> conf)
		: config(conf)
	{}

	Future<User> register_user( User user )
	{
		auto p = promise<User>();

		reproweb::JSON::Rest::url( config->registrationEndpoint())
		.insecure()
		.post( user )
		.then([p](Json::Value json)
		{
			User user;
			fromJson(json,user);

			p.resolve(user);
		})
		.otherwise( [p](const std::exception& ex)
		{
			std::cout << typeid(ex).name() << ":" << ex.what() << std::endl;
			p.reject(RegistrationEx(ex.what()));
		}); 

		return p.future();
	}

	Future<User> login_user( const std::string& login, const std::string& pwd )
	{
		auto p = promise<User>();

		Json::Value json(Json::objectValue);
		json["login"] = login;
		json["pwd"] = pwd;

		reproweb::JSON::Rest::url( config->loginEndpoint())
		.insecure()
		.post(json)
		.then([p](Json::Value json)		
		{
			User user;
			fromJson(json,user);

			p.resolve(user);
		})
		.otherwise( [p](const std::exception& ex)
		{
			std::cout << typeid(ex).name() << ":" << ex.what() << std::endl;
			p.reject(LoginEx(ex.what()));
		}); 

		return p.future();
	}
private:

	std::shared_ptr<AppConfig> config;

};



#endif
