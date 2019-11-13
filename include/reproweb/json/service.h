#ifndef MOL_DEF_GUARD_DEFINE_REPROWEB_JSON_SERVICE_H_DEF_GUARD
#define MOL_DEF_GUARD_DEFINE_REPROWEB_JSON_SERVICE_H_DEF_GUARD

#include "reproweb/json/json.h"
#include "reprocurl/api.h"

namespace reproweb {
namespace JSON {


class RestEx : public repro::ReproEx<RestEx>	
{
public:

	RestEx() : status(500), json(Json::nullValue) {};
	RestEx(long s) : status(s),json(Json::nullValue)  {};
	RestEx(long s,const std::string& b) : repro::ReproEx<RestEx>(b),status(s),json(Json::nullValue)  {};
	RestEx(long s,Json::Value j) : status(s),json(j)  {};

	long status;
	Json::Value json;
};


struct Rest
{
	Rest& insecure()
	{
		req_.insecure();
		return *this;
	}

	Rest& verbose()
	{
		req_.verbose();
		return *this;
	}

	static Rest url(const std::string& serviceUrl)
	{
		Rest r;
		r.req_.url(serviceUrl);

		return r;
	}

    template<class T, class ... Args>
	static Rest url(const std::string& serviceUrl, T t, Args ... args)
	{
        std::ostringstream oss;
        oss << serviceUrl;
        if( serviceUrl.empty() || serviceUrl.back() != '/')
        {
            oss << "/";
        }
        oss << t;
		return url(oss.str(),args ...);
	}


	repro::Future<Json::Value> get()
	{
		return invoke(req_);
	}

	repro::Future<> remove()
	{
		req_.method("DELETE");

		auto p = repro::promise<>();
				
		reprocurl::fetch(req_)
		.then([p](reprocurl::response res)
		{
			std::cout << res.status() << ":" << res.content() << std::endl;

			Json::Value json = parse(res);

			p.resolve();
		})
		.otherwise(prio::reject(p));

		return p.future();	
	}	

	template<class T>
	repro::Future<Json::Value> post(T& t)
	{
		req_.data( JSON::flatten(toJson(t))).method("POST");

		return invoke(req_);
	}

	template<class T>
	repro::Future<Json::Value> put(T& t)
	{
		req_.data( JSON::flatten(toJson(t))).method("PUT");

		return invoke(req_);
	}


private:

	reprocurl::request req_;

	static repro::Future<Json::Value> invoke(reprocurl::request& req)
	{
		auto p = repro::promise<Json::Value>();
				
		reprocurl::fetch(req)
		.then([p](reprocurl::response res)
		{
			std::cout << res.status() << ":" << res.content() << std::endl;

			Json::Value json = parse(res);

			p.resolve(json);
		})
		.otherwise(prio::reject(p));

		return p.future();			
	}

	static Json::Value parse(reprocurl::response& res)
	{
		Json::Value json;
		try {
			json = reproweb::JSON::parse(res.content());
		}
		catch(...)
		{
			json = Json::nullValue;
		}

		if(res.status() != 200)
		{
			if(!json.isNull())
			{
				throw RestEx(res.status(),json);	 
			}
			else
			{
				throw RestEx(res.status(),res.content());	
			}
		}		
		return json;
	}

};

}}


#endif
