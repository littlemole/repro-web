#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_REPO_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_REPO_DEFINE_

#include "reproredis/redis.h"
#include "reprosqlite/sqlite.h"
#include "model.h"

using namespace prio;
using namespace repro;
using namespace reproweb;


class SessionRepository
{
public:

	SessionRepository(std::shared_ptr<reproredis::RedisPool> redisPool)
		: redis(redisPool)
	{}

	Future<Session> get_user_session( std::string sid)
	{
		auto p = promise<Session>();

		auto payload = std::make_shared<std::string>();

		redis->cmd("GET", sid)
		.then([this,sid,payload](reproredis::RedisResult::Ptr reply)
		{
			if(reply->isError() || reply->isNill())
			{
				throw AuthEx("invalid session");
			}

			*payload = reply->str();

			return redis->cmd("EXPIRE", sid, 180);
		})
		.then([p,sid,payload](reproredis::RedisResult::Ptr reply)
		{
			Json::Value json = reproweb::JSON::parse(*payload);
			p.resolve(Session(sid,json));
		})
		.otherwise(reject(p));

		return p.future();
	}

	Future<Session> write_user_session(User user)
	{
		auto p = promise<Session>();

		auto session = std::make_shared<Session>(user.toJson());

		redis->cmd("SET", session->sid(), session->profile())
		.then([this,session](reproredis::RedisResult::Ptr reply)
		{
			return redis->cmd("EXPIRE", session->sid(), 180);
		})
		.then([p,session](reproredis::RedisResult::Ptr reply)
		{
			p.resolve(*session);
		})
		.otherwise(reject(p));

		return p.future();
	}

	Future<> remove_user_session( std::string sid)
	{
		auto p = promise<>();

		redis->cmd("DEL", sid)
		.then([p](reproredis::RedisResult::Ptr reply)
		{
			p.resolve();
		})
		.otherwise(reject(p));

		return p.future();
	}

private:

	std::shared_ptr<reproredis::RedisPool> redis;
};




class UserRepository
{
public:

	UserRepository(std::shared_ptr<reprosqlite::SqlitePool> sqlitePool)
		: sqlite(sqlitePool)
	{}

	Future<User> register_user(
		const std::string& username, 
		const std::string& login, 
		const std::string& pwd, 
		const std::string& avatar_url )
	{
		auto p = promise<User>();

		if(username.empty() || login.empty() || pwd.empty())
		{
			nextTick( [p]() 
			{
				p.reject(RegistrationEx("username, login and password may not be empty"));
			});

			return p.future();
		}

		cryptoneat::Password pass;
		std::string hash = pass.hash(pwd);

		auto result = std::make_shared<User>(username,login,hash,avatar_url);

		sqlite->query(
					"INSERT INTO users (username,login,pwd,avatar_url) VALUES ( ? , ? , ? , ? )",
					username,login,hash,avatar_url
		)
		.then([p,result](reprosqlite::Result r)
		{
			p.resolve(*result);
		})
		.otherwise([p](const std::exception& ex)
		{
			p.reject(RegistrationEx("error.msg.login.alreaady.taken"));
		});

		return p.future();
	}

	Future<User> get_user( const std::string& login )
	{
		auto p = promise<User>();

		sqlite->query(
				"SELECT username,login,pwd,avatar_url FROM users WHERE login = ? ;",
				login
		)
		.then([p](reprosqlite::Result r)
		{
			if ( r.rows() < 1) throw repro::Ex("user not found");

			User result(
				r[0][0],
				r[0][1],
				r[0][2],
				r[0][3]
			);
			p.resolve(result);
		})
		.otherwise([p](const std::exception& ex)
		{
			p.reject(LoginEx("error.msg.login.failed"));
		});

		return p.future();
	}

private:

	std::shared_ptr<reprosqlite::SqlitePool> sqlite;
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

#endif
