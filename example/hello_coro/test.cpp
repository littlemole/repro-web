#include "test.h"
#include "reproweb/ctrl/controller.h"
#include "reproweb/web_webserver.h"
#include <signal.h>
  
#include "model.h"
#include "view.h"
#include "repo.h"
#include "controller.h"

using namespace diy;  
using namespace prio;
using namespace reproweb;

class AppConfig : public Config
{
public:
	AppConfig()
	  : Config("config.json")
	{
		const char* redis = getenv("REDIS_HOST");
		if(redis)
		{
			std::ostringstream oss;
			oss << "redis://" << redis << ":6379";

			get("redis") = oss.str();
		}

	}
};

struct SessionPool : public reproredis::RedisPool
{
	SessionPool(std::shared_ptr<Config> config) 
	  : RedisPool(config->getString("redis")) 
	{}
};

struct UserPool : public reprosqlite::SqlitePool
{
	UserPool(std::shared_ptr<Config> config) 
	  : SqlitePool(config->getString("sqlite")) 
	{}
};


class Exceptions
{
public:

	void on_auth_failed(const AuthEx& ex,Request& req, Response& res)
	{
		std::cout << ex.what() << std::endl;
		redirect_to_login(res);
	}

	void on_registration_failed(const RegistrationEx& ex,Request& req, Response& res)
	{
		std::cout << ex.what() << std::endl;
		redirect_to_registration(res);
	}

	void on_std_ex(const std::exception& ex,Request& req, Response& res)
	{
		std::cout << ex.what() << std::endl;
		redirect_to_login(res);
	}

private:

	static void redirect_to_login(Response& res)
	{
		res
		.redirect("https://localhost:9876/login.html")
		.flush();
	}

	static void redirect_to_registration(Response& res)
	{
		res
		.redirect("https://localhost:9876/register.html")
		.flush();
	}	
};



int main(int argc, char **argv)
{
	prio::init();
	cryptoneat::SSLUser useSSL;

	WebApplicationContext ctx {

		GET  ( "/",				&ExampleController::index),
		GET  ( "/logout",		&ExampleController::logout),
		GET  ( "/login",		&ExampleController::show_login),
		GET  ( "/register",		&ExampleController::show_registration),
		POST ( "/login",		&ExampleController::login),
		POST ( "/register",		&ExampleController::register_user),

		ex_handler(&Exceptions::on_auth_failed),
		ex_handler(&Exceptions::on_registration_failed),
		ex_handler(&Exceptions::on_std_ex),

		static_content("/htdocs/","/etc/mime.types"),

		singleton<AppConfig()>(),
		singleton<SessionPool(AppConfig)>(),
		singleton<UserPool(AppConfig)>(),

		singleton<SessionRepository(SessionPool)>(),
		singleton<UserRepository(UserPool)>(),

		singleton<View(AppConfig)>(),
		singleton<ExampleController(View,SessionRepository,UserRepository)>(),

		singleton<Exceptions()>()
	};	

	Http2SslCtx sslCtx;
	sslCtx.load_cert_pem("pem/server.pem");
	//sslCtx.enableHttp2();

	WebServer server(ctx);
	server.listen(sslCtx,9876);
     
	theLoop().run();

	MOL_TEST_PRINT_CNTS();	
    return 0;
}

/*
Application defines (
	exceptions(
		ex_handler(&Exceptions::on_auth_failed),
		ex_handler(&Exceptions::on_registration_failed),
		ex_handler(&Exceptions::on_std_ex)
	),

	view_path("/view"),

	static_content("/htdocs/","/etc/mime.types")
);
*/
