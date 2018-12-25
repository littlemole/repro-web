#ifndef INCLUDE_PROMISE_WEB_CONTROLLER_SESSION_H_
#define INCLUDE_PROMISE_WEB_CONTROLLER_SESSION_H_

#include "reproweb/ctrl/controller.h"
#include "reproweb/ctrl/filter.h"
#include "reproweb/json/json.h"
#include "reproweb/serialization/meta.h"
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

    auto meta() const
    {
        return metadata<Session>(
            "authenticated", &Session::authenticated,
            "sid", &Session::sid,
            "data", &Session::data
        );
    }

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

    MemorySessionProvider();
    MemorySessionProvider( int timeout);

    virtual ~MemorySessionProvider();

    virtual repro::Future<Session> get_session(std::string sid);
    virtual repro::Future<> set_session(Session session);
    virtual repro::Future<> remove_user_session(Session session);

private:

    void cleanup();
    int now();

    prio::Timeout timer_;
    int session_timeout_;

    std::map<std::string,Session> map_;    
    std::map<std::string,int> timeouts_;    
};


class SessionFilter
{
public:

    SessionFilter(std::shared_ptr<SessionProvider> sp);
    SessionFilter(const std::string& cn, std::shared_ptr<SessionProvider> sp);

    void before( prio::Request& req, prio::Response& res, std::shared_ptr<FilterChain> chain);
    void after( prio::Request& req, prio::Response& res, std::shared_ptr<FilterChain> chain);

private:

    std::string sid_cookie_name_;
    std::shared_ptr<SessionProvider> session_provider_;

	std::string make_session_id();
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
