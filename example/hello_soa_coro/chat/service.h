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
				
		req.insecure();

		reprocurl::response res = co_await reprocurl::fetch(req);

		std::cout << res.status() << ":" << res.content() << std::endl;

		if(res.status() != 200)
		{
			Json::Value json = reproweb::JSON::parse(res.content());
			throw E(json["error"]["msg"].asString());
		}

		co_return;
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
		req.insecure();//.verbose();

		reprocurl::response res = co_await reprocurl::fetch(req);

		std::cout << res.status() << ":" << res.content() << std::endl;

		Json::Value json = reproweb::JSON::parse(res.content());

		if(res.status() != 200)
		{
			throw E(json["error"]["msg"].asString());
		}

		co_return json;
	}
};

class SessionService
{
public:

	SessionService(std::shared_ptr<AppConfig> conf)
		: config(conf)
	{}

	Future<Session> get_user_session( std::string sid)
	{
		Json::Value json = co_await Service::get<AuthEx>( config->sessionService(sid) );

		co_return Session(sid,json);
	}

	Future<Session> write_user_session(User user)
	{
		Json::Value json = co_await Service::post<AuthEx>( config->sessionService(), user );

		co_return Session(
			json["sid"].asString(),
			json["profile"]
		);
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
		Json::Value json = co_await Service::post<RegistrationEx>( config->registrationService(), user );

		User result;
		fromJson(json,result);

		co_return result;
	}

	Future<User> login_user( const std::string& login, const std::string& pwd )
	{
		Json::Value json(Json::objectValue);
		json["login"] = login;
		json["pwd"] = pwd;

		Json::Value value = co_await Service::post<LoginEx>( config->loginService(), json );

		User user;
		fromJson(value,user);

		co_return user;
	}
private:

	std::shared_ptr<AppConfig> config;

};



#endif
