#ifndef INCLUDE_CSGI_MUSTACHE_TPL_H_
#define INCLUDE_CSGI_MUSTACHE_TPL_H_

#include "reproweb/json/json.h"
#include "reproweb/view/mustache.hpp"
#include <diycpp/ctx.h>

namespace reproweb  {

class mustache
{
public:
	using Mustache = Kainjow::BasicMustache<std::string>;
	using Data = Mustache::Data;

	mustache( const std::string& tpl);

	std::string render(Data& data);
	std::string render(Json::Value data);

	static std::string render(const std::string& tpl,Data& data);
	static std::string render(const std::string& tpl,Json::Value data);
	static Data fromJson(Json::Value& data);

private:
	std::string template_;
};

class TplStore
{
public:

	TplStore();

	void register_tpl(const std::string& name, const std::string& tpl);
	void unregister_tpl(const std::string& name);
	std::string& get(const std::string& name);

	void load(const std::string& path);

	std::string render(const std::string& tpl, Json::Value val);
	std::string render(const std::string& tpl, const std::string& json);

private:
	std::string path_;
	std::map<std::string,std::string> templates_;
};

class view_templates
{
public:

    view_templates(const std::string& path)
		: path_(path)
	{}

    void ctx_register(diy::Context* ctx)
	{
		auto tpls = std::make_shared<TplStore>();
		tpls->load(path_);
		ctx->registerFactory( typeid(TplStore), new diy::FactoryImpl<TplStore>(tpls) );
	}

private:

    std::string path_;
};

}

#endif /* INCLUDE_CSGI_MUSTACHE_TPL_H_ */
