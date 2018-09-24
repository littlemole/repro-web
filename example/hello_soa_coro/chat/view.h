#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_VIEW_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_VIEW_DEFINE_

#include "reproweb/tools/config.h"
#include "reproweb/ctrl/ssi.h"
#include "entities.h"


class View : public I18nSSIMustacheView
{
public:

	View(
		std::shared_ptr<AppConfig> conf, 
		std::shared_ptr<TplStore> tpls, 
		std::shared_ptr<I18N> i18n )
		: I18nSSIMustacheView(tpls,i18n),i18n_(i18n)
	{
		version_ = conf->getString("version");
	}

	void render_index(Request& req, Response& res, Json::Value value)
	{
		value["page"] = "index";
		value["version"] = version_;

		render(req,res,"index",value);
	}

	void render_login(Request& req, Response& res, const std::string& errMsg )
	{
		Json::Value value = error_msg(get_locale(req),errMsg);
		value["page"] = "login";
		value["version"] = version_;

		render(req,res,"login",value);
	}

	void render_registration(Request& req, Response& res, const std::string& errMsg )
	{
		Json::Value value = error_msg(get_locale(req),errMsg);
		value["page"] = "register";
		value["version"] = version_;

		render(req,res,"register",value);
	}	

	void redirect_to_index(Response& res, const std::string& sid)
	{
		redirect(
			res.cookie(Cookie("repro_web_sid", sid)),
			"https://localhost:9876/"
		);
	}

	void redirect_to_login(Response& res)
	{
		redirect(res,"https://localhost:9876/login");
	}	

	void redirect_to_registration(Response& res)
	{
		redirect(res,"https://localhost:9876/register");
	}	

private:

	std::shared_ptr<I18N> i18n_;
	std::string version_;

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

