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



WebServer::WebServer(diy::Context& ctx)
    : ctx_(ctx)
{

#ifndef _WIN32        
        signal(SIGPIPE).then( [](int s) {} );
#endif        
        signal(SIGINT).then( [this](int s) 
        { 
            shutdown();
            nextTick([]
            {
                theLoop().exit(); 
            });
        });    
}

WebServer::~WebServer()
{
} 

int WebServer::listen(int port)
{
    int r = 0;
    try
    {
        prio::http_server* server = new prio::http_server;
        
        auto fc = diy::inject<FrontController>(ctx_);
        server->bind(port)
        .then( [fc](prio::Request& req, prio::Response& res)
        {
        	fc->request_handler(req,res);
        })
		.otherwise([](const std::exception& ex)
        {

#ifdef MOL_PROMISE_DEBUG
        	std::cout << "ex:!" << ex.what() << std::endl;
#endif
        });
  
        servers_.push_back(std::unique_ptr<prio::http_server>(server));
    }
    catch( repro::Ex& ex)
    {
        std::cout << "ex: " << ex.msg << std::endl;
        r = 1;
    }  
    return r;
}


int WebServer::listen(prio::SslCtx& ssl, int port)
{
    int r = 0;
    try
    {
        prio::http_server* server = new prio::http_server(ssl);
        
        auto fc = diy::inject<FrontController>(ctx_);
        server->bind(port)
        .then( [fc](prio::Request& req, prio::Response& res)
        {
        	fc->request_handler(req,res);
        })
		.otherwise([](const std::exception& ex)
        {

#ifdef MOL_PROMISE_DEBUG
        	std::cout << "ex:!" << ex.what() << std::endl;
#endif
        });
  
        servers_.push_back(std::unique_ptr<prio::http_server>(server));
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
    for( auto& server : servers_)
    {
        server->shutdown();
    }
}




} // end namespace csgi


