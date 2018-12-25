#ifndef INCLUDE_PROMISE_WEB_CONTROLLER_SESSION_H_
#define INCLUDE_PROMISE_WEB_CONTROLLER_SESSION_H_

#include "reproweb/ctrl/controller.h"
#include "reproweb/ctrl/filter.h"
#include "reproweb/json/json.h"
#include "cryptoneat/cryptoneat.h"
#include <ctime>

namespace reproweb  
{


class Session
{
public:

    Session() 
    {}

	Session(const std::string& id,Json::Value p) 
		:sid(id), data(p)
	{}

    bool valid = true;
    bool authenticated = false;
	std::string sid;
	Json::Value data;	

private:
};

class SessionProvider
{
public:

    virtual ~SessionProvider() {}

    virtual repro::Future<Session> get_session( std::string sid) = 0;
    virtual repro::Future<> set_session(Session session) = 0;
    virtual repro::Future<> remove_user_session(Session session) = 0;
};

class MemorySessionProvider : public SessionProvider
{
public:

    MemorySessionProvider()
        : session_timeout_(60*5)
    {
        cleanup();
    }


    MemorySessionProvider( int timeout)
        : session_timeout_(timeout)
    {
        cleanup();
    }

    virtual ~MemorySessionProvider() 
    {
        timer_.cancel();
    }

    virtual repro::Future<Session> get_session(std::string sid)
    {
        auto p = repro::promise<Session>();

        prio::nextTick( [this,p,sid]() 
        {
            if(map_.count(sid) == 0)
            {
                //std::cout << "NEW SESSION "  << std::endl;
                auto json = Json::Value(Json::objectValue);
                map_[sid] = Session(sid,json);
            }

           // std::cout << "GET SESSION: " << map_[sid].authenticated << " " << JSON::stringify(map_[sid].data) << std::endl;
            timeouts_[sid] = now() + session_timeout_;
            p.resolve(map_[sid]);
        });

        return p.future();
    }

    virtual repro::Future<> set_session(Session session)
    {
        auto p = repro::promise<>();

       // std::cout << "SET SESSION: " << session.authenticated << " " << JSON::stringify(session.data) << std::endl;

        prio::nextTick( [this,p,session]() 
        {
            map_[session.sid] = session;
            timeouts_[session.sid] = now() + session_timeout_;
            p.resolve();
        });

        return p.future();
    }

    virtual repro::Future<> remove_user_session(Session session)
    {
        auto p = repro::promise<>();

       // std::cout << "DEL SESSION: " << session.authenticated << " " << JSON::stringify(session.data) << std::endl;

        prio::nextTick( [this,p,session]() 
        {        
            if(map_.count(session.sid)!=0)
            {
                map_.erase(session.sid);
                timeouts_.erase(session.sid);
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

        timer_.after([this]()
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


    std::map<std::string,Session> map_;    
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

    void before( prio::Request& req, prio::Response& res, std::shared_ptr<FilterChain> chain)
    {
        get_session(req,res)
        .then([this,&req,&res,chain]( Session session ) 
        {
            req.attributes.set("_session_", std::make_shared<Session>(session) );

            chain->next(req,res);
        })
        .otherwise([this,&req,&res,chain]( const std::exception& ex)
        {
            res.error().body(ex.what()).flush();
        });
    }


    void after( prio::Request& req, prio::Response& res, std::shared_ptr<FilterChain> chain)
    {
        auto session = req.attributes.attr<std::shared_ptr<Session>>("_session_");
        if ( session->valid)
        {
            std::cout << "session valid" << std::endl;
            // std::cout << "SESSION: " << session->authenticated << " " << JSON::stringify(session->data) << std::endl;

            session_provider_->set_session(*session)
            .then([&req,&res,chain]()
            {
                chain->next(req,res);     
            })
            .otherwise([&req,&res,chain]( const std::exception& ex)
            {
                //res.error().body(ex.what()).flush();
            });
        }
        else
        {
            std::cout << "session invalid" << std::endl;
            session_provider_->remove_user_session(*session)
            .then([&req,&res,chain]()
            {
                chain->next(req,res);     
            })
            .otherwise([&req,&res,chain]( const std::exception& ex)
            {
                //res.error().body(ex.what()).flush();
            });
        }
    }    

private:

    std::string sid_cookie_name_;
    std::shared_ptr<SessionProvider> session_provider_;

	repro::Future<Session> get_session(prio::Request& req, prio::Response& res)
	{
        const prio::Cookies& cookies = req.headers.cookies();

        std::string sid;
		if(cookies.exists(sid_cookie_name_))
		{
            sid = cookies.get(sid_cookie_name_).value();
        }

        if(sid.empty())
        {
            sid = make_session_id();
        }

        // move to flush
        res.cookie(prio::Cookie(sid_cookie_name_,sid));

		return session_provider_->get_session(sid);
	}

	std::string make_session_id()
	{
		std::string sid = sid_cookie_name_ + "::";
		sid += cryptoneat::toHex(cryptoneat::nonce(64));
		return sid;
	}    

};

template<class F = SessionFilter>
struct session_filter
{
    session_filter(const std::string& m, const std::string& p, int prio = 0 )
        : before(m,p,&F::before,prio), 
          after(m,p,&F::after,prio)
    {}


	void ctx_register(diy::Context* ctx)
	{
        before.ctx_register(ctx);
        after.ctx_register(ctx);
	}    

    filter_router<decltype(& F::before)> before;
    flush_filter_router<decltype(& F::after)> after;
};

inline std::shared_ptr<Session> req_session(prio::Request& req)
{
    if(!req.attributes.exists("_session_"))
    {
        auto json = Json::Value(Json::objectValue);
        return std::make_shared<Session>("",json);
    }
    return req.attributes.attr<std::shared_ptr<Session>>("_session_");
}

inline bool is_authenticated(prio::Request& req)
{
    return req_session(req)->authenticated;
}


inline void invalidate_session(prio::Request& req)
{
    auto s = req_session(req);
    s->valid = false;
    s->authenticated = false;
}

}

#endif
