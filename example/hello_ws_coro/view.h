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

	View(std::shared_ptr<Config> conf, std::shared_ptr<I18N> i18n)
		: config_(conf), i18n_(i18n)
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
		errorMsg["errorMsg"] = "";

		if(!msg.empty())
		{
			errorMsg["errorMsg"] = i18n_->key(get_locale(req),msg);
		}

		render(req,res,"login",errorMsg);
	}

	void render_registration(Request& req, Response& res, const std::string& msg )
	{
		Json::Value errorMsg(Json::objectValue);
		errorMsg["errorMsg"] = "";

		if(!msg.empty())
		{
			errorMsg["errorMsg"] = i18n_->key(get_locale(req),msg);
		}

		render(req,res,"register",errorMsg);
	}	

	void redirect_to_index(Response& res, const std::string& sid)
	{
		res
		.cookie(Cookie("repro_web_sid", sid))
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
	TplStore templates_;
	std::shared_ptr<Config> config_;
	std::shared_ptr<I18N> i18n_;

	void render(Request& req, Response& res, const std::string& page, Json::Value value)
	{
		value["page"] = page;

		std::string locale = get_locale(req);
		std::string view = templates_.get(page);

		//std::cout << "---------------------------------" << std::endl;
		//std::cout << locale << ":" << view << std::endl;
 
		SSIResolver::resolve(req,view)
		.then( [this,&res,value,locale](std::string txt)
		{

			//std::cout << "---------------------------------" << std::endl;
			std::string tmpl = i18n_->render(locale,txt);
			//std::cout << tmpl << std::endl;
 	
	 		//std::cout << "---------------------------------" << std::endl;
			std::string content = mustache::render(tmpl,value);
			//std::cout << content << std::endl;
			//std::cout << "---------------------------------" << std::endl;

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

	std::string get_locale(Request& req)
	{
		auto h = req.headers.values("Accept-Language");
		auto lang = h.value().main();
		std::string locale = std::regex_replace (lang,std::regex("-"),"_");		

		return locale;
	}
};
 
#endif

