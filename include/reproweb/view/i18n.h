#ifndef INCLUDE_REPROWEB_I18N_TPL_H_
#define INCLUDE_REPROWEB_I18N_TPL_H_

#include <regex>
#include "priohttp/common.h"
#include <diycpp/ctx.h>

namespace reproweb {


class I18N 
{
public:

	I18N(const std::string& base, const std::vector<std::string>& locales);

	std::string find_locale(std::string locale);

	const std::string& key(std::string locale, const std::string& k);

	std::string render(std::string locale, const std::string& txt);

private:

	std::map<std::string,std::map<std::string,std::string>> map_;

	const std::string& get_key(std::string locale, const std::string& k);
	void parse(const std::string& locale,const std::string& content);
	void load(const std::string& locale,const std::string& path);
	void load(const std::string& base, const std::vector<std::string>& locales);
};


class i18n_props
{
public:

    i18n_props(const std::string& path,const std::vector<std::string>& locales)
		: path_(path), locales_(locales)
	{}

    void ctx_register(diy::Context* ctx)
	{
		auto i18n = std::make_shared<I18N>(path_,locales_);
		ctx->registerFactory( typeid(I18N), new diy::FactoryImpl<I18N>(i18n) );
	}

private:

    std::string path_;
    std::vector<std::string> locales_;
};

}


#endif

