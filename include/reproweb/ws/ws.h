#ifndef INCLUDE_PROMISE_WS_WS_H_
#define INCLUDE_PROMISE_WS_WS_H_

#include "reproweb/ws/websocket.h"
#include <diycpp/ctx.h>

//////////////////////////////////////////////////////////////

namespace reproweb   {


template<class T>
class ws_controller
{
public:

	ws_controller( const std::string& path)
	  : path_(path)
	{
		std::cout << "declare WS " << path_ << std::endl;		
	}

	void ctx_register(diy::Context* ctx)
	{
		std::cout << "register WS " << path_ << std::endl;
		auto fc = ctx->resolve<FrontController>();

		fc->registerHandler(
			"GET",
			path_,
			[](prio::Request& req, prio::Response& res)
			{
				handshake(req, res);
			}
		);
	}

private:

	static void handshake(prio::Request& req, prio::Response& res)
	{
		reproweb::WsConnection::Ptr ws = reproweb::WsConnection::create();

	    std::string hash = ws->handshake(req);

	    if(hash.empty())
	    {
			res.error().flush();
			return;
	    }

	    ws->onConnect([](WsConnection::Ptr ws)
	    {
	    	auto ptr = ws->attributes.attr<std::shared_ptr<diy::Context>>("ctx")->resolve<T>();
	    	ptr->onConnect(ws);
	    });

	    ws->onClose([](WsConnection::Ptr ws)
	    {
	    	auto ptr = ws->attributes.attr<std::shared_ptr<diy::Context>>("ctx")->resolve<T>();
	    	ptr->onClose(ws);
	    });

	    ws->onMsg([](WsConnection::Ptr ws, const std::string& data)
		{
	    	auto ptr = ws->attributes.attr<std::shared_ptr<diy::Context>>("ctx")->resolve<T>();
	    	ptr->onMsg(ws,data);
		});

	    res
		.header("Upgrade","websocket")
	    .header("Connection","Upgrade")
	    .header("Sec-WebSocket-Accept",hash)
	    .status("HTTP/1.1 101 Switching Protocols")
	    .flush();

	    ws->run();
	};

	std::string path_;
};



} // close namespaces

#endif

