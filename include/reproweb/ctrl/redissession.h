#ifndef INCLUDE_PROMISE_WEB_CONTROLLER_REDIS_SESSION_H_
#define INCLUDE_PROMISE_WEB_CONTROLLER_REDIS_SESSION_H_

#include "reproweb/ctrl/session.h"
#include "reproredis/redis.h"

namespace reproweb  
{


class RedisSessionProvider : public SessionProvider
{
public:

    RedisSessionProvider( std::shared_ptr<reproredis::RedisPool> r)
        : redis_(r)
    {}

    virtual ~RedisSessionProvider() {}


    virtual repro::Future<Session> get_session( std::string sid)
    {
		auto p = repro::promise<Session>();

        auto session = std::make_shared<Session>(sid,Json::Value(Json::nullValue));

		redis->cmd("GET", sid) 
		.then([p,session](reproredis::RedisResult::Ptr reply)
		{
			if(reply->isError() || reply->isNill())
			{
				p.reject(repro::Ex("invalid session"));
				return;
			}

			std::string payload = reply->str();
			Json::Value json = reproweb::JSON::parse(payload);

            *session = Session(session->sid,json);
			
            return redis->cmd("EXPIRE", session->sid(), 60);
		})
		.then([p,session](reproredis::RedisResult::Ptr reply)
		{
			p.resolve( *session );
		})        
		.otherwise(reject(p));

		return p.future();
    }

    virtual repro::Future<> set_session(const std::string& sid, Json::Value json)
    {
		auto p = repro::promise<>();

		Session session(sid,json);

		redis->cmd("SET", session.sid(), session.data() )
		.then([p,this,session](reproredis::RedisResult::Ptr reply)
		{
			return redis->cmd("EXPIRE", session.sid(), 60);
		})
		.then([p,session](reproredis::RedisResult::Ptr reply)
		{
			p.resolve();
		})
		.otherwise(reject(p));

		return p.future();
    }


	virtual repro::Future<> remove_user_session(Session session)
	{
		auto p = repro::promise<>();

		redis->cmd("DEL", session->sid)
		.then([p](reproredis::RedisResult::Ptr reply)
		{
			p.resolve();
		})
		.otherwise(reject(p));

		return p.future();
	}

private:
    std::shared_ptr<reproredis::RedisPool> redis_;
};



}

#endif
