#ifndef INCLUDE_PROMISE_WEB_CONFIG_H_
#define INCLUDE_PROMISE_WEB_CONFIG_H_

#include "reproweb/json/json.h"
#include "reproweb/view/tpl.h"
#include "reproweb/view/i18n.h"
#include "reproweb/ctrl/ssi.h"
#include "reproweb/ctrl/front_controller.h"
#include "priohttp/common.h"

namespace reproweb  {

class Config
{
public:

	Config()
	{}

	Config( const std::string& path)
	{
		load(path);
	}

	Config& load( const std::string& path)
	{
		std::string f = prio::slurp(path);
		json_ = JSON::parse(f);
		return *this;
	}

	Json::Value& json()
	{
		return json_;
	}

	Json::Value& get(const std::string& key)
	{
		return json_[key];
	}

	std::string getString(const std::string& key)
	{
		return json_[key].asString();
	}

	int getInt(const std::string& key)
	{
		return json_[key].asInt();
	}

private:

	Json::Value json_;
};

class WebAppConfig : public Config
{
public:
	WebAppConfig( const std::string& path, std::shared_ptr<diy::Context> ctx)
	  : Config(path)
	{
		if( json().isMember("i18n"))
		{
			Json::Value localisator = json()["i18n"];
			std::string path = localisator["path"].asString();
			Json::Value locale_array = localisator["locales"];
	
			std::vector<std::string> locales;
			for ( unsigned int i = 0; i < locale_array.size(); i++)
			{
				locales.push_back(locale_array[i].asString());
			}
			
			auto i18n = std::make_shared<I18N>(path,locales);
			ctx->registerFactory( typeid(I18N), new diy::FactoryImpl<I18N>(i18n) );
		}

		if( json().isMember("view"))
		{
			std::string path = getString("view");
	
			auto tpls = std::make_shared<TplStore>();
			tpls->load(path);
			ctx->registerFactory( typeid(TplStore), new diy::FactoryImpl<TplStore>(tpls) );
		}

		if( json().isMember("htdocs"))
		{
			std::string mime = "mime.types";
			std::string path = getString("htdocs");

			if(json().isMember("mime"))
			{
				mime = getString("mime");
			}
	
			auto content = std::make_shared<reproweb::StaticContentHandler>(path,mime);
			ctx->registerFactory( typeid(StaticContentHandler), new diy::FactoryImpl<StaticContentHandler>(content) );

			content->register_static_handler(ctx.get());
		}
	}

};

}


#endif /* INCLUDE_PROMISE_WEB_CONFIG_H_ */
