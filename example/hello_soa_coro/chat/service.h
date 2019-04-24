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

	template<class T, class E>
	static Future<T> get(const std::string& serviceUrl)
	{
		reprocurl::request req( serviceUrl );

		return invoke<T,E>(req);
	}

	template<class E>
	static Future<> remove(const std::string& serviceUrl)
	{
		reprocurl::request req( serviceUrl );
		req.method("DELETE");

		auto p = promise<>();
				
		req.insecure();

		reprocurl::response res = co_await reprocurl::fetch(req);

		if(res.status() != 200)
		{
			Json::Value json = reproweb::JSON::parse(res.content());
			throw E(json["error"]["msg"].asString());
		}

		co_return;
	}	

	template<class R, class E, class T>
	static Future<R> post(const std::string& serviceUrl, T& t)
	{
		reprocurl::request req( serviceUrl );

		req.data( JSON::flatten(toJson(t))).method("POST");

		return invoke<R,E>(req);
	}

	template<class R, class E, class T>
	static Future<R> put(const std::string& serviceUrl, T& t)
	{
		reprocurl::request req( serviceUrl );

		req.data( JSON::flatten(toJson(t))).method("PUT");

		return invoke<R,E>(req);
	}

private:

	template<class T,class E>
	static Future<T> invoke(reprocurl::request& req)
	{
		req.insecure();//.verbose();

		reprocurl::response res = co_await reprocurl::fetch(req);

		if(res.status() != 200)
		{
			Json::Value json = reproweb::JSON::parse(res.content());
			throw E(json["error"]["msg"].asString());
		}

		T t;
		fromJson(res.content(),t);
		co_return t;
	}
};

class SessionService
{
public:

	SessionService(std::shared_ptr<AppConfig> conf)
		: config(conf)
	{}

	Future<::Session> get_user_session( std::string sid)
	{
		User user = co_await Service::get<User,AuthEx>( config->sessionService(sid) );

		co_return ::Session(sid,user);
	}

	Future<::Session> write_user_session(User user)
	{
		::Session session = co_await Service::post<::Session,AuthEx>( config->sessionService(), user );

		co_return session;	
	}

	Future<> remove_user_session( std::string sid)
	{
		co_await Service::remove<AuthEx>( config->sessionService(sid) );
		
		co_return;
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
		User result = co_await Service::post<User,RegistrationEx>( config->registrationService(), user );

		co_return result;
	}

	Future<User> login_user( Login login )
	{
		User result = co_await Service::post<User,LoginEx>( config->loginService(), login );
		
		co_return result;
	}
private:

	std::shared_ptr<AppConfig> config;

};



#endif
