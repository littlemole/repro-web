#ifndef INCLUDE_CSGI_MUSTACHE_TPL_H_
#define INCLUDE_CSGI_MUSTACHE_TPL_H_

//! \file tpl.h
//! \defgroup view

#include "reproweb/json/json.h"
#include "reproweb/view/i18n.h"
#include "reproweb/ctrl/ssi.h"
#include "reproweb/view/mustache.hpp"
#include <diycpp/ctx.h>
#include <regex>

namespace reproweb  {

//////////////////////////////////////////////////////

//! Mustache wrapper
//! \ingroup view
class mustache
{
public:
	using Mustache = Kainjow::BasicMustache<std::string>;
	using Data = Mustache::Data;

	//! constrcut mustache template from string
	mustache( const std::string& tpl);

	//! \private
	std::string render(Data& data);
	//! render template with data from JSON
	std::string render(Json::Value data);

	//! \private
	static std::string render(const std::string& tpl,Data& data);
	//! render template with data from JSON
	static std::string render(const std::string& tpl,Json::Value data);

	//! \private
	static Data fromJson(Json::Value& data);

private:
	std::string template_;
};

//! Mustache Template Store
//! \ingroup view

class TplStore
{
public:

	//! construct empty template store
	TplStore();

	//! register a stemplate with store by name
	void register_tpl(const std::string& name, const std::string& tpl);
	//! unregister a template
	void unregister_tpl(const std::string& name);
	//! get template by name
	std::string& get(const std::string& name);

	//! load all templates from given directory
	void load(const std::string& path);

	//! render named template with given data from JSON
	std::string render(const std::string& tpl, Json::Value val);
	//! \private
	std::string render(const std::string& tpl, const std::string& json);

private:
	std::string path_;
	std::map<std::string,std::string> templates_;
};

//! declare views with mustache templates in WebApplicationContext
//! \ingroup view

class view_templates
{
public:

	typedef view_templates type;

	//! construct views from template files at directory
    view_templates(const std::string& path);

	//! \private
    void ctx_register(diy::Context* ctx);

private:

    std::string path_;
};

//////////////////////////////////////////////////////

//! abstract view base class
//! \ingroup view

class AbstractView
{
public:

	AbstractView();
	virtual ~AbstractView();

	//! get locale from request
	static std::string get_locale(prio::Request& req);

	//! render given page with JSON data specified and flush() the response
	virtual repro::Future<> render(prio::Request& req, prio::Response& res, const std::string& page, Json::Value value);

	//! render given page with JSON data and return template as string
	virtual repro::Future<std::string> render_content(prio::Request& req, const std::string& page, Json::Value value) = 0;

	//! generate HTTP redirect response
	void redirect(prio::Response& res, const std::string& url);
	//! flush content
	void flush_content(prio::Response& res,const std::string& content);
	//! render error
	void render_error(prio::Response& res, const std::exception& ex);
};

//! Mustache Template view
//! \ingroup view

class TemplateView : public AbstractView
{
public:

	TemplateView( std::shared_ptr<TplStore> tpls);

	virtual repro::Future<std::string> render_content(prio::Request& req, const std::string& page, Json::Value value);

protected:

	std::shared_ptr<TplStore> templates_;
};

//! I18n View decorator
//! \ingroup view

class ViewI18nDecorator : public AbstractView
{
public:

	ViewI18nDecorator( std::shared_ptr<I18N> i18n,  AbstractView* av);

	virtual repro::Future<std::string> render_content(prio::Request& req, const std::string& page, Json::Value value);

protected:
	std::shared_ptr<I18N> i18n_;
	std::unique_ptr<AbstractView> view_;
};


//! Mustache View Decorator
//! \ingroup view

class ViewMustacheDecorator : public AbstractView
{
public:

	ViewMustacheDecorator(  AbstractView* av);

	virtual repro::Future<std::string> render_content(prio::Request& req, const std::string& page, Json::Value value);

protected:
	std::unique_ptr<AbstractView> view_;
};

//! SSI View Decorator
//! \ingroup view

class ViewSSIDecorator : public AbstractView
{
public:

	ViewSSIDecorator( AbstractView* av);

	virtual repro::Future<std::string> render_content(prio::Request& req, const std::string& page, Json::Value value);

protected:
	std::unique_ptr<AbstractView> view_;
};

//////////////////////////////////////////////////////

//! I18nSSIMustache View - view that does i18n and SSI and Mustache
//! \ingroup view

class I18nSSIMustacheView : public ViewMustacheDecorator
{
public:

	I18nSSIMustacheView( std::shared_ptr<TplStore> tpls, std::shared_ptr<I18N> i18n );
};

//! I18nMustacheView - view that does i18n and mustache
//! \ingroup view

class I18nMustacheView : public ViewMustacheDecorator
{
public:

	I18nMustacheView( std::shared_ptr<TplStore> tpls, std::shared_ptr<I18N> i18n );
};

//! MustacheView - only Mustache
//! \ingroup view

class MustacheView : public ViewMustacheDecorator
{
public:

	MustacheView( std::shared_ptr<TplStore> tpls );
};

//! SSIMustacheView - only Mustache and SSI
//! \ingroup view
class SSIMustacheView : public ViewMustacheDecorator
{
public:

	SSIMustacheView( std::shared_ptr<TplStore> tpls );
};


//////////////////////////////////////////////////////

}

#endif /* INCLUDE_CSGI_MUSTACHE_TPL_H_ */
