#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_REPO_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_REPO_DEFINE_

#include "reproredis/redis.h"
#include "reprosqlite/sqlite.h"
#include "reproweb/json/json.h"
#include "model.h"



class SessionRepository
{
public:

	SessionRepository(std::shared_ptr<reproredis::RedisPool> redisPool)
		: redis(redisPool)
	{}

	Future<Session> get_user_session( std::string sid)
	{
		try
		{
			reproredis::RedisResult::Ptr reply = co_await redis->cmd("GET", sid);

			if(reply->isError() || reply->isNill())
			{
				throw AuthEx("invalid session");
			}

			std::string payload = reply->str();
			Json::Value json = reproweb::JSON::parse(payload);

			reproredis::RedisResult::Ptr reply2 = co_await redis->cmd("EXPIRE", sid, 180);

			co_return Session(sid,json);
		}
		catch(const std::exception& ex)
		{
			std::cout << "inside get_user_Session: " << ex.what() << std::endl;
			throw AuthEx(ex.what());
		}
	}

	Future<Session> write_user_session(User user)
	{
		try
		{
			Session session(toJson(user));

			reproredis::RedisResult::Ptr reply = co_await redis->cmd("SET", session.sid(), session.profile());
			reproredis::RedisResult::Ptr reply2 = co_await redis->cmd("EXPIRE", session.sid(), 180);

			co_return session;
		}
		catch(const std::exception& ex)
		{
			throw AuthEx(ex.what());
		}
	}

	Future<> remove_user_session( std::string sid)
	{
		try
		{
			reproredis::RedisResult::Ptr reply = co_await redis->cmd("DEL", sid);

		}
		catch(const std::exception& ex)
		{
			throw AuthEx(ex.what());
		}			
		co_return;
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
		try
		{
			if(username.empty() || login.empty() || pwd.empty())
			{
				throw RegistrationEx("username, login and password may not be empty");
			}

			cryptoneat::Password pass;
			std::string hash = pass.hash(pwd);

			User result(username,login,hash,avatar_url);

			try
			{		 
				reprosqlite::Result r = co_await sqlite->query(
					"INSERT INTO users (username,login,pwd,avatar_url) VALUES ( ? , ? , ? , ? )",
					username,login,hash,avatar_url
				);

			}
			catch(const std::exception& ex)
			{
				throw RegistrationEx("error.msg.login.alreaady.taken");
			}

			co_return result;
		}
		catch(const std::exception& ex)
		{
			throw RegistrationEx(ex.what());
		}
	}

	Future<User> get_user( const std::string& login )
	{
		try
		{
			reprosqlite::Result r = co_await sqlite->query(
				"SELECT username,login,pwd,avatar_url FROM users WHERE login = ? ;",
				login
			);

			if ( r.rows() < 1) throw repro::Ex("user not found");

			User result(
				r[0][0],
				r[0][1],
				r[0][2],
				r[0][3]
			);

			co_return result;
		}
		catch(const std::exception& ex)
		{
			throw LoginEx("error.msg.login.failed");
		}
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
