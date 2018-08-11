#ifndef _MOL_DEF_GUARD_DEFINE_MOD_HTTP_SSI_REQUEST_HANDLER_DEF_GUARD_
#define _MOL_DEF_GUARD_DEFINE_MOD_HTTP_SSI_REQUEST_HANDLER_DEF_GUARD_

#include "reproweb/ctrl/front_controller.h"

//////////////////////////////////////////////////////////////

namespace reproweb  {



class SSIResolver : public std::enable_shared_from_this<SSIResolver>
{
public:

	static 	repro::Future<std::string> resolve( prio::Request& req, const std::string& tmpl);

    static std::string tmpl(Request& req, const std::string& htdocs);

private:

	std::shared_ptr<SSIResolver> self_;
	int cnt_ = 0;

	std::vector<std::string> includes_;
	std::vector<std::string> parts_;
	std::vector<std::string> resolves_;

	repro::Future<std::string> fetch( prio::Request& req, const std::string& tmpl);
	std::string combine();
	bool match(const std::string& tmpl);
};


class ssi_content
{
public:

    ssi_content(const std::string& htdocs_path,const std::string& filter);

    void ctx_register(diy::Context* ctx)
	{
        register_static_handler(ctx);
	}

private:

    std::string htdocs_;
    std::string filter_;

    void register_static_handler(diy::Context* ctx);

};


} // end namespace mol

#endif

