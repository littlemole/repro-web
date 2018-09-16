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
		auto p = promise<Session>();

		auto payload = std::make_shared<std::string>();

		redis->cmd("GET", sid)
		.then([this,sid,payload](reproredis::RedisResult::Ptr reply)
		{
			if(reply->isError() || reply->isNill())
			{
				throw NoSessionEx("invalid session");
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

		auto session = std::make_shared<Session>(toJson(user));

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


 

struct SessionPool : public reproredis::RedisPool
{
	SessionPool(std::shared_ptr<Config> config) 
	  : RedisPool(config->getString("redis")) 
	{}
};


#endif
