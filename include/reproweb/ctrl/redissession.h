#ifndef INCLUDE_PROMISE_WEB_CONTROLLER_REDIS_SESSION_H_
#define INCLUDE_PROMISE_WEB_CONTROLLER_REDIS_SESSION_H_

#include "reproweb/ctrl/session.h"
#include "reproredis/redis.h"

namespace reproweb  
{


class RedisSessionProvider
{
public:

    RedisSessionProvider( std::shared_ptr<reproredis::RedisPool> r)
        : redis_(r)
    {}

    virtual ~RedisSessionProvider() {}


    virtual repro::Future<Session> get_session(const std::string& sid)
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
		auto p = repro::promise<Session>();

		Session session(sid,json);

		redis->cmd("SET", session.sid(), session.profile() )
		.then([p,this,session](reproredis::RedisResult::Ptr reply)
		{
			return redis->cmd("EXPIRE", session.sid(), 60);
		})
		.then([p,session](reproredis::RedisResult::Ptr reply)
		{
			p.resolve(session);
		})
		.otherwise(reject(p));

		return p.future();
    }


	virtual Future<> remove_user_session(const std::string& sid)
	{
		auto p = repro::promise<>();

		redis->cmd("DEL", sid)
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


class SessionFilter
{
public:

    SessionFilter(std::shared_ptr<SessionProvider> sp)
        : sid_cookie_name_("repro_web_sid"), session_provider_(sp)
    {}

    SessionFilter(const std::string& cn, std::shared_ptr<SessionProvider> sp)
        : sid_cookie_name_(cn), session_provider_(sp)
    {}    

    void filter( prio::Request& req, prio::Response& res, std::shared_ptr<FilterChain> chain)
    {
        auto json = std::make_shared<Json::Value>(get_session(req,res));

        req.attributes.set("_session_", json );

        chain->next(req,res);
    }

private:

    std::string sid_cookie_name_;
    std::shared_ptr<SessionProvider> session_provider_;

	static const Json::Value get_session(prio::Request& req, prio::Response& res)
	{
        const Cookies& cookies = req.headers.cookies();
		if(!cookies.exists(sid_cookie_name_))
		{
			std::string sid = make_session_id();
            res.cookie(Cookie(sid_cookie_name_,sid));
            return session_provider_->get_session(sid);
		}

		return session_provider_->get_session(cookies.get(sid_cookie_name_).value());
	}

	static std::string make_session_id()
	{
		std::string sid = sid_cookie_name_ + "::";
		sid += cryptoneat::toHex(cryptoneat::nonce(64));
		return sid;
	}    

};

Json::Value& session(prio::Request& req)
{
    std::shared_ptr<Json::Value> json = req.attributes.attr<std::shared_ptr<Json::Value>>("_session_");
    return *json;
} 


}

#endif
