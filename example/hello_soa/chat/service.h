#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_REPO_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_REPO_DEFINE_

#include "reprocurl/api.h"
#include "entities.h"

using namespace prio;
using namespace repro;
using namespace reproweb;

class Service 
{
public:

	template<class E>
	static Future<Json::Value> get(const std::string& serviceUrl)
	{
		reprocurl::request req( serviceUrl );

		return invoke<E>(req);
	}

	template<class E>
	static Future<> remove(const std::string& serviceUrl)
	{
		reprocurl::request req( serviceUrl );
		req.method("DELETE");

		auto p = promise<>();
				
		req.insecure().verbose();

		reprocurl::fetch(req)
		.then([p](reprocurl::response res)
		{
			std::cout << res.status() << ":" << res.content() << std::endl;

			if(res.status() != 200)
			{
				Json::Value json = reproweb::JSON::parse(res.content());
				throw E(json["error"]["msg"].asString());
			}

			p.resolve();
		})
		.otherwise(reject(p));

		return p.future();	
	}	

	template<class E, class T>
	static Future<Json::Value> post(const std::string& serviceUrl, T& t)
	{
		reprocurl::request req( serviceUrl );

		std::cout << serviceUrl << ":" << std::endl;
		std::cout << JSON::flatten(toJson(t))  << std::endl;

		req.data( JSON::flatten(toJson(t))).method("POST");

		return invoke<E>(req);
	}

	template<class E, class T>
	static Future<Json::Value> put(const std::string& serviceUrl, T& t)
	{
		reprocurl::request req( serviceUrl );

		req.data( JSON::flatten(toJson(t))).method("PUT");

		return invoke<E>(req);
	}

private:

	template<class E>
	static Future<Json::Value> invoke(reprocurl::request& req)
	{
		auto p = promise<Json::Value>();
				
		req.insecure().verbose();

		reprocurl::fetch(req)
		.then([p](reprocurl::response res)
		{
			std::cout << res.status() << ":" << res.content() << std::endl;

			Json::Value json = reproweb::JSON::parse(res.content());

			if(res.status() != 200)
			{
				throw E(json["error"]["msg"].asString());
			}

			p.resolve(json);
		})
		.otherwise(reject(p));

		return p.future();			
	}
};

class SessionService
{
public:

	SessionService(std::shared_ptr<AppConfig> conf)
		: config(conf)
	{}

	Future<reproweb::Session> get_user_session( std::string sid)
	{
		auto p = promise<reproweb::Session>();

		Service::get<AuthEx>( config->sessionService(sid) )
		.then([p,sid](Json::Value json)
		{
			Session session;
			fromJson(json,session);
			p.resolve(session);
		})
		.otherwise([p,sid](const std::exception& ex)
		{
			std::cout << typeid(ex).name() << ":" << ex.what() << std::endl;
			p.reject(ex);
		});

		return p.future();
	}

	Future<> write_user_session(reproweb::Session& session)
	{
		auto p = promise<>();

		Service::post<AuthEx>( config->sessionService(), session )
		.then([p](Json::Value json)
		{
			p.resolve();
		})
		.otherwise( [p](const std::exception& ex)
		{
			std::cout << typeid(ex).name() << ":" << ex.what() << std::endl;
			p.reject(ex);
		}); 

		return p.future();
	}

	Future<> remove_user_session( std::string sid)
	{
		return Service::remove<AuthEx>( config->sessionService(sid) );
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

		Service::post<RegistrationEx>( config->registrationService(), user )
		.then([p](Json::Value json)
		{
			User user;
			fromJson(json,user);

			p.resolve(user);
		})
		.otherwise(reject(p));

		return p.future();
	}

	Future<User> login_user( const std::string& login, const std::string& pwd )
	{
		auto p = promise<User>();

		Json::Value json(Json::objectValue);
		json["login"] = login;
		json["pwd"] = pwd;

		Service::post<LoginEx>( config->loginService(), json )
		.then([p](Json::Value json)		
		{
			User user;
			fromJson(json,user);

			p.resolve(user);
		})
		.otherwise( [p](const std::exception& ex)
		{
			std::cout << typeid(ex).name() << ":" << ex.what() << std::endl;
			p.reject(ex);
		}); //reject(p));

		return p.future();
	}
private:

	std::shared_ptr<AppConfig> config;

};



#endif
