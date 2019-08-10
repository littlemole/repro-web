
#ifndef _MOL_DEF_GUARD_DEFINE_CHAT_PROMISE_EXAMPLE_WS_DEF_
#define _MOL_DEF_GUARD_DEFINE_CHAT_PROMISE_EXAMPLE_WS_DEF_

#include "model.h"
#include "service.h"
#include "reproweb/ws/ws.h"
#include "reproweb/json/json.h"
#include "reproredis/redis.h"

using namespace reproweb;


struct Redis : public reproredis::RedisPool
{
	Redis(std::shared_ptr<AppConfig> config) 
	  : RedisPool(config->getString("redis")) 
	{}
};


class RedisBus
{
public:

	RedisBus(std::shared_ptr<Redis> p)
		: redis_(p)
	{}

	//! subscribe a callback to a topic, return subscrption id
	std::string subscribe( const std::string topic, std::function<void(Json::Value)> observer)
	{
		std::cout << "ws subscribe " << topic << std::endl;
		// if there is no redis subscriber for this topic yet, create one
		if(listeners_.count(topic) == 0)
		{
			redis_subscribe(topic,observer);
		}

		std::string id = cryptoneat::nonce(32);
		prio::nextTick( [this,id,topic,observer] ()
		{
			subscriptions_[topic].insert(std::make_pair(id, subscription(observer)));
		});
		return id;		
	}

	//! unsubscribe with subscription id
	void unsubscribe( const std::string& topic,  const std::string& id)
	{
		prio::nextTick( [this,topic,id] ()
		{
			subscriptions_[topic].erase(id);
		});
	}

	//! raise an Event to a topic using Redis passing JSON as payload
	void notify(const std::string topic, Json::Value value)
	{
		std::string msg = JSON::flatten(value);

		std::cout << "send: " << topic << ": " << msg << std::endl;

		redis_->cmd("publish", topic, msg)
		.then([](reproredis::RedisResult::Ptr r)
		{
			std::cout << "MSG SEND! " << r->str() << std::endl;
		})			
		.otherwise([](const std::exception& ex)
		{
			std::cout << "redis send failed: " <<  ex.what() << std::endl;
		});		
	}

	//! clear all subscriptions
	void clear()
	{
		subscriptions_.clear();
		listeners_.clear();
	}

private:

	//! subscribe a callback to a topic, return subscrption id
	void redis_subscribe( const std::string topic, std::function<void(Json::Value)> observer)
	{
		std::cout << "redis resubscribe " << topic << std::endl;

		reproredis::RedisSubscriber* sub = new reproredis::RedisSubscriber(*(redis_.get()));
		sub->subscribe(topic)
		.then( [this] (std::pair<std::string,std::string> msg)
		{
			std::cout  << "msg: " << msg.first << ": " << msg.second << std::endl;
			this->notified(msg.first,msg.second);
		})			
		.otherwise([this,topic,observer](const std::exception& ex)
		{
			std::cout << "!" << ex.what() << std::endl;
			redis_subscribe(topic,observer);
		});				

		std::cout << "redis subscribed " << topic << std::endl;

		listeners_[topic] = std::unique_ptr<reproredis::RedisSubscriber>(sub);
	}

	void notified(std::string topic, std::string msg)
	{
		for( auto fun : subscriptions_[topic])
		{
			prio::nextTick( [fun,msg]()
			{
				fun.second.fun(JSON::parse(msg));
			});
		}
	}

	std::shared_ptr<Redis> redis_;

	std::map<std::string,std::map<std::string,subscription>> subscriptions_;
	std::map<std::string,std::unique_ptr<reproredis::RedisSubscriber>> listeners_;
};


class WebSocketController
{
public:

	// c'tor
	WebSocketController(
		std::shared_ptr<SessionService> s, 
		std::shared_ptr<RedisBus> bus
	)
	  : session_(s), 
	  	eventBus_(bus)
	{}

	// websocket connects
    void onConnect(WsConnection::Ptr ws)
    {
		std::cout << "on connect" << std::endl;
		// subscribe to topic and pass any Json downwards on invocation
    	std::string sid = eventBus_->subscribe("chat-topic", [ws] (Json::Value value)
    	{
    		ws->send(0x01,JSON::stringify(value));
    	});

		std::cout << "on connected" << std::endl;

		// make this ws remember the subscriber id
		set_subscriber_id(ws,sid);
    };

	// websocket disconnects
    void onClose(WsConnection::Ptr ws)
    {
		// unsubscribe from topic using supscription id
    	eventBus_->unsubscribe("chat-topic", subscriber_id(ws));
    };

	// receive a websocket msg
    void onMsg(reproweb::WsConnection::Ptr ws, const std::string& data)
	{
		try 
		{
			std::cout << "ws: " << data << std::endl;

			// parse msg
			Json::Value json = JSON::parse(data);
			std::string sid  = json["sid"].asString();
			std::string msg  = json["msg"].asString();

			// validate session			
			session_->get_user_session(sid)
			.then([this,msg](reproweb::Session session)
			{
				if(!session.authenticated)
				{
					throw AuthEx();
				}
				// populate result
				Json::Value profile = session.data;

				Json::Value result(Json::objectValue);
				result["uid"]   = profile["username"];
				result["login"] = profile["login"];
				result["img"]   = profile["avatar_url"];
				result["msg"]   = msg;

				// publish msg to all subscribers
				eventBus_->notify("chat-topic",result);

			})
			.otherwise([ws](const std::exception& ex)
			{
				ws->close();
			});

		}
		catch(const std::exception& ex)
		{
    		ws->close();
		}
	};

private:

	void set_subscriber_id(WsConnection::Ptr ws, const std::string& id)
	{
		ws->attributes.set("subscription-id",id);
	}

	std::string subscriber_id(WsConnection::Ptr ws)
	{
		return ws->attributes.attr<std::string>("subscription-id");
	}

    std::shared_ptr<SessionService> session_;
    std::shared_ptr<RedisBus> eventBus_;
};

#endif


