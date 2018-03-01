#ifndef DEFINE_MOL_HTTP_SERVER_DEF_GUARD_DEFINE_
#define DEFINE_MOL_HTTP_SERVER_DEF_GUARD_DEFINE_

#include "priohttp/common.h"
#include "priohttp/http_server.h"
#include "priocpp/api.h"

namespace reproweb  {


class WebServer
{
public:

    WebServer();
    WebServer(prio::SslCtx& ctx);
    ~WebServer();    
    
    int run(int port);

    void shutdown();

private:
    std::unique_ptr<prio::http_server> server_;
};


reproweb::WebServer& webServer();

}


#endif



