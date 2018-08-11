#include "reproweb/ctrl/ssi.h"

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
        nextTick([p]()
        {
            p.reject(repro::Ex("SSI match failed"));
        });
        return p.future();
    }

    if(includes_.size() == 0)
    {
        nextTick([this,p]()
        {
            p.resolve(combine());
        });
        return p.future();
    }

    resolves_.resize(includes_.size());

    for ( auto& url : includes_ )
    {
        prio::Request request = req;
        int n = cnt_;

        fc->include(request,url)
        .then([this,p,n](std::string s)
        {
            resolves_[n] = s;
            cnt_--;
            if(cnt_ == 0)
            {
                p.resolve(combine());
            }
        })
        .otherwise(reject(p));

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
    char* cwd = getcwd(0,0);
    std::string path_ = cwd;
    path_ += htdocs;
    free(cwd); 

    std::regex e ("\\.\\.");
    std::string path = std::regex_replace(req.path.path(),e,"");
    std::string fp = path_ +  path;		

    return prio::slurp(fp);
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

/*
		char* cwd = getcwd(0,0);
    	std::string path_ = cwd;
    	path_ += htdocs;
    	free(cwd); 

        std::regex e ("\\.\\.");
        std::string path = std::regex_replace(req.path.path(),e,"");
        std::string fp = path_ +  path;		

		std::string tmpl = prio::slurp(fp);
*/

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