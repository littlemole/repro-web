
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
		auto p = repro::promise<Json::Value>();

		if(!cookies.exists("repro_web_sid"))
		{
			return rejected(p,repro::Ex("no session"));
		}

		std::string sid = cookies.get("repro_web_sid").value();

		redis->cmd("GET", sid)
		.then([p](Reply reply)
		{
			if(reply.isError() || reply.isNil())
			{
				p.reject(repro::Ex("invalid session"));
				return;
			}

			std::string payload = reply.asString();
			Json::Value json = JSON::parse(payload);

			p.resolve(json);
		})
		.otherwise([p](const std::exception& ex)
		{
			p.reject(ex);
		});

		return p.future();
	}

	std::string write(Json::Value profile)
	{
		std::string sid = make_new_sid();

		redis->cmd("SET", sid, JSON::stringify(profile))
		.then([this,sid](Reply reply)
		{
			redis->cmd("EXPIRE", sid, 60);
		});

		return sid;
	}

	void remove(const Cookies& cookies)
	{
		if(!cookies.exists("repro_web_sid"))
		{
			return;
		}

		std::string sid = cookies.get("repro_web_sid").value();

		redis->cmd("DEL", sid)
		.then([](Reply reply){});
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
		auto p = repro::promise<Json::Value>();

		Password pass;
		std::string hash = pass.hash(pwd);

		Json::Value result(Json::objectValue);
		result["username"]   = username;
		result["login"] 	 = login;
		result["pwd"] 		 = hash;
		result["avatar_url"] = avatar_url;
		 
		sqlite->query(
			"INSERT INTO users (username,login,pwd,avatar_url) VALUES ( ? , ? , ? , ? )",
			username,login,hash,avatar_url
		)
		.then( [p,result](Result r) 
		{
			p.resolve(result);
		})
		.otherwise( [p]( const std::exception& ex)
		{
			p.reject(ex);
		});
		return p.future();
	}

	Future<Json::Value> getUser( const std::string& login )
	{
		auto p = repro::promise<Json::Value>();

		sqlite->query(
			"SELECT username,login,pwd,avatar_url FROM users WHERE login = ? ;",
			login
		)
		.then( [p](Result r) 
		{
			if ( r.rows() < 1) throw repro::Ex("user login not found");

			Json::Value result(Json::objectValue);
			result["username"]   = r[0][0];
			result["login"] 	 = r[0][1];
			result["pwd"] 		 = r[0][2];
			result["avatar_url"] = r[0][3];

			p.resolve(result);
		})
		.otherwise( [p]( const std::exception& ex)
		{
			p.reject(ex);
		});
		return p.future();
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

	void index( Request& req, Response& res)
	{

		sessionRepository->fetch_user_session(req.headers.cookies())
		.then( [&res](Json::Value profile)
		{
			Json::Value removed;
			profile.removeMember("pwd",&removed);

			res
			.body(render("index", profile ))
			.contentType("text/html")
			.ok()
			.flush();

		})
		.otherwise([&res](const std::exception& ex)
		{
			res
			.redirect("https://localhost:9876/login.html")
			.flush();
		});
		
	}

	void login( Request& req, Response& res)
	{
		QueryParams qp(req.body());
		std::string login = qp.get("login");
		std::string pwd   = qp.get("pwd");

		userRepository->getUser(login)
		.then( [this,&res,pwd](Json::Value user)
		{
			Password pass;
			bool verified = pass.verify(pwd, user["pwd"].asString() );

			if(!verified) throw repro::Ex("invalid login/password combination");

			std::string sid = sessionRepository->write(user);

			res
			.cookie(Cookie("repro_web_sid", sid))
			.redirect("https://localhost:9876/")
			.flush();
		})
		.otherwise( [&res](const std::exception& ex)
		{
			res
			.redirect("https://localhost:9876/login.html")
			.flush();
		});
	}

	void logout( Request& req, Response& res)
	{
		sessionRepository->remove(req.headers.cookies());

		res
		.redirect("https://localhost:9876/login.html")
		.flush();		
	}

	void register_user( Request& req, Response& res)
	{
		QueryParams qp(req.body());
		std::string username   = qp.get("username");
		std::string login      = qp.get("login");
		std::string pwd        = qp.get("pwd");
		std::string avatar_url = qp.get("avatar_url");

		userRepository->register_user(username,login,pwd,avatar_url)
		.then( [this,&res](Json::Value user)
		{
			std::string sid = sessionRepository->write(user);

			res
			.cookie(Cookie("repro_web_sid", sid))
			.redirect("https://localhost:9876/")
			.flush();
		})
		.otherwise( [&res](const std::exception& ex)
		{
			res
			.redirect("https://localhost:9876/login.html")
			.flush();
		});		
	}

private:

	std::shared_ptr<SessionRepository> sessionRepository;
	std::shared_ptr<UserRepository> userRepository;
};


DIY_DEFINE_CONTEXT()

typedef Application components;

struct SessionPool : public RedisPool
{
	SessionPool() : RedisPool("redis://localhost:6379") {}
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
		singleton<SessionPool()>(),
		singleton<UserPool()>(),
		singleton<SessionRepository(SessionPool)>(),
		singleton<UserRepository(UserPool)>(),
		singleton<ExampleController(SessionRepository,UserRepository)>()
	),
 
	static_content("/htdocs/","/etc/mime.types")
);

void server()
{

	SSLUser useSSL;
	Http2SslCtx sslCtx;
	sslCtx.load_cert_pem("pem/server.pem");
	//sslCtx.enableHttp2();
	templates().load("/view");

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
