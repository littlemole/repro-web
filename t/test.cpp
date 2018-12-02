#include "gtest/gtest.h"

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
#include <reproweb/ctrl/ssi.h>
#include <reprocurl/asyncCurl.h>
#include <reproweb/ctrl/front_controller.h>

#include <reproweb/json/json.h>
#include <reproweb/serialization/json.h>
#include <reproweb/serialization/xml.h>
#include <reproweb/serialization/web.h>

#include <reproweb/web_webserver.h>
#include <reproweb/view/i18n.h>
#include <reproweb/view/tpl.h>
#include <reproweb/tools/validation.h>
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

typedef const char* entity_name_t;

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

 
struct Input 
{
	std::string id;
	std::string filter;
	std::string sid;
	prio::HeaderValues header;
	prio::Cookie cookie;
	std::string lang;


};
 
auto meta(const Input&)
{
	return metadata (
		// note some mappings are duplicated
		// for serialization, last one will overider earlier ones
		// for deserialization however, all will be mapped
		"id", &Input::id,
		"filter", &Input::filter,
		"sid", &Input::sid,
		"Accept-Language", &Input::header,
		"Accept-Language", &Input::lang,
		"sid", &Input::cookie
	)["Input"];
}


struct User
{
public:

	std::string username;
	std::string login;
	std::string pwd;
	std::vector<std::string> tags;
};

auto meta(const User&)
{
	return metadata (
		"username", &User::username,
		"login", &User::login,
		"pwd", &User::pwd,
		"tags", &User::tags
	)["user"];
}
 

void validate(User& user)
{
	reproweb::valid( user.username, std::regex("[^<>&]+"), "invalid username");
	reproweb::valid( user.login, std::regex("[^<>&]+"), "invalid login");
	reproweb::valid( user.pwd, std::regex(".+"), "invalid pwd");
	reproweb::valid( user.tags, std::regex("[0-9a-zA-Z]+"), "invalid tags");
}
 

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

	void postMultipart( prio::MultiParts mp,  prio::Request& req,  prio::Response& res)
	{
		for( auto& p : mp.parts)
		{
			if ( p.headers.content_type() == "text/html")
			{
				res.body( p.body );
				res.ok().contentType("text/html").flush();
				return;
			}
		}
		res.error().body("error").flush();
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

			auto req = reprocurl::async_curl()->url("http://127.0.0.1:8765/path/a");

			reprocurl::CurlEasy::Ptr curl = co_await req->perform();

			status = curl->status();

			res.ok().body(curl->response_body()).header("server", "molws");

	//		res.ok().body("hello").header("server", "molws");

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

	void handlerSSI( prio::Request& req, prio::Response& res)
	{
		res.contentType("text/html");

		logger_->log(req.path.path());

		char* cwd = getcwd(0,0);
    	std::string path_ = cwd;
    	path_ += "/htdocs";
    	free(cwd); 

        std::regex e ("\\.\\.");
        std::string path = std::regex_replace(req.path.path(),e,"");
        std::string fp = path_ +  path;		

		std::string tmpl = prio::slurp(fp);

		reproweb::SSIResolver::resolve(req,tmpl)
		.then( [&res](std::string s)
		{
			res.body(s);
			res.ok().flush();
		})
		.otherwise([&res](const std::exception& ex)
		{
			res.error().flush();
		});
	}	

	void queryParams( QueryParams qp, /* prio::Request& req,  */ prio::Response& res)
	{
		res.body( qp.get("param") );
		res.ok().flush();
	}
 
	async_json_t<User> getUser()//prio::Request& req, prio::Response& res)
	{
		auto p = json_promise<User>();//promise<json_t<User>>();

		nextTick( [p]()
		{
			User user{ "mike", "littlemole", "secret", { "one", "two", "three"} };
			p.resolve( json_t<User>{user} );
		});

		return p.future();
	}
   
 
	repro::Future<std::string> getParams( Parameter<Input> params)//, prio::Request& req, prio::Response& res)
	{
		auto p = promise<std::string>();

		std::ostringstream oss;
		oss << "======================================" << std::endl;
		oss << params->cookie.str() << std::endl;
		oss << params->sid << std::endl;
		oss << params->filter << std::endl;
		oss << params->id << std::endl;
		oss << "======================================" << std::endl;

		std::string out = oss.str();

		nextTick( [p,params,out]()
		{
			p.resolve( out );
		});

		return p.future();
	}

	repro::Future<json_t<User>> postUser(json_t<User> user)//, prio::Request& req, prio::Response& res)
	{
		auto p = promise<json_t<User>>();

		nextTick( [p,user]()
		{
			p.resolve(user);
		});

		return p.future();
	}


	repro::Future<Json::Value> postUserJson(Json::Value user)//, prio::Request& req, prio::Response& res)
	{
		auto p = promise<Json::Value>();

		nextTick( [p,user]()
		{
			p.resolve(user);
		});

		return p.future();
	}	

#ifdef _RESUMABLE_FUNCTIONS_SUPPORTED

	repro::Future<json_t<User>> getUserCoro() //prio::Request& req, prio::Response& res)
	{
		//co_await nextTick();

		User user{ "mike", "littlemole", "secret", { "one", "two", "three"} };
		co_return json_t<User> {user};
	}


	repro::Future<json_t<User>> postUserCoro(json_t<User> user)//, prio::Request& req, prio::Response& res)
	{
		//co_await nextTick();
		co_return user;
	}


	repro::Future<Json::Value> postUserJsonCoro(Json::Value user)//, prio::Request& req, prio::Response& res)
	{
		//co_await nextTick();

		co_return user;
	}

#endif

private:

	std::shared_ptr<Logger> logger_;
};

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

		coroutine_example(server,result)
		.then([](){})
		.otherwise([](const std::exception& ex){});

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

		auto post = reprocurl::async_curl()->url("http://127.0.0.1:8765/path/b")->method("POST")->data("dummy");

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


TEST_F(BasicTest, SplitSSI) 
{
	std::string ssisrc = 
	"<html><head></head><body>"
	"<table><tr><td>"
	"<!--#include virtual='/inc/header.html' -->"
	"</td></tr><tr><td>"
	"<!--#include virtual='/inc/main.html' -->"
	"</td></tr><tr><td>"
	"<!--#include virtual='/inc/footer.html' -->"
	"</td></tr><tr><td>"
	"</td></tr></table>"
	"</body></html>";

	std::regex r("<!--#include +virtual=(?:[\"'])([^\"']+)(?:[\"']) *-->");
	std::smatch match;

	std::string::const_iterator start = ssisrc.begin();
    std::string::const_iterator end   = ssisrc.end();    

	std::vector<std::string> result;
    
    while (std::regex_search (start,end,match,r)) 
    {
        if ( match.size() > 1 )
        {
			std::cout << std::string(start,match[0].first) << std::endl;
			std::cout << "inc: " << match[1] << std::endl;
            result.push_back(match[1]);
        }
        start = match[0].second;
    }
	std::cout << std::string(start,end) << std::endl;
	EXPECT_EQ(3,result.size());
}

TEST_F(BasicTest, ResolveSSI) 
{
	std::string ssisrc = 
	"<html><head></head><body>"
	"<table><tr><td>"
	"<!--#include virtual='/path/a' -->"
	"</td></tr><tr><td>"
	"<!--#include virtual='/path/a' -->"
	"</td></tr><tr><td>"
	"<!--#include virtual='/path/a' -->"
	"</td></tr><tr><td>"
	"</td></tr></table>"
	"</body></html>";

	std::string result;

	WebApplicationContext ctx {

		LoggerComponent,
		TestControllerComponent,

		GET ("/path/a",&TestController::handlerA)
	};

	{
		reproweb::WebServer server(ctx);

		nextTick()
		.then( [&ctx,ssisrc]()
		{

			auto fc = inject<FrontController>(ctx);

			Request req;
			req.attributes.set<std::shared_ptr<diy::Context>>("ctx",std::make_shared<Context>(&ctx));
			req.path.method("GET");

			return reproweb::SSIResolver::resolve(req,ssisrc);
		})
		.then( [&result,&server](std::string s)
		{
				result = s;
				server.shutdown();
				theLoop().exit();
		})
		.otherwise([&server](const std::exception& ex)
		{
			std::cout << ex.what() << std::endl;
			server.shutdown();
			theLoop().exit();
		});

		server.listen(8765);
		theLoop().run();
	}
    EXPECT_EQ("<html><head></head><body><table><tr><td>HELO WORL</td></tr><tr><td>HELO WORL</td></tr><tr><td>HELO WORL</td></tr><tr><td></td></tr></table></body></html>",result);
    MOL_TEST_ASSERT_CNTS(0,0);
}

TEST_F(BasicTest, handleSSI) 
{
	std::string result;

	WebApplicationContext ctx {

		LoggerComponent,
		TestControllerComponent,

		GET ("/path/a",&TestController::handlerA),
		GET ("/index.shtml",&TestController::handlerSSI)
	};

	{
		reproweb::WebServer server(ctx);

		nextTick()
		.then( [&result,&server]()
		{
			HttpClient::url("http://localhost:8765/index.shtml")
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
    EXPECT_EQ("<html><head></head><body>\n<table><tr><td>\nHELO WORL\n</td></tr><tr><td>\nHELO WORL\n</td></tr><tr><td>\nHELO WORL\n</td></tr><tr><td>\n</td></tr></table>\n</body></html>\n",result);
    MOL_TEST_ASSERT_CNTS(0,0);
}

TEST_F(BasicTest, autoHandleSSI) 
{
	std::string result;

	WebApplicationContext ctx {

		LoggerComponent,
		TestControllerComponent,

		GET ("/path/a",&TestController::handlerA),
		ssi_content("/htdocs", "/.*\\.shtml")
	};

	{
		reproweb::WebServer server(ctx);

		nextTick()
		.then( [&result,&server]()
		{
			HttpClient::url("http://localhost:8765/index.shtml")
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
    EXPECT_EQ("<html><head></head><body>\n<table><tr><td>\nHELO WORL\n</td></tr><tr><td>\nHELO WORL\n</td></tr><tr><td>\nHELO WORL\n</td></tr><tr><td>\n</td></tr></table>\n</body></html>\n",result);
    MOL_TEST_ASSERT_CNTS(0,0);
}


  

TEST_F(BasicTest, SimpleInclude) 
{
	std::string result;

	WebApplicationContext ctx {

		LoggerComponent,
		TestControllerComponent,

		GET ("/path/a",&TestController::handlerA)
	};

	{
		reproweb::WebServer server(ctx);

		nextTick()
		.then( [&ctx]()
		{
			auto fc = inject<FrontController>(ctx);

			Request req;
			req.path.method("GET");

			return fc->include(req,"/path/a");
		})
		.then( [&result,&server](std::string s)
		{
				result = s;
				server.shutdown();
				theLoop().exit();
		})
		.otherwise([&server](const std::exception& ex)
		{
			std::cout << ex.what() << std::endl;
			server.shutdown();
			theLoop().exit();
		});

		server.listen(8765);
		theLoop().run();
	}
    EXPECT_EQ("HELO WORL",result);
    MOL_TEST_ASSERT_CNTS(0,0);
}


TEST_F(BasicTest, SimpleI18N) 
{
	I18N i18n("/locale/properties", {"de", "de_DE", "en"});

	std::string val = i18n.key("de","login.page.title");
	EXPECT_EQ("Hallo Coro Websockets",val);

	val = i18n.key("de_DE","login.page.title");
	EXPECT_EQ("Guten Tag Coro Websockets",val);

	val = i18n.key("en_US","login.page.title");
	EXPECT_EQ("Heya Coro Websockets",val);	

	val = i18n.key("en","login.page.title");
	EXPECT_EQ("Heya Coro Websockets",val);	

	val = i18n.key("","login.page.title");
	EXPECT_EQ("Hello Coro Websockets",val);	

	val = i18n.key("xxxunknownlocalexxx","login.page.title");
	EXPECT_EQ("Hello Coro Websockets",val);	

}


 
TEST_F(BasicTest, SimpleI18Ndefaults) 
{
	I18N i18n("/locale/properties", {"de", "de_DE", "en"});

	std::string val = i18n.key("de","login.main.copyright");
	EXPECT_EQ("&copy; littlemole. All rights reserved.",val);

	val = i18n.key("de_DE","login.main.copyright");
	EXPECT_EQ("&copy; littlemole. All rights reserved.",val);

	val = i18n.key("de_DE","login.main.copyright");
	EXPECT_EQ("&copy; littlemole. All rights reserved.",val);

	val = i18n.key("de_DE","login.main.copyright");
	EXPECT_EQ("&copy; littlemole. All rights reserved.",val);
}


TEST_F(BasicTest, SimpleI18Ntpl) 
{
	I18N i18n("/locale/properties", {"de", "de_DE", "en"});

	std::string tpl = 
	"<html>\n"
	"<body>\n"
	"<h1><!--#i18n key='login.page.title' --></h1>\n"
	"<p><!--#i18n key = 'login.header.headline.desc' --></p>\n"
	"</body>\n"
	"</html>\n";

	std::string content = i18n.render("de_DE",tpl);
	EXPECT_EQ("<html>\n<body>\n<h1>Guten Tag Coro Websockets</h1>\n<p>modernes reactives c++ f\xC3\xBCr das web.</p>\n</body>\n</html>\n",content);

	content = i18n.render("de",tpl);
	EXPECT_EQ("<html>\n<body>\n<h1>Hallo Coro Websockets</h1>\n<p>modernes reactives c++ f\xC3\xBCr das web.</p>\n</body>\n</html>\n",content);

	content = i18n.render("en",tpl);
	EXPECT_EQ("<html>\n<body>\n<h1>Heya Coro Websockets</h1>\n<p>modern reactive c++ for the web.</p>\n</body>\n</html>\n",content);

	content = i18n.render("en_US",tpl);
	EXPECT_EQ("<html>\n<body>\n<h1>Heya Coro Websockets</h1>\n<p>modern reactive c++ for the web.</p>\n</body>\n</html>\n",content);

	content = i18n.render("en_UK",tpl);
	EXPECT_EQ("<html>\n<body>\n<h1>Heya Coro Websockets</h1>\n<p>modern reactive c++ for the web.</p>\n</body>\n</html>\n",content);

	content = i18n.render("xyz",tpl);
	EXPECT_EQ("<html>\n<body>\n<h1>Hello Coro Websockets</h1>\n<p>modern reactive c++ for the web.</p>\n</body>\n</html>\n",content);

}

TEST_F(BasicTest, I18NtplWithMarkup) 
{
	I18N i18n("/locale/properties", {"de", "de_DE", "en"});

	std::string tpl = 
	"<html>\n"
	"<body>\n"
	"<h1><!--#i18n key='login.page.title' --></h1>\n"
	"email: {{ login }}\n"
	"<p><!--#i18n key = 'login.main.greeting' --></p>\n"
	"</body>\n"
	"</html>\n";     
 
	Json::Value json(Json::objectValue);
	json["username"] = "Michael";
	json["login"]    = "littlemole@oha7.org";

	std::string markup = i18n.render("de_DE",tpl);
	std::string content = mustache::render(markup,json);

	EXPECT_EQ("<html>\n<body>\n<h1>Guten Tag Coro Websockets</h1>\nemail: littlemole@oha7.org\n<p>Willkomen zur\xC3\xBC" "ck, Michael!</p>\n</body>\n</html>\n",content);

	content = mustache::render(i18n.render("de",tpl),json);
	EXPECT_EQ("<html>\n<body>\n<h1>Hallo Coro Websockets</h1>\nemail: littlemole@oha7.org\n<p>Willkomen zur\xC3\xBC" "ck, Michael!</p>\n</body>\n</html>\n",content);

	content = mustache::render(i18n.render("en",tpl),json);
	EXPECT_EQ("<html>\n<body>\n<h1>Heya Coro Websockets</h1>\nemail: littlemole@oha7.org\n<p>Hello dear Michael</p>\n</body>\n</html>\n",content);

	content = mustache::render(i18n.render("en_US",tpl),json);
	EXPECT_EQ("<html>\n<body>\n<h1>Heya Coro Websockets</h1>\nemail: littlemole@oha7.org\n<p>Hello dear Michael</p>\n</body>\n</html>\n",content);

	content = mustache::render(i18n.render("en_UK",tpl),json);
	EXPECT_EQ("<html>\n<body>\n<h1>Heya Coro Websockets</h1>\nemail: littlemole@oha7.org\n<p>Hello dear Michael</p>\n</body>\n</html>\n",content);

	content = mustache::render(i18n.render("xyz",tpl),json);
	EXPECT_EQ("<html>\n<body>\n<h1>Hello Coro Websockets</h1>\nemail: littlemole@oha7.org\n<p>Hello dear Michael</p>\n</body>\n</html>\n",content);

}
 
 
 

TEST_F(BasicTest, fromParams) 
{
	QueryParams qp("username=mike,thumes&login=littlemole&pwd=secret&tags=one,two,three");

	User user;
	fromParams(qp,user);

	EXPECT_EQ("mike,thumes",user.username);
	EXPECT_EQ("littlemole",user.login);
	EXPECT_EQ("secret",user.pwd);

	EXPECT_EQ("one",user.tags[0]);
	EXPECT_EQ("two",user.tags[1]);
	EXPECT_EQ("three",user.tags[2]);
}
  


struct XmlTest
{
	XmlTest()
	{
		id = "42";
	}

	std::string id;

	struct XmlTest1 {

	  int index = 1;

		struct XmlTest2 {

			XmlTest2()
			{
				v.push_back("one");
				v.push_back("two");
				v.push_back("three");
			}
			std::vector<std::string> v;  


		}	level2;

	} level1;

};  


auto meta(const XmlTest::XmlTest1::XmlTest2& t)
{
	return metadata(
		"v", &XmlTest::XmlTest1::XmlTest2::v
	);
}

    

auto meta(const XmlTest::XmlTest1& t)
{
	return metadata (
		"@index", &XmlTest::XmlTest1::index,
		"level2", &XmlTest::XmlTest1::level2
	);
}
 

auto meta(const XmlTest& t)
{
	return metadata (
			"@id", &XmlTest::id,
			"level1", &XmlTest::level1
	)["XmlTest"];
}
 

TEST_F(BasicTest, toXml) 
{
	User user{ "mike", "littlemole", "secret", { "one", "two", "three"} };
	std::shared_ptr<xml::Document> doc = toXml(user);

	std::string s = doc->toString();
 
	std::cout << s << std::endl;

	EXPECT_EQ("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\r\n<user><username>mike</username><login>littlemole</login><pwd>secret</pwd><tags>one</tags><tags>two</tags><tags>three</tags></user>",s);

	User other;
	fromXml(doc,other);

	EXPECT_EQ("mike",other.username);
	EXPECT_EQ("littlemole",other.login);
	EXPECT_EQ("secret",other.pwd);

	EXPECT_EQ("one",other.tags[0]);
	EXPECT_EQ("two",other.tags[1]);
	EXPECT_EQ("three",other.tags[2]);
 
	XmlTest xt;
	doc = toXml(xt);
   
	s = doc->toString();

	EXPECT_EQ("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\r\n<XmlTest id=\"42\"><level1 index=\"1\"><level2><v>one</v><v>two</v><v>three</v></level2></level1></XmlTest>",s);

	std::cout << s << std::endl;

	xml::DocumentPtr d2 = xml::Document::create();
	xml::Element* p = d2->parse(s);

	std::cout << "--- " << (int*)p << " ---" << std::endl;

	std::cout << "--- " << d2->toString() << " ---" << std::endl;

	XmlTest rxto;
	rxto.id = "";
	rxto.level1.index = 0;

	fromXml(d2,rxto);

	doc = toXml(rxto);
	s = doc->toString();
std::cout << s << std::endl;

	EXPECT_EQ("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\" ?>\r\n<XmlTest id=\"42\"><level1 index=\"1\"><level2><v>one</v><v>two</v><v>three</v></level2></level1></XmlTest>", s);
}


TEST_F(BasicTest, toJson) 
{
	User user{ "mike", "littlemole", "secret", { "one", "two", "three"} };
	Json::Value json = toJson(user);

	std::string s = JSON::flatten(json); 

	EXPECT_EQ("{\"user\":{\"login\":\"littlemole\",\"pwd\":\"secret\",\"tags\":[\"one\",\"two\",\"three\"],\"username\":\"mike\"}}",s);

	User other;
	fromJson(json,other);

	EXPECT_EQ("mike",other.username);
	EXPECT_EQ("littlemole",other.login);
	EXPECT_EQ("secret",other.pwd);

	EXPECT_EQ("one",other.tags[0]);
	EXPECT_EQ("two",other.tags[1]);
	EXPECT_EQ("three",other.tags[2]);
}
 
 
TEST_F(BasicTest, toJsonArray) 
{
	User user{ "mike", "littlemole", "secret", { "one", "two", "three"} };
	std::vector<User> root;
	root.push_back(user);
	root.push_back(user);
	root.push_back(user);

	Json::Value json = toJson(root);

	std::string s = JSON::flatten(json); 

	EXPECT_EQ("[{\"login\":\"littlemole\",\"pwd\":\"secret\",\"tags\":[\"one\",\"two\",\"three\"],\"username\":\"mike\"},{\"login\":\"littlemole\",\"pwd\":\"secret\",\"tags\":[\"one\",\"two\",\"three\"],\"username\":\"mike\"},{\"login\":\"littlemole\",\"pwd\":\"secret\",\"tags\":[\"one\",\"two\",\"three\"],\"username\":\"mike\"}]",s);

	std::vector<User> other;
	fromJson(json,other);

	for(int i = 0; i < 3; i++)
	{
	 	EXPECT_EQ("mike",other[i].username);
		EXPECT_EQ("littlemole",other[i].login);
		EXPECT_EQ("secret",other[i].pwd);

		EXPECT_EQ("one",other[i].tags[0]);
		EXPECT_EQ("two",other[i].tags[1]);
		EXPECT_EQ("three",other[i].tags[2]);
	}
}

class Users 
{
public:
	std::string something;
	std::vector<User> users;

	auto meta() const
	{
		return metadata(
			"something", &Users::something,
			"users", &Users::users
		);
	}
};
   
TEST_F(BasicTest, toJsonArray2) 
{
	User user{ "mike", "littlemole", "secret", { "one", "two", "three"} };
	Users users { "the thing", { user } };

	Json::Value json = toJson(users);

	std::string s = JSON::flatten(json); 

	EXPECT_EQ("{\"something\":\"the thing\",\"users\":[{\"login\":\"littlemole\",\"pwd\":\"secret\",\"tags\":[\"one\",\"two\",\"three\"],\"username\":\"mike\"}]}",s);

	Users other;
	fromJson(json,other);

	EXPECT_EQ("the thing",other.something);

	EXPECT_EQ("mike",other.users[0].username);
	EXPECT_EQ("littlemole",other.users[0].login);
	EXPECT_EQ("secret",other.users[0].pwd);

	EXPECT_EQ("one",other.users[0].tags[0]);
	EXPECT_EQ("two",other.users[0].tags[1]);
	EXPECT_EQ("three",other.users[0].tags[2]);
}

class ArrayTest 
{
public:

	std::vector<std::vector<int>> array;

	auto meta() const 
	{
		return metadata(
			"array", &ArrayTest::array
		);
	}
};
 
TEST_F(BasicTest, toJsonArray3) 
{
	ArrayTest at { { {1,2,3}, {4,5,6} } };

	Json::Value json = toJson(at);

	std::string s = JSON::flatten(json); 

	EXPECT_EQ("{\"array\":[[1,2,3],[4,5,6]]}",s);

	ArrayTest other;
	fromJson(json,other);

	EXPECT_EQ(1,other.array[0][0]);
	EXPECT_EQ(2,other.array[0][1]);
	EXPECT_EQ(3,other.array[0][2]);
	EXPECT_EQ(4,other.array[1][0]);
	EXPECT_EQ(5,other.array[1][1]);
	EXPECT_EQ(6,other.array[1][2]);

}

TEST_F(BasicTest, toJsonArray4) 
{
	std::vector<std::vector<std::string>> v { {"a","b"}, {"c","d"} };

	Json::Value json = toJson(v);

	std::string s = JSON::flatten(json); 

	EXPECT_EQ("[[\"a\",\"b\"],[\"c\",\"d\"]]",s);

	std::vector<std::vector<std::string>> other;
	fromJson(json,other);

	EXPECT_EQ("a",other[0][0]);
	EXPECT_EQ("b",other[0][1]);
	EXPECT_EQ("c",other[1][0]);
	EXPECT_EQ("d",other[1][1]);
}

TEST_F(BasicTest, SimpleRest) 
{
	std::string result;

	WebApplicationContext ctx {

		LoggerComponent,
		TestControllerComponent,

		GET ("/path/a",&TestController::getUser)
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
    EXPECT_EQ("{\"user\":{\"login\":\"littlemole\",\"pwd\":\"secret\",\"tags\":[\"one\",\"two\",\"three\"],\"username\":\"mike\"}}",result);
    MOL_TEST_ASSERT_CNTS(0,0);
}

 
const char* multipart = "-----------------------------9051914041544843365972754266\r\n"
"Content-Disposition: form-data; name=\"text\"\r\n"
"\r\n"
"text default\r\n"
"-----------------------------9051914041544843365972754266\r\n"
"Content-Disposition: form-data; name=\"file1\"; filename=\"a.txt\"\r\n"
"Content-Type: text/plain\r\n"
"\r\n"
"Content of a.txt.\r\n"
"\r\n"
"-----------------------------9051914041544843365972754266\r\n"
"Content-Disposition: form-data; name=\"file2\"; filename=\"a.html\"\r\n"
"Content-Type: text/html\r\n"
"\r\n"
"<!DOCTYPE html><title>Content of a.html.</title>\r\n"
"\r\n"
"-----------------------------9051914041544843365972754266--\r\n\r\n";


TEST_F(BasicTest, SimpleMultipart) 
{
	std::string result;

	WebApplicationContext ctx {

		LoggerComponent,
		TestControllerComponent,

		POST ("/path/a",&TestController::postMultipart)
	};

	{
		reproweb::WebServer server(ctx);

		nextTick()
		.then( [&result,&server]()
		{
			HttpClient::url("http://localhost:8765/path/a")
			->content_type("multipart/form-data;boundary=\"---------------------------9051914041544843365972754266\"")
			->POST(multipart)
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
    EXPECT_EQ("<!DOCTYPE html><title>Content of a.html.</title>\r\n",result);
    MOL_TEST_ASSERT_CNTS(0,0);
}


TEST_F(BasicTest, SimpleRestParams) 
{
	std::string result;

	WebApplicationContext ctx {

		LoggerComponent,
		TestControllerComponent,

		GET ("/path/{id}",&TestController::getParams)
	};

	{
		reproweb::WebServer server(ctx);

		nextTick()
		.then( [&result,&server]()
		{
			HttpClient::url("http://localhost:8765/path/a?filter=123456789")
			->header("Cookie", prio::Cookie("sid","987654321").path("/").domain("localhost").secure().maxAge(100).str())
			->header("Accept-Language", "de-DE")
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
    EXPECT_EQ("======================================\nsid=987654321;max-Age=100;domain=localhost;path=/;secure\n987654321\n123456789\na\n======================================\n",result);
    MOL_TEST_ASSERT_CNTS(0,0);
}


TEST_F(BasicTest, SimpleRestQueryParams) 
{
	std::string result;

	WebApplicationContext ctx {

		LoggerComponent,
		TestControllerComponent,

		GET ("/path/a",&TestController::queryParams)
	};

	{
		reproweb::WebServer server(ctx);

		nextTick()
		.then( [&result,&server]()
		{
			HttpClient::url("http://localhost:8765/path/a?param=test")
			->GET()
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
    EXPECT_EQ("test",result);
    MOL_TEST_ASSERT_CNTS(0,0);
}



TEST_F(BasicTest, SimpleRestPost) 
{
	std::string result;

	WebApplicationContext ctx {

		LoggerComponent,
		TestControllerComponent,

		POST ("/path/a",&TestController::postUser)
	};

	{
		reproweb::WebServer server(ctx);

		nextTick()
		.then( [&result,&server]()
		{
			HttpClient::url("http://localhost:8765/path/a")
			->POST("{\"user\":{\"login\" : \"littlemole\",\"pwd\" : \"secret\",\"tags\" : [\"one\",\"two\",\"three\"],\"username\" : \"mike\"\n}}")
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
    EXPECT_EQ("{\"user\":{\"login\":\"littlemole\",\"pwd\":\"secret\",\"tags\":[\"one\",\"two\",\"three\"],\"username\":\"mike\"}}",result);
    MOL_TEST_ASSERT_CNTS(0,0);
}

TEST_F(BasicTest, SimpleRestPost_invalid) 
{
	std::string result;

	WebApplicationContext ctx {

		LoggerComponent,
		TestControllerComponent,

		POST ("/path/a",&TestController::postUser)
	};

	{
		reproweb::WebServer server(ctx);

		nextTick()
		.then( [&result,&server]()
		{
			HttpClient::url("http://localhost:8765/path/a")
			->POST("{\"user\":{\"login\" : \"<littlemole>\",\"pwd\" : \"secret\",\"tags\" : [\"one\",\"two\",\"three\"],\"username\" : \"mike\"\n}}")
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
    EXPECT_EQ("invalid login",result);
    MOL_TEST_ASSERT_CNTS(0,0);
}

TEST_F(BasicTest, SimpleRestPostJson) 
{
	std::string result;

	WebApplicationContext ctx {

		LoggerComponent,
		TestControllerComponent,

		POST ("/path/a",&TestController::postUserJson)
	};

	{
		reproweb::WebServer server(ctx);

		nextTick()
		.then( [&result,&server]()
		{
			HttpClient::url("http://localhost:8765/path/a")
			->POST("{\"login\" : \"littlemole\",\"pwd\" : \"secret\",\"tags\" : [\"one\",\"two\",\"three\"],\"username\" : \"mike\"\n}")
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
    EXPECT_EQ("{\"login\":\"littlemole\",\"pwd\":\"secret\",\"tags\":[\"one\",\"two\",\"three\"],\"username\":\"mike\"}",result);
    MOL_TEST_ASSERT_CNTS(0,0);
}



#ifdef _RESUMABLE_FUNCTIONS_SUPPORTED

TEST_F(BasicTest, SimpleRestCoro) 
{
	std::string result;

	WebApplicationContext ctx {

		LoggerComponent,
		TestControllerComponent,

		GET ("/path/a",&TestController::getUserCoro)
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
    EXPECT_EQ("{\"user\":{\"login\":\"littlemole\",\"pwd\":\"secret\",\"tags\":[\"one\",\"two\",\"three\"],\"username\":\"mike\"}}",result);
    MOL_TEST_ASSERT_CNTS(0,0);
}

TEST_F(BasicTest, SimpleRestPostCoro) 
{
	std::string result;

	WebApplicationContext ctx {

		LoggerComponent,
		TestControllerComponent,

		POST ("/path/a",&TestController::postUserCoro)
	};

	{
		reproweb::WebServer server(ctx);

		nextTick()
		.then( [&result,&server]()
		{
			HttpClient::url("http://localhost:8765/path/a")
			->POST("{\"login\":\"littlemole\",\"pwd\":\"secret\",\"tags\":[\"one\",\"two\",\"three\"],\"username\":\"mike\"}")
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
    EXPECT_EQ("{\"user\":{\"login\":\"littlemole\",\"pwd\":\"secret\",\"tags\":[\"one\",\"two\",\"three\"],\"username\":\"mike\"}}",result);
    MOL_TEST_ASSERT_CNTS(0,0);
}

TEST_F(BasicTest, SimpleRestPostJsonCoro) 
{
	std::string result;

	WebApplicationContext ctx {

		LoggerComponent,
		TestControllerComponent,

		POST ("/path/a",&TestController::postUserJsonCoro)
	};
 
	{
		reproweb::WebServer server(ctx);

		nextTick()
		.then( [&result,&server]()
		{
			HttpClient::url("http://localhost:8765/path/a")
			->POST("{\"login\" : \"littlemole\",\"pwd\" : \"secret\",\"tags\" : [\"one\",\"two\",\"three\"],\"username\" : \"mike\"\n}")
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
    EXPECT_EQ("{\"login\":\"littlemole\",\"pwd\":\"secret\",\"tags\":[\"one\",\"two\",\"three\"],\"username\":\"mike\"}",result);
    MOL_TEST_ASSERT_CNTS(0,0);
}

#endif

 
TEST_F(BasicTest, Invocable) 
{
	User user{ "mike", "littlemole", "secret", { "one", "two", "three"} };

	std::cout << "invocable: " << reproweb::has_valid<User>::value << std::endl;

	if ( reproweb::has_valid<User>::value )
	{
		reproweb::call_valid::invoke(user);
	}

	Logger logger;

	std::cout << "invocable: " << reproweb::has_valid<Logger>::value << std::endl;

	if ( reproweb::has_valid<Logger>::value )
	{
		reproweb::call_valid::invoke(logger);
	}

}

int main(int argc, char **argv)
{
	prio::Libraries<prio::EventLoop,cryptoneat::SSLUser> init;

    ::testing::InitGoogleTest(&argc, argv);
    int r = RUN_ALL_TESTS();

    return r;
}
