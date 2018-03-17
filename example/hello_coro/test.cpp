
#include "reprocpp/after.h"
        
#include "test.h"
#include "reproredis/redis.h"
#include "priocpp/api.h"
#include "priocpp/task.h"
#include "reprocurl/asyncCurl.h"
#include "reprocurl/urlencode.h"
#include "reproweb/ctrl/controller.h"
#include "reproweb/view/tpl.h"
#include "reproweb/web_webserver.h"
#include "cryptoneat/cryptoneat.h"
#include "reprosqlite/sqlite.h"
#include <signal.h>
      
using namespace prio;
using namespace reproweb;
using namespace reprocurl;
using namespace reproredis;
using namespace reprosqlite;
using namespace cryptoneat;
using namespace diy;
 
 
class SessionRepository
{
public: 

	SessionRepository(std::shared_ptr<RedisPool> redisPool)
		: redis(redisPool)
	{}

	Future<Json::Value> fetch_user_session(const Cookies& cookies)
	{
		if(!cookies.exists("repro_web_sid"))
		{
			throw repro::Ex("no session");
		}

		std::string sid = cookies.get("repro_web_sid").value();

		Reply reply = co_await redis->cmd("GET", sid);

		if(reply.isError() || reply.isNil())
		{
			throw repro::Ex("invalid session");
		}

		std::string payload = reply.asString();
		Json::Value json = JSON::parse(payload);

		co_return json;
	}

	Future<std::string> write(Json::Value profile)
	{
		std::string sid = make_new_sid();

		co_await redis->cmd("SET", sid, JSON::stringify(profile));

		co_await redis->cmd("EXPIRE", sid, 60);

		co_return sid;
	}
 
	Future<> remove(const Cookies& cookies)
	{
		if(!cookies.exists("repro_web_sid"))
		{
			co_return;
		}

		std::string sid = cookies.get("repro_web_sid").value();

		co_await redis->cmd("DEL", sid);
	}

private:

	std::string make_new_sid()
	{
		std::string sid = "repro_web_sid::";
		sid += toHex(cryptoneat::nonce(64));
		return sid;
	}

	std::shared_ptr<RedisPool> redis;
};

class UserRepository
{
public:

	UserRepository(std::shared_ptr<SqlitePool> sqlitePool)
		: sqlite(sqlitePool)
	{}

	Future<Json::Value> register_user( 
		const std::string& username, 
		const std::string& login, 
		const std::string& pwd, 
		const std::string& avatar_url )
	{
		Password pass;
		std::string hash = pass.hash(pwd);

		Json::Value result(Json::objectValue);
		result["username"]   = username;
		result["login"] 	 = login;
		result["pwd"] 		 = hash;
		result["avatar_url"] = avatar_url;
		 
		Result r = co_await sqlite->query(
			"INSERT INTO users (username,login,pwd,avatar_url) VALUES ( ? , ? , ? , ? )",
			username,login,hash,avatar_url
		);

		co_return result;
	}

	Future<Json::Value> getUser( const std::string& login )
	{
		Result r = co_await sqlite->query(
			"SELECT username,login,pwd,avatar_url FROM users WHERE login = ? ;",
			login
		);

		if ( r.rows() < 1) throw repro::Ex("user login not found");

		Json::Value result(Json::objectValue);
		result["username"]   = r[0][0];
		result["login"] 	 = r[0][1];
		result["pwd"] 		 = r[0][2];
		result["avatar_url"] = r[0][3];

		co_return result;
	}

private:

	std::shared_ptr<SqlitePool> sqlite;
};


class ExampleController
{
public:

	ExampleController( std::shared_ptr<SessionRepository> sessionRepo, std::shared_ptr<UserRepository> userRepo )
		: sessionRepository(sessionRepo), userRepository(userRepo)
	{}

	Async index( Request& req, Response& res)
	{
		try 
		{
			Json::Value profile = co_await sessionRepository->fetch_user_session(req.headers.cookies());

			Json::Value removed;
			profile.removeMember("pwd",&removed);

			res
			.body(render("index", profile ))
			.contentType("text/html")
			.ok()
			.flush();	
		}	
		catch(const std::exception& ex)
		{
			res
			.redirect("https://localhost:9876/login.html")
			.flush();
		}
	}

	Async login( Request& req, Response& res)
	{
		try 
		{
			QueryParams qp(req.body());
			std::string login = qp.get("login");
			std::string pwd   = qp.get("pwd");

			Json::Value user = co_await userRepository->getUser(login);

			Password pass;
			bool verified = pass.verify(pwd, user["pwd"].asString() );

			if(!verified) throw repro::Ex("invalid login/password combination");

			std::string sid = co_await sessionRepository->write(user);

			res
			.cookie(Cookie("repro_web_sid", sid))
			.redirect("https://localhost:9876/")
			.flush();
		}
		catch(const std::exception& ex)
		{
			res
			.redirect("https://localhost:9876/login.html")
			.flush();
		}
	}

	Async logout( Request& req, Response& res)
	{
		co_await sessionRepository->remove(req.headers.cookies());

		res
		.redirect("https://localhost:9876/login.html")
		.flush();		
	}

	Async register_user( Request& req, Response& res)
	{
		try
		{
			QueryParams qp(req.body());
			std::string username   = qp.get("username");
			std::string login      = qp.get("login");
			std::string pwd        = qp.get("pwd");
			std::string avatar_url = qp.get("avatar_url");

			Json::Value user = co_await userRepository->register_user(username,login,pwd,avatar_url);
			std::string sid = co_await sessionRepository->write(user);

			res
			.cookie(Cookie("repro_web_sid", sid))
			.redirect("https://localhost:9876/")
			.flush();
		}
		catch(const std::exception& ex)
		{
			res
			.redirect("https://localhost:9876/login.html")
			.flush();
		}
	}

private:

	std::shared_ptr<SessionRepository> sessionRepository;
	std::shared_ptr<UserRepository> userRepository;
};


DIY_DEFINE_CONTEXT()

typedef Application components;

struct Config
{
	Config()
	{
		const char* redis = getenv("REDIS_HOST");
		if(!redis)
		{
			redis = "localhost";
		}

		std::ostringstream oss;
		oss << "redis://" << redis << ":6379";

		redis_host = oss.str();
	}

	std::string redis_host;
};

struct SessionPool : public RedisPool
{
	SessionPool(std::shared_ptr<Config> config) : RedisPool(config->redis_host) {}
};

struct UserPool : public SqlitePool
{
	UserPool() : SqlitePool("./user.db") {}
};


Application depends(

	http_controller(

		GET  ( "/",				&ExampleController::index),
		GET  ( "/logout",		&ExampleController::logout),
		POST ( "/login",		&ExampleController::login),
		POST ( "/register",		&ExampleController::register_user)
	),
 
	components(
		singleton<Config()>(),
		singleton<SessionPool(Config)>(),
		singleton<UserPool()>(),
		singleton<SessionRepository(SessionPool)>(),
		singleton<UserRepository(UserPool)>(),
		singleton<ExampleController(SessionRepository,UserRepository)>()
	),
 
	view_path("/view"),
	static_content("/htdocs/","/etc/mime.types")
); 
 
void server() 
{ 
 
	SSLUser useSSL;
	Http2SslCtx sslCtx;
	sslCtx.load_cert_pem("pem/server.pem");
	//sslCtx.enableHttp2();

	WebServer server(sslCtx);

	signal(SIGPIPE).then( [](int s) {} );
	signal(SIGINT).then( [&server](int s) 
	{ 
		server.shutdown();
		nextTick([]
		{
			theLoop().exit(); 
		});
	});

	server.run(9876);
}


int main(int argc, char **argv)
{
	prio::init();
	server();

	MOL_TEST_PRINT_CNTS();	
    return 0;
}
