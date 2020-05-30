#include "gtest/gtest.h"

#include "reprocpp/test.h"
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
using namespace patex;
using namespace meta;

typedef const char* entity_name_t;

//DIY_DEFINE_CONTEXT()

class SerializeTest : public ::testing::Test {
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
 
template<>
struct meta::Data<Input>
{
        static constexpr auto meta()
        {
                return meta::data<Input>(
					entity_root("Input"),
					// note some mappings are duplicated
					// for serialization, last one will overider earlier ones
					// for deserialization however, all will be mapped
					"id", &Input::id,
					"filter", &Input::filter,
					"sid", &Input::sid,
					"Accept-Language", &Input::header,
					"Accept-Language", &Input::lang,
					"sid", &Input::cookie
				);
		}
};


struct User
{
public:

	std::string username;
	std::string login;
	std::string pwd;
	std::vector<std::string> tags;
};


template<>
struct meta::Data<User>
{
        static constexpr auto meta()
        {
                return meta::data<User>(
					entity_root("user"),
					"username", &User::username,
					"login", &User::login,
					"pwd", &User::pwd,
					"tags", &User::tags
				);
		}
};
 
 

void validate(User& user)
{
	reproweb::valid( user.username, std::regex("[^<>&]+"), "invalid username");
	reproweb::valid( user.login, std::regex("[^<>&]+"), "invalid login");
	reproweb::valid( user.pwd, std::regex(".+"), "invalid pwd");
	reproweb::valid( user.tags, std::regex("[0-9a-zA-Z]+"), "invalid tags");
}

#include <reproweb/ws/ws.h>
#include <fcntl.h>
#include <signal.h>

 

TEST_F(SerializeTest, fromParams) 
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

template<>
struct meta::Data<XmlTest::XmlTest1::XmlTest2>
{
        static constexpr auto meta()
        {
                return meta::data<XmlTest::XmlTest1::XmlTest2>(
					"v", &XmlTest::XmlTest1::XmlTest2::v
				);
		}
};

    

template<>
struct meta::Data<XmlTest::XmlTest1>
{
        static constexpr auto meta()
        {
                return meta::data<XmlTest::XmlTest1>(
					member("index", &XmlTest::XmlTest1::index, attribute()),
					"level2", &XmlTest::XmlTest1::level2
				);
		}
};
 

template<>
struct meta::Data<XmlTest>
{
        static constexpr auto meta()
        {
                return meta::data<XmlTest>(
					entity_root("XmlTest"),
					member("id", &XmlTest::id, attribute()),
					"level1", &XmlTest::level1
				);
		}
};
 
 

TEST_F(SerializeTest, toXml) 
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


TEST_F(SerializeTest, toJson) 
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
 
 
TEST_F(SerializeTest, toJsonArray) 
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

	static constexpr auto meta()
	{
		return meta::data(
			"something", &Users::something,
			"users", &Users::users
		);
	}
};
   
TEST_F(SerializeTest, toJsonArray2) 
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

	static constexpr auto meta()
	{
		return meta::data(
			"array", &ArrayTest::array
		);
	}
};
 
TEST_F(SerializeTest, toJsonArray3) 
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

TEST_F(SerializeTest, toJsonArray4) 
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

class TestGetterSetter
{
public:
    int x = 0;
    std::string s;

    int get_x() const
    {
        return x;
    }

    void set_x(int v)
    { 
        x = v;
    }

    std::string get_s() const
    {
        return s;
    }

    void set_s(std::string v)
    {
        s = v;
    }    

    static constexpr auto meta()
    {
        return meta::data<TestGetterSetter>(

            getter_setter("x",&TestGetterSetter::get_x,&TestGetterSetter::set_x),
            getter_setter("s",&TestGetterSetter::get_s,&TestGetterSetter::set_s)
        );
    }
};


TEST_F(SerializeTest, getterSetterJson) 
{
    TestGetterSetter tgs { 42, "Hello World" };

	Json::Value json = toJson(tgs);

	std::string s = JSON::flatten(json); 

	EXPECT_EQ(s,"{\"s\":\"Hello World\",\"x\":42}");

    TestGetterSetter other;
	fromJson(json,other);

	EXPECT_EQ(42,other.x);
	EXPECT_EQ("Hello World",other.s);
}

int main(int argc, char **argv)
{
	prio::Libraries<prio::EventLoop,cryptoneat::SSLUser> init;

    ::testing::InitGoogleTest(&argc, argv);
    int r = RUN_ALL_TESTS();

    return r;
}
