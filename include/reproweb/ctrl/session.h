#ifndef INCLUDE_PROMISE_WEB_CONTROLLER_SESSION_H_
#define INCLUDE_PROMISE_WEB_CONTROLLER_SESSION_H_

#include "reproweb/ctrl/controller.h"
#include "reproweb/ctrl/filter.h"
#include "cryptoneat/cryptoneat.h"
#include <ctime>

namespace reproweb  
{


class Session
{
public:
	Session(Json::Value profile) 
		:sid_(make_session_id()), profile_(profile)
	{}

	Session(const std::string& sid,Json::Value profile) 
		:sid_(sid), profile_(profile)
	{}

	const std::string& sid() const  { return sid_; }
	Json::Value profile() const     { return profile_; }

private:
	std::string sid_;
	Json::Value profile_;	
};

class SessionProvider
{
public:

    virtual ~SessionProvider() {}

    virtual Session get_session(const std::string& sid) = 0;
    virtual void set_session(const std::string& sid, Json::Value json) = 0;
    virtual Future<> remove_user_session(const std::string& sid) = 0;
};

class MemorySessionProvider
{
public:

    MemorySessionProvider()
        : session_timeout_(60*5)
    {
        cleanup()
    }


    MemorySessionProvider( int timeout)
        : session_timeout_(timeout)
    {
        cleanup();
    }

    virtual ~MemorySessionProvider() 
    {
        timer_->cancel();
    }

    virtual repro::Future<Session> get_session(const std::string& sid)
    {
        auto p = repro::promise<Session>();

        nextTick( [this,p,sid]() 
        {
            if(map_.count(sid) == 0)
            {
                map_[sid] = Json::Value(Json::objectValue);
            }
            timeouts_[sid] = now() + session_timeout_;
            p.resolve(Session(sid,map_[sid]);
        });

        return p.future();
    }

    virtual repro::Future<> set_session(const std::string& sid, Json::Value json)
    {
        auto p = repro::promise<>();

        nextTick( [this,p,sid,json]() 
        {
            map_[sid] = json;
            timeouts_[sid] = now() + session_timeout_;
            p.resolve();
        });

        return p.future();
    }

    virtual Future<> remove_user_session(const std::string& sid)
    {
        auto p = repro::promise<>();

        nextTick( [this,p,sid,json]() 
        {        
            if(map_count(sid)!=0)
            {
                map_.erase(sid);
                timeouts_.erase(sid);
            }
            p.resolve();
        });

        return p.future();
    }

private:

    prio::Timeout timer_;

    void cleanup()
    {
        int n = now();

        std::vector<std::string> purgatory;

        for( auto& it : timeouts_)
        {
            if ( n > it.second)
            {
                purgatory.push_back(it.first);
            }
        }

        for( auto& k : purgatory)
        {
            map_.erase(k);
            timeouts_.erase(k);
        }

        timer.after([this]()
        {
            cleanup();
        }, 1000*60*5);
    }

    int session_timeout_;

    int now()
    {
        std::time_t t = std::time(0);
        return t;
    }

    std::map<std::string,Json::Value> map_;    
    std::map<std::string,int> timeouts_;    
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
        get_session(req,res)
        .then([this,&req,&res,chain]( Session session ) 
        {
            req.attributes.set("_session_", session );
            req.attributes.set("_session_filter_", this );

            chain->next(req,res);
        });
    }

    void save_session(prio::Request& req)
    {
        Session session = req.attributes.attr<Session>("_session_");
        session_provider_->set_session(session.sid(), session.profile());
    }

private:

    std::string sid_cookie_name_;
    std::shared_ptr<SessionProvider> session_provider_;

	static Future<Json::Value> get_session(prio::Request& req, prio::Response& res)
	{
        const Cookies& cookies = req.headers.cookies();

        std::string sid;
		if(!cookies.exists(sid_cookie_name_))
		{
			sid = make_session_id();
		}
        else
        {
            sid = cookies.get(sid_cookie_name_).value()
        }

        res.cookie(Cookie(sid_cookie_name_,sid));

		return session_provider_->get_session(sid);
	}

	static std::string make_session_id()
	{
		std::string sid = sid_cookie_name_ + "::";
		sid += cryptoneat::toHex(cryptoneat::nonce(64));
		return sid;
	}    

};

void save_session(prio::Request& req)
{
    if(!req.attributes.exists("_session_filter_"))
    {
        return;
    }

    SessionFilter* sf = req.attributes.attr<SessionFilter*>("_session_filter_");

    sf->save_session();
}


}

#endif
