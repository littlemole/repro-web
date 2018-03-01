#ifndef INCLUDE_CSGI_MUSTACHE_TPL_H_
#define INCLUDE_CSGI_MUSTACHE_TPL_H_

#include "reproweb/json/json.h"
#include "reproweb/view/mustache.hpp"

namespace reproweb  {

class mustache
{
public:
	using Mustache = Kainjow::BasicMustache<std::string>;
	using Data = Mustache::Data;

	mustache( const std::string& tpl);

	std::string render(Json::Value data);
	Data fromJson(Json::Value& data);

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

private:
	std::string path_;
	std::map<std::string,std::string> templates_;
};

TplStore& templates();

std::string render(const std::string& tpl, Json::Value val);
std::string render(const std::string& tpl, const std::string& json);

class view_templates
{
public:
	view_templates(const std::string& path)
	{
		templates().load(path);
	}
};

}

#endif /* INCLUDE_CSGI_MUSTACHE_TPL_H_ */
