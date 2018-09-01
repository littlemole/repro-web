#include "reproweb/view/tpl.h"
#include "priohttp/common.h"

#ifndef _WIN32
#include <unistd.h>
#else
#include <direct.h>
#define getcwd _getcwd
#endif

namespace reproweb  {


mustache::mustache( const std::string& tpl)
	: template_(tpl)
{}

std::string mustache::render(Json::Value data)
{
	return render(template_,data);
}


std::string mustache::render(Data& data)
{
	return render(template_,data);
}

std::string mustache::render(const std::string& tpl, Json::Value data)
{
	Data d(fromJson(data));
	return render(tpl,d);
}


std::string mustache::render(const std::string& tpl, Data& data)
{
	std::ostringstream oss;

	Mustache tmpl{tpl};

	tmpl.render(data, oss);

	return oss.str();
}

mustache::Data mustache::fromJson(Json::Value& data)
{
	if ( data.isArray())
	{
		Data d(Data::List());
		for( unsigned int i = 0; i < data.size(); i++)
		{
			d.push_back( fromJson( data[i]));
		}
		return d;
	}
	if ( data.isObject())
	{
		Data d(Data::Type::Object);
		std::vector<std::string> members = data.getMemberNames();
		for( std::string m : members)
		{
			d.set(m, fromJson( data[m]));
		}
		return d;
	}
	if ( data.isNull())
	{
		return Data("null");
	}
	return Data(data.asString());
}



TplStore::TplStore()
{
	path_ = prio::get_current_work_dir();
}

void TplStore::register_tpl(const std::string& name, const std::string& tpl)
{
	templates_[name] = tpl;
}

void TplStore::unregister_tpl(const std::string& name)
{
	templates_.erase(name);
}

std::string& TplStore::get(const std::string& name)
{
	return templates_[name];
}

void TplStore::load(const std::string& path)
{
	std::string p = path_ + path;
	p = prio::real_path(p);

	std::vector<std::string> v = prio::glob(p);
	for ( std::string s : v )
	{
		std::string fn = p + "/" + s;
		std::string f = prio::slurp(fn);
		if ( f.empty() )
		{
			continue;
		}
		std::string n = s;
		size_t pos = s.find_last_of(".");
		if ( pos != std::string::npos)
		{
			n = s.substr(0,pos);
		}
		register_tpl(n,f);
	}
}



std::string TplStore::render(const std::string& tpl, Json::Value val)
{
	mustache m = {
		get(tpl)
	};

	return m.render(val);
}

std::string TplStore::render(const std::string& tpl, const std::string& json)
{
	mustache m = {
		get(tpl)
	};

	Json::Value val = reproweb::JSON::parse(json);
	return m.render(val);
}

}
