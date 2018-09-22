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

#define TO_STR_HELPER(x) #x
#define TO_STR(x) TO_STR_HELPER(x)

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

		json()["version"] = TO_STR(VERSION);
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



int main(int argc, char **argv)
{
	std::cout << TO_STR(VERSION) << std::endl;

	prio::init();
	cryptoneat::SSLUser useSSL;

	WebApplicationContext ctx {

		GET  ( "/",				&Controller::index),
		GET  ( "/logout",		&Controller::logout),
		GET  ( "/login",		&Controller::show_login),
		GET  ( "/register",		&Controller::show_registration),
		POST ( "/login",		&Controller::login),
		POST ( "/register",		&Controller::register_user),

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
		singleton<Controller(View,SessionRepository,UserRepository)>(),
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
