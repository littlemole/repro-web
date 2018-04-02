#include "gtest/gtest.h"
#include <memory>
#include <list>
#include <utility>
#include <iostream>
#include <string>
#include <exception>
#include <functional>
#include "test.h"
#include "priocpp/loop.h"
#include "priocpp/api.h"
#include "priocpp/task.h"
#include "priohttp/http_server.h"
#include "priohttp/request.h"
#include "priohttp/response.h"
#include "priohttp/conversation.h"
#include "priohttp/client_conversation.h"
#include "priohttp/client.h"
#include <reproweb/ctrl/controller.h>
#include <reproweb/tools/config.h>
#include <reproweb/json/jwt.h>
#include <reprocurl/asyncCurl.h>
#include <reproweb/ctrl/front_controller.h>
#include <reproweb/web_webserver.h>
#include <cryptoneat/cryptoneat.h>
#include <cryptoneat/base64.h>
#include <diycpp/ctx.h>

#ifdef _WIN32
#include <openssl/applink.c>
#endif

 
using namespace reproweb;
using namespace repro;
using namespace prio;
using namespace cryptoneat;
using namespace diy;

//DIY_DEFINE_CONTEXT()

class BasicTest : public ::testing::Test {
 protected:

  static void SetUpTestCase() {

	  //theLoop().signal(SIGPIPE).then([](int s){});

  }

  virtual void SetUp() {
	 // MOL_TEST_PRINT_CNTS();
  }

  virtual void TearDown() {
	 // MOL_TEST_PRINT_CNTS();
  }
}; // end test setup




class Logger
{
public:

	void log(const std::string& s)
	{
		std::cout << s << std::endl;
	}

#ifdef _RESUMABLE_FUNCTIONS_SUPPORTED

	repro::Future<int> test()
	{
		co_await nextTick();
		co_return 42;
	}
#endif

};

singleton<Logger()> LoggerComponent;


class TestController
{
public:

	TestController() {}

	TestController(std::shared_ptr<Logger> logger)
	: logger_(logger)
	{}

	~TestController()
	{
		std::cout << "~TestController" << std::endl;
	}

	void handlerA( prio::Request& req, prio::Response& res)
	{
		logger_->log(req.path.path());
		res.body( "HELO WORL");
		res.ok().flush();
	}

#ifdef _RESUMABLE_FUNCTIONS_SUPPORTED
	Async handlerB( prio::Request& req, prio::Response& res) 
	{
		int status = 0;
		std::string header;

		std::cout << "controller B" << std::endl;

		try {

			auto req = reprocurl::async_curl()->url("http://localhost:8765/path/a");

			reprocurl::CurlEasy::Ptr curl = co_await req->perform();

			status = curl->status();

			res.ok().body(curl->response_body()).header("server", "molws");

			int i = co_await logger_->test();

			std::cout << i <<  " status: " << status << " server:" << header << std::endl;

			res.flush();

		}
		catch (const std::exception& ex)
		{
			std::cout << ex.what() << std::endl;
			theLoop().exit();
		};
	}

#endif

private:

	std::shared_ptr<Logger> logger_;
};

/*
http_controller routes(

	GET ("/path/a",&TestController::handlerA)

#ifdef _RESUMABLE_FUNCTIONS_SUPPORTED
	,
	POST("/path/b",&TestController::handlerB)
#endif
);
*/
singleton<TestController(Logger)> TestControllerComponent;





TEST_F(BasicTest, SimpleDI) 
{
	std::string result;

	WebApplicationContext ctx {

		LoggerComponent,
		TestControllerComponent,

		GET ("/path/a",&TestController::handlerA)
#ifdef _RESUMABLE_FUNCTIONS_SUPPORTED
		,
		POST("/path/b",&TestController::handlerB)
#endif		
	};

	{
		reproweb::WebServer server(ctx);

		nextTick()
		.then( [&result,&server]()
		{
			HttpClient::url("http://localhost:8765/path/a")
			->fetch()
			.then([&result,&server](prio::Response& res)
			{
				result = res.body();
				server.shutdown();
				theLoop().exit();
			})
			.otherwise([&server](const std::exception& ex)
			{
				std::cout << ex.what() << std::endl;
				server.shutdown();
				theLoop().exit();
			});
		});

		server.listen(8765);
		theLoop().run();
	}
    EXPECT_EQ("HELO WORL",result);
    MOL_TEST_ASSERT_CNTS(0,0);
}


TEST_F(BasicTest, SimpleSSL) {

	std::string result;


	WebApplicationContext ctx {

		LoggerComponent,
		TestControllerComponent,

		GET ("/path/a",&TestController::handlerA)

#ifdef _RESUMABLE_FUNCTIONS_SUPPORTED
		,
		POST("/path/b",&TestController::handlerB)
#endif		
	};

	{
		prio::SslCtx server_ctx;
		server_ctx.load_cert_pem("pem/server.pem");
		prio::SslCtx client_ctx;

		reproweb::WebServer server(ctx);

		nextTick()
		.then( [&result,&client_ctx,&server]()
		{
			HttpClient::url(client_ctx,"https://localhost:8765/path/a")
			->fetch()
			.then([&result,&server](prio::Response& res)
			{
				result = res.body();

				timeout([&server]()
				{
					//theLoop().exit();
					server.shutdown();
					theLoop().exit();
				},1,0);
			});
		});
		
		server.listen(server_ctx,8765);
		theLoop().run();

	}
    EXPECT_EQ("HELO WORL",result);
    MOL_TEST_ASSERT_CNTS(0,0);
}


TEST_F(BasicTest, SimpleConfig)
{
	Config config;
	config.load("./config.json");

	EXPECT_STREQ("/test", config.getString("url").c_str() );
}


TEST_F(BasicTest, EventBus)
{
	std::string result;

	EventBus eventBus;

	eventBus.subscribe( "test", [&result](Json::Value value)
	{
		result = value["event"].asString();
		theLoop().exit();
	});

	timeout( [&eventBus]()
	{
		Json::Value value(Json::objectValue);
		value["event"] = "TEST";
		eventBus.notify("test", value);
	},0,100);

	theLoop().run();

	EXPECT_EQ("TEST",result);
	MOL_TEST_ASSERT_CNTS(0,0);
}


TEST_F(BasicTest, EventBusMore)
{
	int result = 0;

	EventBus eventBus;

	eventBus.subscribe( "test", [&result](Json::Value value)
	{
		result++;
	});

	eventBus.subscribe( "test", [&result](Json::Value value)
	{
		result++;
	});

	eventBus.subscribe( "test2", [&result](Json::Value value)
	{
		result = -1;
	});

	timeout( [&eventBus]()
	{
		Json::Value value(Json::objectValue);
		value["event"] = "TEST";
		eventBus.notify("test", value);
		timeout([]()
		{
			theLoop().exit();
		},0,100);
	},0,100);


	theLoop().run();

	EXPECT_EQ(2,result);
	MOL_TEST_ASSERT_CNTS(0,0);
}


TEST_F(BasicTest, EventBusMoreButLess)
{
	int result = 0;

	EventBus eventBus;

	std::string id = eventBus.subscribe( "test", [&result](Json::Value value)
	{
		result++;
	});

	eventBus.subscribe( "test", [&result](Json::Value value)
	{
		result++;
	});

	eventBus.subscribe( "test2", [&result](Json::Value value)
	{
		result = -1;
	});

	eventBus.unsubscribe("test",id);

	timeout( [&eventBus]()
	{
		Json::Value value(Json::objectValue);
		value["event"] = "TEST";
		eventBus.notify("test", value);
		timeout([]()
		{
			theLoop().exit();
		},0,100);
	},0,100);

	theLoop().run();

	EXPECT_EQ(1,result);
	MOL_TEST_ASSERT_CNTS(0,0);
}


TEST_F(BasicTest, JWT)
{
	std::string secret = "verysecretsecret";

	Json::Value claim(Json::objectValue);
	claim["data"] = "some payload";
	reproweb::JWT jwt(claim);

	std::string token = jwt.sign(secret,60);

	std::cout << "token: " << token << "<-----" << std::endl;

	reproweb::JWT verifier(token);
	bool verified = verifier.verify(secret);

	EXPECT_EQ(true,verified);

	EXPECT_STREQ("some payload", verifier.claim()["data"].asString().c_str());
}


TEST_F(BasicTest, JWTRSASHA256)
{
	cryptoneat::PublicKey pubkey("pem/public.pem");
	cryptoneat::PrivateKey privkey("pem/private.pem");

    std::time_t exp = time(NULL);
    exp += 60;
    Json::Value claim_(Json::objectValue);
    Json::Value header_(Json::objectValue);

    header_["typ"] = "JWT";
    header_["alg"] = "RS256";
    claim_["exp"] = exp;
	claim_["data"] = "some payload";

    std::cout << "exp:" << exp << std::endl;

    std::ostringstream oss;
    oss << Base64Url::encode(JSON::stringify(header_));
    oss << ".";
    oss << Base64Url::encode(JSON::stringify(claim_));
    std::string payload = oss.str();

    cryptoneat::Signature signer(digest("sha256"),privkey);
    std::string signature_ = signer.sign(payload);


    std::string b64 = Base64Url::encode(signature_);
    std::string tmp = Base64Url::decode(b64);

    std::ostringstream token;
    token << payload << "." << Base64Url::encode(signature_);

	std::cout << token.str() << std::endl;

    cryptoneat::Signature verifier(digest("sha256"),pubkey);
    bool b = verifier.verify(payload,signature_);

    EXPECT_EQ(true,b);


    b = verifier.verify(payload,tmp);

    EXPECT_EQ(true,b);
}

TEST_F(BasicTest, JWTRSASHA25__6)
{
	cryptoneat::PublicKey pubkey("pem/public.pem");
	cryptoneat::PrivateKey privkey("pem/private.pem");

	Json::Value claim(Json::objectValue);
	claim["data"] = "some payload";

	reproweb::JWT jwt(claim);
	std::string token = jwt.sign(privkey,60);

	reproweb::JWT verifier(token);
	bool verified = verifier.verify(pubkey);

	EXPECT_EQ(true,verified);

	EXPECT_STREQ("some payload", verifier.claim()["data"].asString().c_str());

}

TEST_F(BasicTest,dummy)
{
	std::string in = "xlPa8hRIGeEhwSJjZ-D0a1U9-f_GvNWm_OiFyKrmzERmYqiuilCtDewMVnQsvC2gUzaJpz6geJzzqJMeRFsuxhkU8YC75Bd_B-JF0FaY5I2S9ogxarBEdnSA17TYeKbao1Kehq47GfPt6xF5I0HcZlsJ7MeuzDJh_NUIRCCEMTPYCRYDNknyUmznwWfgZyD5t6JcpL7WdxDAAVfxXWZXBOEmDQcV0qCh0cnct3xgixa83c-YjjfO6SBUdruzPXefyrkBqG08szx-5VSvIA1QB3NvqodkfL0PIYDYVwwrjRue9E1dbZ57Tt2P6zw8YZ8zA9MWutu8WhB7gfgdtim_";

    std::string plain = Base64Url::decode(in);

    std::string b64 = Base64Url::encode(plain);

    plain = Base64Url::decode(in);

    EXPECT_STREQ(b64.c_str(),in.c_str());
}



#ifdef _RESUMABLE_FUNCTIONS_SUPPORTED

repro::Future<> coroutine_example(reproweb::WebServer& server, std::string& result);

TEST_F(BasicTest, coroutine)
{
	std::string result;

	WebApplicationContext ctx {

		GET ("/path/a",&TestController::handlerA),

#ifdef _RESUMABLE_FUNCTIONS_SUPPORTED
		POST("/path/b",&TestController::handlerB),
#endif		
		LoggerComponent,
		TestControllerComponent
	};

	{
		reproweb::WebServer server(ctx);

		coroutine_example(server,result);

		server.listen(8765);
		theLoop().run();

	}
	EXPECT_EQ("molws", result);
	MOL_TEST_ASSERT_CNTS(0, 0);

}




repro::Future<> coroutine_example(reproweb::WebServer& server, std::string& result)
{
	try
	{
		co_await nextTick();

		auto post = reprocurl::async_curl()->url("http://localhost:8765/path/b")->method("POST");

		reprocurl::CurlEasy::Ptr curl = co_await post->perform();

		result = curl->response_header("server");
		server.shutdown();

		timeout( []() {
			theLoop().exit();
			reprocurl::curl_multi().dispose();
		},1,0);
	}
	catch(const std::exception& ex)
	{
		std::cout << ex.what() << std::endl;
		server.shutdown();
		theLoop().exit();
		reprocurl::curl_multi().dispose();
	}
}

#endif

#include <reproweb/ws/ws.h>
#include <fcntl.h>
#include <signal.h>




class WebSocketController
{
public:

	WebSocketController()
	{}

    void onConnect(WsConnection::Ptr ws)
    {
    	std::cout << "ws on connect" << std::endl;
    };

    void onClose(WsConnection::Ptr ws)
    {
    	std::cout << "ws on close" << std::endl;
    };

    void onMsg(WsConnection::Ptr ws, const std::string& data)
	{
    	std::cout << "ws: " << data << std::endl;
    	ws->send(0x01,data);
	};

};

provider<WebSocketController()> WebSocketControllerComponent;


TEST_F(BasicTest, SimpleHttp) {

	std::string result;


	WebApplicationContext ctx {

		LoggerComponent,
		TestControllerComponent,
		WebSocketControllerComponent,

		ws_controller<WebSocketController>( "/ws"),

		GET ("/path/a",&TestController::handlerA),

#ifdef _RESUMABLE_FUNCTIONS_SUPPORTED
		POST("/path/b",&TestController::handlerB),
#endif		
	};	

	{

		reproweb::WebServer server(ctx);


		//ws_controller<WebSocketController> ws(inject<FrontController>(ctx),"/ws");

		WsConnection::Ptr client= WsConnection::create();

		nextTick([&client,&server]()
		{
			client->connect("ws://localhost:8765/ws")
			.then( [&server](WsConnection::Ptr ws)
			{
				std::cout << "connected" << std::endl;
				ws->onMsg([&server](WsConnection::Ptr ws, const std::string& data)
				{
					std::cout << "ws client: " << data << std::endl;
					//ws->connection()->shutdown();
					ws->close();
					//ws->connection()->close();
					//ws->dispose();
					//loop.exit();
					timeout([&server]()
					{
						std::cout << "TIMEOUT" << std::endl;
						server.shutdown();
						timeout([]()
						{
							theLoop().exit();
						},0,200);
					},0,200);
				});
				ws->onClose( [](WsConnection::Ptr ws) 
				{
					std::cout << "client ws on close" << std::endl;
				});
				ws->send(0x01,"HELO");
			});

		});

		server.listen(8765);
		theLoop().run();
	}

	EXPECT_EQ(1,1);
	MOL_TEST_ASSERT_CNTS(0,0);
}

TEST_F(BasicTest, SimpleHttps) {

	std::string result;


	WebApplicationContext ctx {

		ws_controller<WebSocketController>( "/ws"),

		GET ("/path/a",&TestController::handlerA),

#ifdef _RESUMABLE_FUNCTIONS_SUPPORTED
		POST("/path/b",&TestController::handlerB),
#endif		
		LoggerComponent,
		TestControllerComponent,
		WebSocketControllerComponent
	};

	{
		prio::SslCtx server_ctx;
		server_ctx.load_cert_pem("pem/server.pem");
		prio::SslCtx client_ctx;

		reproweb::WebServer server(ctx);


		//ws_controller<WebSocketController> ws(inject<FrontController>(ctx),"/ws");

		WsConnection::Ptr client;

		timeout([&client_ctx,&client,&server]()
		{
			client= WsConnection::create(client_ctx);
			client->connect("wss://localhost:8766/ws")
			.then( [&server](WsConnection::Ptr ws)
			{
				std::cout << "connected" << std::endl;
				ws->onMsg([&server](WsConnection::Ptr ws, const std::string& data)
				{
					std::cout << "ws client: " << data << std::endl;
					//ws->connection()->shutdown();
					ws->close();
					//ws->connection()->close();
					//ws->dispose();
			    	//loop.exit();
					timeout([&server]()
					{
						std::cout << "TIMEOUT" << std::endl;
						server.shutdown();
						timeout([]()
						{
							theLoop().exit();
						}, 0, 200);
						
					},0,200);
				});

				ws->send(0x01,"HELO");
			});

		},0,500);

		server.listen(server_ctx,8766);
		theLoop().run();

	}

	EXPECT_EQ(1,1);
	MOL_TEST_ASSERT_CNTS(0,0);
}




int main(int argc, char **argv)
{
	prio::init();
	SSLUser useSSL;

    ::testing::InitGoogleTest(&argc, argv);
    int r = RUN_ALL_TESTS();

    return r;
}
