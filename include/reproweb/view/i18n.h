#ifndef INCLUDE_REPROWEB_I18N_TPL_H_
#define INCLUDE_REPROWEB_I18N_TPL_H_

#include <regex>
#include "priohttp/common.h"

namespace reproweb {


class I18N 
{
public:

	I18N(const std::string& base, const std::vector<std::string>& locales)
	{
		load(base,locales);
	}

	std::string find_locale(std::string locale)
	{
		if(map_.count(locale) == 0)
		{
			auto v = prio::split(locale,'_');
			if ( v.size() == 2 )
			{				
				if ( map_.count(v[0]) == 0 )
				{
					locale = "";
				}
				else
				{
					locale = v[0];
				}
			}
			else
			{
				locale = "";
			}
		}

		return locale;
	}

	std::string key(std::string locale, const std::string& k)
	{
		locale = find_locale(locale);
		return get_key(locale,k);
	}

	std::string render(std::string locale, const std::string& txt)
	{
		locale = find_locale(locale);

		std::map<std::string,std::string> props = map_[locale];
		std::regex r("<!--#i18n\\s+key\\s*=\\s*[\"']([^'\"]*)[\"']\\s*-->");
		std::smatch match;

		std::string::const_iterator start = txt.begin();
		std::string::const_iterator end   = txt.end();    

		std::ostringstream result;
		
		while (std::regex_search (start,end,match,r)) 
		{
			if ( match.size() > 1 )
			{
				result.write(&*start,match[0].first-start);

				std::string key = match[1];
				if( props.count(key)==0 && map_[""].count(key)==0 )
				{
					result << "key: '" << key << "' not found for i18n using locale " << locale;
				}
				else
				{
					std::string value = get_key(locale,key);
					result.write(value.c_str(),value.size());
				}
			}
			start = match[0].second;			
		}
	    result.write(&*start,end-start);
		return result.str();
	}


private:

	std::map<std::string,std::map<std::string,std::string>> map_;

	std::string get_key(std::string locale, const std::string& k)
	{
		if( map_[locale].count(k) > 0 )
		{
 			return map_[locale][k];
		}
		return map_[""][k];
	}

	void parse(const std::string& locale,const std::string& content)
	{
 		std::vector<std::string> lines;
		std::istringstream iss(content);
		while(!iss.eof())
		{
			std::string line;
			std::getline(iss,line);
			if(line.empty())
			{
				continue;
			}
			if(line[0] == '#')
			{
				continue;
			}
			if (line[line.length()-1] != '\\')
			{
				lines.push_back(line);
			}
			else
			{
				if(lines.empty())
				{
					lines.push_back("");
				}
				lines.back().append(line.substr(0,line.length()-1));
			}
		}

		for(auto& line : lines)
		{
			std::size_t pos = line.find("=");
			if (pos == std::string::npos)
			{
				continue;
			}
			std::string key = line.substr(0,pos);
			std::string value = line.substr(pos+1);
			map_[locale][key] = value;
		}
	}

	void load(const std::string& locale,const std::string& path)
	{
		std::string content = prio::slurp(path);
		if ( content.empty() )
		{
			return;
		}
		
		//std::cout << "load locales: " << locale << " from " << path << std::endl;
		
		parse(locale,content);
	}


	void load(const std::string& base, const std::vector<std::string>& locales)
	{
		std::string path_base = prio::get_current_work_dir()+ base;
		std::string path = prio::real_path(path_base );		

		if ( path.substr(0,path_base.length()) != path_base )
		{
			return;
		} 

		load("",path);
		for( auto& l : locales)
		{
			load(l,path + "." + l);
		}
	}

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

