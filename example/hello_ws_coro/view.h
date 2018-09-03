#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_VIEW_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_VIEW_DEFINE_

#include <string>
#include <memory>

#include "reproweb/tools/config.h"
#include "reproweb/view/tpl.h"
#include "reproweb/view/i18n.h"
#include "reproweb/ctrl/ssi.h"

using namespace prio;
using namespace repro;
using namespace reproweb;

class View
{
public:

	View(std::shared_ptr<reproweb::Config> conf)
		: config_(conf)
	{
		templates_.load("/view/");
	}

	void render_index(Request& req, Response& res, Json::Value profile)
	{
		render(req,res,"index",profile);
	}

	void render_login(Request& req, Response& res, const std::string& msg )
	{
		Json::Value errorMsg(Json::objectValue);
		errorMsg["errorMsg"] = msg;

		render(req,res,"login",errorMsg);
	}

	void render_registration(Request& req, Response& res, const std::string& msg )
	{
		Json::Value errorMsg(Json::objectValue);
		errorMsg["errorMsg"] = msg;

		render(req,res,"register",errorMsg);
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

	void render(Request& req, Response& res, const std::string& page, Json::Value value)
	{
		std::string view = templates_.get(page);

		value["page"] = page;

		reproweb::SSIResolver::resolve(req,view)
		.then( [&req,&res,value](std::string txt)
		{
			auto h = req.headers.values("Accept-Language");
		    auto lang = h.value().main();

			std::regex e ("-");   	

		    std::string locale = std::regex_replace (lang,e,"_");
			std::cout << locale << std::endl;

			I18N i18n("/locale/properties", {"de","en"} );
			std::string tmpl = i18n.render(locale,txt);

			std::cout << "-------------------------------" << std::endl;
			std::cout << JSON::stringify(value) << std::endl;
			std::cout << "-------------------------------" << std::endl;
 
			std::string content = mustache::render(tmpl,value);

			res
			.body(content)
			.contentType("text/html")
			.ok()
			.flush();
		})
		.otherwise([&res](const std::exception& ex)
		{
			std::cout << ex.what() << std::endl;
			res.error().flush();
		});
	}

};
 
#endif

