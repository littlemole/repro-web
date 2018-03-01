#ifndef _MOL_DEF_GUARD_DEFINE_MOD_HTTP_REQUEST_HANDLER_FILTER_DEF_GUARD_
#define _MOL_DEF_GUARD_DEFINE_MOD_HTTP_REQUEST_HANDLER_FILTER_DEF_GUARD_

#include "reproweb/ctrl/handler_info.h"

namespace reproweb  {

 
//////////////////////////////////////////////////////////////

class FilterChain : public std::enable_shared_from_this<FilterChain>
{
public:

	typedef std::shared_ptr<FilterChain> Ptr;

	FilterChain() = delete;
	FilterChain(const FilterChain&) = delete;

	static Ptr create(std::vector<HttpFilterInfo*> chain,HttpFilterInfo* terminator)
	{
		return Ptr(new FilterChain(chain,terminator) );
	}

    ~FilterChain()
    {}
    
    void next(prio::Request& req, prio::Response& res)
    {
        it_++;
        if ( it_ == chain_.end() )
        {
            return;
        }
        (*it_)->filter()(req,res,shared_from_this());
    }
    
    void filter( prio::Request& req, prio::Response& res )
    {
        (*it_)->filter()(req,res,shared_from_this());
    }
    
private:

    FilterChain( std::vector<HttpFilterInfo*> chain, HttpFilterInfo* terminator )
        : chain_(chain), terminator_(terminator)
    {
        it_ = chain_.begin();
    }

    std::vector<HttpFilterInfo*>::iterator it_;
    std::vector<HttpFilterInfo*> chain_;
    std::unique_ptr<HttpFilterInfo> terminator_;
};

//////////////////////////////////////////////////////////////

class FilterChainBuilder
{
public:

    FilterChainBuilder()
    {}
    
    FilterChainBuilder& add( HttpFilterInfo* filter )
    {
        chain_.push_back(filter);
        return *this;
    }
    
    FilterChain::Ptr build(HttpFilterInfo* filter)
    {
        sort();
        chain_.push_back(filter);
        return FilterChain::create(chain_,filter);
    }
    
private:    

    
    FilterChainBuilder& sort()
    {
        std::sort( 
            chain_.begin(), 
            chain_.end(), 
            [](HttpFilterInfo* lhs, HttpFilterInfo* rhs) {
                return lhs->priority() > rhs->priority();
            });
        return *this;
    }
    
    std::vector<HttpFilterInfo*> chain_;
};


} // end namespace mol

#endif

