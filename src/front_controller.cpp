#include "reproweb/ctrl/front_controller.h"
#include "reproweb/ctrl/filter.h"
#include "priocpp/api.h"
#include "priocpp/task.h"
#include "priohttp/response.h"
#include "priohttp/http_server.h"
#include "reproweb/ctrl/handler_info.h"
#include "diycpp/ctx.h"
#include <fstream>
#include <iostream>

#ifndef _WIN32
#include <unistd.h>
#else
#include <direct.h>
#define getcwd _getcwd
#endif

namespace reproweb  {



////////////////////////////////////////////////////////////////////////////

FrontController::FrontController(std::shared_ptr<diy::Context> ctx)
  : ctx_(ctx) 
{}

void  FrontController::handle_exception(const std::exception& ex, prio::Request& req, prio::Response& res)
{
	for( auto& h : ex_handlers_)
	{
		if ( h->match(ex))
		{
			h->invoke(ex,req,res);
			return;
		}
	}

	for( auto& h : ex_handlers_)
	{
		if ( h->isa(ex))
		{
			h->invoke(ex,req,res);
			return;
		}
	}

    res.error().body(ex.what()).flush();
}


FrontController& FrontController::registerHandler( const std::string& method, const std::string& path, http_handler_t handler)
{
    handlers_.push_back( std::make_unique<HandlerInfo>(method,path,handler) );
    return *this;
}

FrontController& FrontController::registerFilter( const std::string& method, const std::string& path, http_filter_t handler, int prio )
{
    filters_.push_back( std::make_unique<HttpFilterInfo>(method,path,handler,prio) );
    return *this;
}


FrontController& FrontController::registerCompletionFilter( const std::string& method, const std::string& path, http_filter_t handler, int prio )
{
    completion_filters_.push_back( std::make_unique<HttpFilterInfo>(method,path,handler,prio) );
    return *this;
}

FrontController& FrontController::registerStaticHandler( const std::string& method, const std::string& path, http_handler_t handler)
{
    staticHandlers_.push_back( std::make_unique<HandlerInfo>(method,path,handler) );
    return *this;
}

////////////////////////////////////////////////////////////////////////////


template<class T>
HttpFilterInfo* terminator( T t)
{
	return new HttpFilterInfo("","",t);
}


FilterChain::Ptr make_filter_chain(
		std::vector<std::unique_ptr<HttpFilterInfo>>& filters,
		const std::string& method,
		const std::string& path,
		std::function<void(prio::Request&,prio::Response&,std::shared_ptr<FilterChain>)> f
	)
{
    FilterChainBuilder builder;

    for( size_t j = 0; j < filters.size(); j++)
    {
        if ( filters[j]->match(method,path))
        {
            builder.add(filters[j].get());
        }
    }

    FilterChain::Ptr chain = builder.build( terminator(f) );
    return chain;
}

////////////////////////////////////////////////////////////////////////////

void FrontController::handle_request(
		HandlerInfo* h,
	    prio::Request& req,
		prio::Response& res,
		const std::string& method,
		const std::string& path)
{
    FilterChain::Ptr chain =  make_filter_chain(
    	filters_,
		method,
		path,
    	[h] (prio::Request& req, prio::Response& res, std::shared_ptr<FilterChain> chain)
		{

            h->invoke(req,res);
        }
    );

    FilterChain::Ptr completion_chain =  make_filter_chain(
    	completion_filters_,
		method,
		path,
    	[] (prio::Request& req, prio::Response& res, std::shared_ptr<FilterChain> chain)
		{}
    );

    prio::HttpResponse& response = (prio::HttpResponse&)res;
    response.onFlush([completion_chain,this](prio::Request& req, prio::Response& res)
    {
        try
        {
        	completion_chain->filter(req,res);
        }
        catch(const std::exception& ex)
        {
        	handle_exception(ex,req,res);
        }
    });

    try
    {
        chain->filter(req,res);
    }
    catch(const std::exception& ex)
    {
    	handle_exception(ex,req,res);
    }
    return;
}

void FrontController::dispatch(const std::string& path, prio::Request& req, prio::Response& res)
{
	std::string method = req.path.method();

	auto handler = find_handler(path, method, req, res);
	if (!handler)
	{
		res.not_found().flush();
		return;
	}

	try
	{
		handler->invoke(req, res);
	}
	catch (const std::exception& ex)
	{
		handle_exception(ex, req, res);
	}
}

HandlerInfo* FrontController::find_handler(
	const std::string& path, 
	const std::string& method, 
	prio::Request& req, 
	prio::Response& res)
{
	prio::HttpRequest& request = (prio::HttpRequest&)req;

	for (size_t i = 0; i < handlers_.size(); i++)
	{
		auto h = handlers_[i].get();

		if (request.match(h->method(), h->regex(), h->args()))
		{
			return h;
		}
	}

	for (size_t i = 0; i < staticHandlers_.size(); i++)
	{
		auto h = staticHandlers_[i].get();

		prio::patharguments_t args;
		if (h->match(method, path, args))
		{
			return h;
		}
	}

	return nullptr;
}

void FrontController::request_handler( prio::Request& req, prio::Response& res )
{
    std::string method = req.path.method();
    std::string path   = req.path.path();

    std::shared_ptr<diy::Context> ctx = std::make_shared<diy::Context>( ctx_.get() );
    req.attributes.set("ctx", ctx);

	auto handler = find_handler(path, method, req, res);
	if (!handler)
	{
		res.not_found().flush();
		return;
	}

	handle_request(handler, req, res, method, path);
}

/*
FrontController& frontController()
{
    static FrontController frontController;
    return frontController;
}
*/

////////////////////////////////////////////////////////////////////////////



std::string static_content::get_mime( const std::map<std::string,std::string>& mime_, const std::string& fp )
{
	size_t pos = fp.find_last_of('.');
	if ( pos == std::string::npos)
	{
		return "text/plain";
	}
	std::string ext = fp.substr(pos+1);
	if ( mime_.count(ext) == 0 )
	{
		return "text/plain";
	}

	return mime_.at(ext);
}


static_content::static_content(const std::string& htdocs_path,const std::string& mime_file_path)
	: htdocs_(htdocs_path), mime_(mime_file_path)
{   
} 
   
void static_content::register_static_handler(diy::Context* ctx)
{
	std::map<std::string,std::string> map_;
    char* cwd = getcwd(0,0);
    std::string path_ = cwd;
    path_ += htdocs_;
    free(cwd); 

    std::ifstream ifs;
    ifs.open(mime_);
    while(ifs)
    {
    	std::string line;
    	std::getline(ifs,line);
    	line = prio::trim(line); 
    	if(line.empty()) continue;

    	if(line[0] == '#') continue;

    	size_t pos;
    	pos = line.find_first_of(" \t\r\n");
    	if ( pos != std::string::npos )
    	{
    		std::string mime = line.substr(0,pos);
    		std::string exts = prio::trim(line.substr(pos));
    		auto v = prio::split(exts,' ');
    		for ( auto e : v )
    		{
    			map_[e] = mime;
    		}
    	}
    }
	ifs.close();

    http_handler_t handler = [path_,map_](prio::Request& req, prio::Response& res)
    {
        std::regex e ("\\.\\.");
        std::string path = std::regex_replace(req.path.path(),e,"");
        std::string fp = path_ +  path;

    	res.contentType(static_content::get_mime(map_,fp));

    	res.header("Access-Control-Allow-Origin", "*");

        prio::task([fp](){
        	return prio::slurp(fp);
    	})
        .then([&res](std::string data){
        	res.body(data);
        	res.ok().flush();
        });
    };

	auto fc = diy::inject<FrontController>(*ctx);
    fc->registerStaticHandler("GET","/.*",handler);
}

static_content::~static_content() 
{
}



} // end namespace csgi



