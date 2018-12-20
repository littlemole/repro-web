#ifndef INCLUDE_PROMISE_WEB_CONTROLLER_SESSION_H_
#define INCLUDE_PROMISE_WEB_CONTROLLER_SESSION_H_

#include "reproweb/ctrl/controller.h"
#include "reproweb/ctrl/filter.h"
#include "cryptoneat/cryptoneat.h"

namespace reproweb  
{

class SessionProvider
{
public:

    virtual ~SessionProvider() {}

    virtual Json::Value get_session(const std::string& sid) = 0;
    virtual void set_session(const std::string& sid, Json::Value json) = 0;
};

class MemorySessionProvider
{
public:

    virtual ~MemorySessionProvider() {}

    virtual Json::Value get_session(const std::string& sid)
    {
        if(map_.count(sid) == 0)
        {
            return Json::Value(Json::objectValue);
        }
        return map_[sid];
    }

    virtual void set_session(const std::string& sid, Json::Value json)
    {
        map_[sid] = json;
    }

private:
    std::map<std::string,Json::Value> map_;    
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
