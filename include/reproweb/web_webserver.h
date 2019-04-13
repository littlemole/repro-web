#ifndef DEFINE_MOL_HTTP_SERVER_DEF_GUARD_DEFINE_
#define DEFINE_MOL_HTTP_SERVER_DEF_GUARD_DEFINE_

#include "priohttp/common.h"
#include "priohttp/http_server.h"
#include "priocpp/api.h"
#include "diycpp/ctx.h"
#include "reproweb/ctrl/front_controller.h"
#include "reproweb/ctrl/session.h"

namespace reproweb  {


class WebServer
{
public:

    WebServer(diy::Context& ctx);

    template<class T>
    WebServer& configure()
    {
        auto conf = diy::inject<T>(ctx_);
        config_ = conf->json();
        return *this;
    }

    ~WebServer();    
    
    int listen(int port);
    int listen(prio::SslCtx& ssl,int port);

    void listen()
    {
        run_config(config_);
    }

    void shutdown();


    template<class F = MemorySessionProvider>
    WebServer& session(const std::string& methods, const std::string& paths, int prio)
    {
        session_filter<F> sf(methods,paths,prio);

        sf.ctx_register(&ctx_);

        return *this;
    }

    template<class F = MemorySessionProvider>
    WebServer& session()
    {
        std::string methods = config_["session"]["verb"].asString();
        std::string paths = config_["session"]["path"].asString();
        int prio = config_["session"]["prio"].asInt();

        session_filter<F> sf(methods,paths,prio);

        sf.ctx_register(&ctx_);

        return *this;
    }

    template<class F>
    WebServer& filter(const std::string& methods, const std::string& paths, F f, int prio)
    {
        auto t = ::reproweb::filter(methods,paths,f,prio);

        t.ctx_register(&ctx_);

        return *this;
    }


    template<class F>
    WebServer& filter(const std::string& name, F f)
    {
        std::string methods = config_["filter"][name]["verb"].asString();
        std::string paths = config_["filter"][name]["path"].asString();
        int prio = config_["filter"][name]["prio"].asInt();

        auto t = ::reproweb::filter(methods,paths,f,prio);

        t.ctx_register(&ctx_);

        return *this;
    }

    WebServer& views(const std::string& methods);
    WebServer& htdocs(const std::string& methods);
    WebServer& htdocs(const std::string& methods, const std::string& mime);
    WebServer& i18n(const std::string& path,const std::vector<std::string>& locales);

private:

    Json::Value config_;

    void run_config(Json::Value json);

    diy::Context& ctx_;

    std::vector<std::unique_ptr<prio::http_server>> servers_;
    std::vector<std::unique_ptr<prio::Http2SslCtx>> sslContexts_;
};

}

#endif



