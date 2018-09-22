#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_VIEW_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_VIEW_DEFINE_

#include "reproweb/tools/config.h"
#include "reproweb/ctrl/ssi.h"
#include "entities.h"


class View
{
public:

	View(
		std::shared_ptr<AppConfig> conf, 
		std::shared_ptr<TplStore> tpls, 
		std::shared_ptr<I18N> i18n )
		: templates_(tpls), i18n_(i18n)
	{
		version_ = conf->getString("version");
	}

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
	std::string version_;

	Async render(Request& req, Response& res, const std::string& page, const std::string& errMsg)
	{
		return render(req,res,page,error_msg(Valid::locale(req),errMsg));
	}

	Async render(Request& req, Response& res, const std::string& page, Json::Value value)
	{
		value["page"] = page;
		value["version"] = version_;

		std::string locale = Valid::locale(req);
		std::string view = templates_->get(page);
 
		std::string txt = co_await SSIResolver::resolve(req,view);

		std::string tmpl = i18n_->render(locale,txt);

		std::string content = mustache::render(tmpl,value);

		res
		.body(content)
		.contentType("text/html")
		.ok()
		.flush();

		co_return;
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

