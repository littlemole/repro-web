#include "reproweb/ctrl/ssi.h"

#ifndef _WIN32
#include <unistd.h>
#else
#include <direct.h>
#define getcwd _getcwd
#endif

using namespace repro;
using namespace prio;

namespace reproweb {

 

Future<std::string> SSIResolver::resolve( Request& req, const std::string& tmpl)
{
    auto me = std::make_shared<SSIResolver>();
    me->self_ = me;

    return me->fetch(req,tmpl);
}


Future<std::string> SSIResolver::fetch( Request& req, const std::string& tmpl)
{
    auto p = repro::promise<std::string>();

    auto fc = diy::inject<FrontController>(*ctx(req));

    if(!match(tmpl))
    {
        nextTick([this,p]()
        {
            p.reject(repro::Ex("SSI match failed"));
            self_.reset();
        });
        return p.future();
    }

    if(includes_.size() == 0)
    {
        nextTick([this,p]()
        {
            p.resolve(combine());
            self_.reset();
        });
        return p.future();
    }

    resolves_.resize(includes_.size());

    for ( auto& url : includes_ )
    {
        prio::Request request = req;
        request.path.method("GET");
        int n = cnt_;

        auto rejected = std::make_shared<bool>(false);

        fc->include(request,url)
        .then([this,p,n,rejected](std::string s)
        {
            resolves_[n] = s;
            cnt_--;
            if(cnt_ == 0)
            {
                if(!*rejected)
                {
                    p.resolve(combine());
                }
                self_.reset();
            }
        })
        .otherwise([this,p,rejected](const std::exception& ex)
        {
            if(!*rejected)
            {
                p.reject(ex);
                *rejected = true;
            }
            
            cnt_--;
            if(cnt_ == 0)
            {
                self_.reset();
            }
        });

        cnt_++;
    }

    return p.future();
}

std::string SSIResolver::combine()
{
    std::ostringstream oss;

    for( unsigned int i = 0; i < includes_.size(); i++)
    {
        oss << parts_[i] << resolves_[i];
    }
    oss << parts_[parts_.size()-1];

    return oss.str();
}

std::string SSIResolver::tmpl(Request& req, const std::string& htdocs)
{
    std::string path_base = prio::get_current_work_dir() + htdocs;
    std::string path = prio::real_path(path_base + req.path.path());		
    
    // TODO
#ifndef _WIN32
    if ( path.substr(0,path_base.length()) != path_base )
    {
        std::cout << "bad SSI path: " << path << " " << path_base << std::endl;
        return "";
    } 
#endif
    return prio::slurp(path);
}

bool SSIResolver::match(const std::string& tmpl)
{
    std::regex r("<!--#include +virtual=(?:[\"'])([^\"']+)(?:[\"']) *-->");
    std::smatch match;

    std::string::const_iterator start = tmpl.begin();
    std::string::const_iterator end   = tmpl.end();    

    std::vector<std::string> result;
    
    while (std::regex_search (start,end,match,r)) 
    {
        if ( match.size() > 1 )
        {
            parts_.push_back(std::string(start,match[0].first));
            includes_.push_back(match[1]);
        }
        start = match[0].second;
    }
    parts_.push_back(std::string(start,end));

    if ( parts_.size() -1 == includes_.size())
    {
        return true;
    }
            
    return false;
}


ssi_content::ssi_content(const std::string& htdocs_path,const std::string& filter)
	: htdocs_(htdocs_path), filter_(filter)
{   
} 
   
void ssi_content::register_static_handler(diy::Context* ctx)
{
    std::string htdocs = htdocs_;

    http_handler_t handler = [htdocs](prio::Request& req, prio::Response& res)
    {
		res.contentType("text/html");

        std::string tmpl = SSIResolver::tmpl(req,htdocs);

		reproweb::SSIResolver::resolve(req,tmpl)
		.then( [&res](std::string s)
		{
			res.body(s);
			res.ok().flush();
		})
		.otherwise([&res](const std::exception& ex)
		{
			res.error().flush();
		});
    };

	auto fc = diy::inject<FrontController>(*ctx);
    fc->registerStaticHandler("GET",filter_,handler);
}

}