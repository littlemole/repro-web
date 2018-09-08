#ifndef _MOL_DEF_GUARD_DEFINE_MOD_HTTP_REQUEST_PROCESSOR_DEF_GUARD_
#define _MOL_DEF_GUARD_DEFINE_MOD_HTTP_REQUEST_PROCESSOR_DEF_GUARD_

#include "reproweb/ctrl/handler_info.h"
#include "diycpp/ctx.h"

namespace reproweb  {

 
 std::shared_ptr<diy::Context> ctx( prio::Request& req);

class FrontController
{
public:

    FrontController(std::shared_ptr<diy::Context> ctx);
    
    FrontController& registerHandler( const std::string& method, const std::string& path, http_handler_t handler);
    FrontController& registerFilter( const std::string& method, const std::string& path, http_filter_t filter, int prio = 0);
    FrontController& registerCompletionFilter( const std::string& method, const std::string& path, http_filter_t filter, int prio = 0);
    FrontController& registerStaticHandler( const std::string& method, const std::string& path, http_handler_t handler);

    template<class E>
    FrontController& registerExceptionHandler( typename ExceptionHandlerInfo<E>::ex_handler_t handler)
    {
		//std::cout << "register ex handler " << typeid(E).name() << std::endl;

    	ex_handlers_.push_back(std::unique_ptr<ExHandlerInfo>(new ExceptionHandlerInfo<E>(handler)));
    	return *this;
    }

    void request_handler( prio::Request& req, prio::Response& res );
	void dispatch(const std::string& path,prio::Request& req, prio::Response& res);
    repro::Future<std::string> include(const prio::Request& req, const std::string& path);


    FrontController(const FrontController& rhs) = delete;
    FrontController& operator=(const FrontController& rhs) = delete;
    
    void handle_exception(const std::exception& ex, prio::Request& req, prio::Response& res);


    auto handle_ex(prio::Request& req, prio::Response& res)
    {
        return [this,&req,&res]( const std::exception& ex)
        {
            handle_exception(ex,req,res);
        };
    }

private:

    std::vector<std::unique_ptr<HandlerInfo>> handlers_;
    std::vector<std::unique_ptr<HandlerInfo>> staticHandlers_;
    std::vector<std::unique_ptr<HttpFilterInfo>> filters_;
    std::vector<std::unique_ptr<HttpFilterInfo>> completion_filters_;
    std::vector<std::unique_ptr<ExHandlerInfo>> ex_handlers_;

    void handle_request(
		HandlerInfo* h,
		prio::Request& req,
		prio::Response& res,
		const std::string& method,
		const std::string& path
	);

	HandlerInfo* find_handler(
		const std::string& path,
		const std::string& method, 
		prio::Request& req, 
		prio::Response& res
	);

    std::shared_ptr<diy::Context> ctx_;
};

class StaticContentHandler
{
public:

    StaticContentHandler(const std::string& htdocs_path,const std::string& mime_file_path);

    ~StaticContentHandler();

    void register_static_handler(diy::Context* ctx);

private:

    std::string htdocs_;
    std::string mime_;


    static std::string get_mime( const std::map<std::string,std::string>& mime_, const std::string& fp );
};


class static_content
{
public:

    static_content(const std::string& htdocs_path,const std::string& mime_file_path);

    ~static_content();

    void ctx_register(diy::Context* ctx);

private:

    std::string htdocs_;
    std::string mime_;
};




} // end namespace mol
 
#endif

