
#ifndef _MOL_DEF_GUARD_DEFINE_CHAT_PROMISE_EXAMPLE_WS_DEF_
#define _MOL_DEF_GUARD_DEFINE_CHAT_PROMISE_EXAMPLE_WS_DEF_

#include "model.h"
#include "repo.h"
#include "reproweb/ws/ws.h"
#include "reproweb/json/json.h"

class WebSocketController
{
public:

	WebSocketController(std::shared_ptr<SessionRepository> s, std::shared_ptr<reproweb::EventBus> bus)
		: session_(s), eventBus_(bus)
	{}

    void onConnect(reproweb::WsConnection::Ptr ws)
    {
    	std::string id = eventBus_->subscribe("chat-topic", [ws] (Json::Value value)
    	{
    		ws->send(0x01,reproweb::JSON::stringify(value));
    	});

    	ws->attributes.set("subscription-id",id);
    };

    void onClose(reproweb::WsConnection::Ptr ws)
    {
    	std::string id = ws->attributes.attr<std::string>("subscription-id");
    	eventBus_->unsubscribe("chat-topic", id);
    };

    void onMsg(reproweb::WsConnection::Ptr ws, const std::string& data)
	{
    	std::cout << "ws: " << data << std::endl;

    	Json::Value json = reproweb::JSON::parse(data);
    	std::string sid  = json["sid"].asString();
    	std::string msg  = json["msg"].asString();

    	session_->get_user_session(sid)
    	.then([this,msg,ws](Session session)
    	{
			Json::Value profile = session.profile();
    		Json::Value json(Json::objectValue);
    		json["uid"]   = profile["username"];
    		json["login"] = profile["login"];
    		json["img"]   = profile["avatar_url"];
    		json["msg"]   = msg;

    		eventBus_->notify("chat-topic",json);
    	})
		.otherwise([ws](const std::exception& ex)
		{
    		ws->close();
		});

	};

private:

    std::shared_ptr<SessionRepository> session_;
    std::shared_ptr<reproweb::EventBus> eventBus_;
};

#endif


