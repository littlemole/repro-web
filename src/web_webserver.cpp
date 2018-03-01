#include "reproweb/web_webserver.h"
#include "reproweb/ctrl/front_controller.h"
#include "reproweb/ctrl/handler.h"
#include "reproweb/json/json.h"
#include "priohttp/http_server.h"
#include "priocpp/api.h"
#include <signal.h>
#include <iostream>

using namespace prio;

namespace reproweb  {



WebServer::WebServer()
    : server_( new prio::http_server )
{
}

WebServer::WebServer(prio::SslCtx& ctx)
    : server_( new prio::http_server(ctx) )
{
}

WebServer::~WebServer()
{
}

int WebServer::run(int port)
{
    int r = 0;
    try
    {
        server_
        ->bind(port)
        .then( [](prio::Request& req, prio::Response& res)
        {
        	reproweb::frontController().request_handler(req,res);
        })
		.otherwise([](const std::exception& ex)
        {
#ifdef MOL_PROMISE_DEBUG
        	std::cout << "ex:!" << ex.what() << std::endl;
#endif
        });
  
        theLoop().run();
    }
    catch( repro::Ex& ex)
    {
        std::cout << "ex: " << ex.msg << std::endl;
        r = 1;
    }  
    return r;
}



void WebServer::shutdown()
{
    server_->shutdown();
}


reproweb::WebServer& webServer()
{
    static reproweb::WebServer s;
    return s;
}


EventBus& eventBus()
{
	static EventBus bus;
	return bus;
}


} // end namespace csgi


