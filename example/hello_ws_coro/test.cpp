#include "test.h"
#include "reproweb/ctrl/controller.h"
#include "reproweb/view/i18n.h"
#include "reproweb/web_webserver.h"
#include <signal.h>
  
#include "model.h"
#include "view.h"
#include "repo.h"
#include "controller.h"
#include "ws.h"

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
		std::cout << "REDIS: " << get("redis") << std::endl;
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
	prio::init();
	cryptoneat::SSLUser useSSL;

	WebApplicationContext ctx {

		GET  ( "/",				&Controller::index),
		GET  ( "/logout",		&Controller::logout),
		GET  ( "/login",		&Controller::show_login),
		GET  ( "/register",		&Controller::show_registration),
		POST ( "/login",		&Controller::login),
		POST ( "/register",		&Controller::register_user),

		ws_controller<WebSocketController> ("/ws"),

#ifndef _WIN32
		static_content("/htdocs/","/etc/mime.types"),
#else
		static_content("/htdocs/","mime.types"),
#endif

		i18n_props("/locale/properties", {"en", "de"} ),

		ex_handler(&Exceptions::on_auth_failed),
		ex_handler(&Exceptions::on_login_failed),
		ex_handler(&Exceptions::on_registration_failed),
		ex_handler(&Exceptions::on_std_ex),

		singleton<AppConfig(FrontController)>(),
		singleton<SessionPool(AppConfig)>(),
		singleton<UserPool(AppConfig)>(),

		singleton<SessionRepository(SessionPool)>(),
		singleton<UserRepository(UserPool)>(),

		singleton<Model(SessionRepository,UserRepository)>(),
		singleton<View(AppConfig,I18N)>(),
		singleton<Controller(Model,View)>(),

		singleton<EventBus()>(),
		singleton<WebSocketController(SessionRepository,EventBus)>(),

		singleton<Exceptions(View)>()
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
