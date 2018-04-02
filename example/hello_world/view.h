#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_VIEW_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_VIEW_DEFINE_

#include <string>
#include <memory>

#include "reproweb/tools/config.h"
#include "reproweb/view/tpl.h"

using namespace prio;

class View
{
public:

	View(std::shared_ptr<reproweb::Config> conf)
		: config_(conf)
	{
		templates_.load("/view/");
	}

	void render_index(Response& res, const Json::Value& profile)
	{
		res
		.body(templates_.render("index", profile ))
		.contentType("text/html")
		.ok()
		.flush();
	}

	void render_login(Response& res, const std::string& msg )
	{
		Json::Value errorMsg(Json::objectValue);
		errorMsg["errorMsg"] = msg;

		res
		.body(templates_.render("login", errorMsg ))
		.contentType("text/html")
		.ok()
		.flush();
	}

	void render_registration(Response& res, const std::string& msg )
	{
		Json::Value errorMsg(Json::objectValue);
		errorMsg["errorMsg"] = msg;

		res
		.body(templates_.render("register", errorMsg ))
		.contentType("text/html")
		.ok()
		.flush();
	}	

	void redirect_to_index(Response& res, const std::string& sid)
	{
		res
		.cookie(prio::Cookie("repro_web_sid", sid))
		.redirect("https://localhost:9876/")
		.flush();
	}

	void redirect_to_login(Response& res)
	{
		res
		.redirect("https://localhost:9876/login")
		.flush();
	}	

	void redirect_to_registration(Response& res)
	{
		res
		.redirect("https://localhost:9876/register")
		.flush();
	}	

private:
	reproweb::TplStore templates_;
	std::shared_ptr<reproweb::Config> config_;
};
 
#endif

