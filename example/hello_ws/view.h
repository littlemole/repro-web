#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_VIEW_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_VIEW_DEFINE_

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

	View(
		std::shared_ptr<TplStore> tpls, 
		std::shared_ptr<I18N> i18n )
		: templates_(tpls), i18n_(i18n)
	{}

	void render_index(Request& req, Response& res, Json::Value profile)
	{
		render(req,res,"index",profile);
	}

	void render_login(Request& req, Response& res, const std::string& errMsg )
	{
		render(req,res,"login",errMsg);
	}

	void render_registration(Request& req, Response& res, const std::string& errMsg )
	{
		render(req,res,"register",errMsg);
	}	

	void render_error(const std::exception& ex, Response& res)
	{
		std::ostringstream oss;
		oss << typeid(ex).name() << ":" << ex.what() << std::endl;

		res
		.body(oss.str())
		.error()
		.flush();
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

	std::shared_ptr<TplStore> templates_;
	std::shared_ptr<I18N> i18n_;

	void render(Request& req, Response& res, const std::string& page, const std::string& errMsg)
	{
		return render(req,res,page,error_msg(Valid::locale(req),errMsg));
	}

	void render(Request& req, Response& res, const std::string& page, Json::Value value)
	{
		value["page"] = page;

		std::string locale = Valid::locale(req);
		std::string view = templates_->get(page);

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

	Json::Value error_msg(const std::string& locale, const std::string& msg )
	{
		Json::Value errorMsg(Json::objectValue);
		errorMsg["errorMsg"] = "";

		if(!msg.empty())
		{
			errorMsg["errorMsg"] = i18n_->key(locale,msg);
		}

		return errorMsg;
	}	
};
 
#endif

