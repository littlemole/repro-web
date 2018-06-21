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
	AppConfig(std::shared_ptr<FrontController> fc)
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

class ssl_ctx : public Http2SslCtx
{
public:
	ssl_ctx(std::shared_ptr<Config> config)
	{
		load_cert_pem(config->getString("cert"));
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

#ifndef _WIN32
		static_content("/htdocs/","/etc/mime.types"),
#else
		static_content("/htdocs/","mime.types"),
#endif
		singleton<AppConfig(FrontController)>(),
		singleton<SessionPool(AppConfig)>(),
		singleton<UserPool(AppConfig)>(),

		singleton<SessionRepository(SessionPool)>(),
		singleton<UserRepository(UserPool)>(),

		singleton<View(AppConfig)>(),
		singleton<ExampleController(View,SessionRepository,UserRepository)>(),

		singleton<ssl_ctx(AppConfig)>()
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
