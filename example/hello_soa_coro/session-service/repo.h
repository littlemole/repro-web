#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_REPO_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_REPO_DEFINE_

#include "reproredis/redis.h"
#include "entities.h"

class SessionRepository
{
public:

	SessionRepository(std::shared_ptr<reproredis::RedisPool> redisPool)
		: redis(redisPool)
	{}

	Future<Session> get_user_session( std::string sid)
	{
		reproredis::RedisResult::Ptr reply = co_await redis->cmd("GET", sid);
		if(reply->isError() || reply->isNill())
		{
			throw NoSessionEx("invalid session");
		}

		std::string payload = reply->str();

		reply =	co_await redis->cmd("EXPIRE", sid, 180);

		Json::Value json = reproweb::JSON::parse(payload);
		co_return Session(sid,json);
	}

	Future<Session> write_user_session(User user)
	{
		Session session(toJson(user));

		reproredis::RedisResult::Ptr reply = co_await redis->cmd("SET", session.sid(), session.profile());

		reply = co_await redis->cmd("EXPIRE", session.sid(), 180);

		co_return session;
	}

	Future<> remove_user_session( std::string sid)
	{
		reproredis::RedisResult::Ptr reply = co_await redis->cmd("DEL", sid);

		co_return;
	}

private:

	std::shared_ptr<reproredis::RedisPool> redis;
};


 

struct SessionPool : public reproredis::RedisPool
{
	SessionPool(std::shared_ptr<Config> config) 
	  : RedisPool(config->getString("redis")) 
	{}
};


#endif
