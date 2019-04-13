#ifndef INCLUDE_CSGI_MUSTACHE_TPL_H_
#define INCLUDE_CSGI_MUSTACHE_TPL_H_

#include "reproweb/json/json.h"
#include "reproweb/view/i18n.h"
#include "reproweb/ctrl/ssi.h"
#include "reproweb/view/mustache.hpp"
#include <diycpp/ctx.h>
#include <regex>

namespace reproweb  {

//////////////////////////////////////////////////////

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

	typedef view_templates type;

    view_templates(const std::string& path);

    void ctx_register(diy::Context* ctx);

private:

    std::string path_;
};

//////////////////////////////////////////////////////


class AbstractView
{
public:

	AbstractView();
	virtual ~AbstractView();

	static std::string get_locale(prio::Request& req);

	virtual repro::Future<> render(prio::Request& req, prio::Response& res, const std::string& page, Json::Value value);

	virtual repro::Future<std::string> render_content(prio::Request& req, const std::string& page, Json::Value value) = 0;

	void redirect(prio::Response& res, const std::string& url);
	void flush_content(prio::Response& res,const std::string& content);
	void render_error(prio::Response& res, const std::exception& ex);
};


class TemplateView : public AbstractView
{
public:

	TemplateView( std::shared_ptr<TplStore> tpls);

	virtual repro::Future<std::string> render_content(prio::Request& req, const std::string& page, Json::Value value);

protected:

	std::shared_ptr<TplStore> templates_;
};


class ViewI18nDecorator : public AbstractView
{
public:

	ViewI18nDecorator( std::shared_ptr<I18N> i18n,  AbstractView* av);

	virtual repro::Future<std::string> render_content(prio::Request& req, const std::string& page, Json::Value value);

protected:
	std::shared_ptr<I18N> i18n_;
	std::unique_ptr<AbstractView> view_;
};


class ViewMustacheDecorator : public AbstractView
{
public:

	ViewMustacheDecorator(  AbstractView* av);

	virtual repro::Future<std::string> render_content(prio::Request& req, const std::string& page, Json::Value value);

protected:
	std::unique_ptr<AbstractView> view_;
};


class ViewSSIDecorator : public AbstractView
{
public:

	ViewSSIDecorator( AbstractView* av);

	virtual repro::Future<std::string> render_content(prio::Request& req, const std::string& page, Json::Value value);

protected:
	std::unique_ptr<AbstractView> view_;
};

//////////////////////////////////////////////////////

class I18nSSIMustacheView : public ViewMustacheDecorator
{
public:

	I18nSSIMustacheView( std::shared_ptr<TplStore> tpls, std::shared_ptr<I18N> i18n );
};

class I18nMustacheView : public ViewMustacheDecorator
{
public:

	I18nMustacheView( std::shared_ptr<TplStore> tpls, std::shared_ptr<I18N> i18n );
};

class MustacheView : public ViewMustacheDecorator
{
public:

	MustacheView( std::shared_ptr<TplStore> tpls );
};

//////////////////////////////////////////////////////

}

#endif /* INCLUDE_CSGI_MUSTACHE_TPL_H_ */
