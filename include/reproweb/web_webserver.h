#ifndef DEFINE_MOL_HTTP_SERVER_DEF_GUARD_DEFINE_
#define DEFINE_MOL_HTTP_SERVER_DEF_GUARD_DEFINE_

#include "priohttp/common.h"
#include "priohttp/http_server.h"
#include "priocpp/api.h"
#include "diycpp/ctx.h"
#include "reproweb/ctrl/front_controller.h"

namespace reproweb  {


class WebServer
{
public:

    WebServer(diy::Context& ctx);
    ~WebServer();    
    
    int listen(int port);
    int listen(prio::SslCtx& ssl,int port);

    void shutdown();

private:

    diy::Context& ctx_;
    std::vector<std::unique_ptr<prio::http_server>> servers_;
};

}

#endif



