#ifndef INCLUDE_PROMISE_WEB_CONTROLLER_PARAMETER_H_
#define INCLUDE_PROMISE_WEB_CONTROLLER_PARAMETER_H_

#include "reproweb/ctrl/front_controller.h"
#include "diycpp/ctx.h"
#include "reproweb/tools/serializer.h"
#include "priohttp/multipart.h"

namespace reproweb  {


//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

inline void output_json(prio::Response& res,Json::Value json)
{
	res
	.body(JSON::flatten(json))
	.contentType("application/json")
	.ok()
	.flush();
}

template<class T>
void output_json(prio::Response& res, T& t)
{
	output_json(res, toJson(t) );
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

template<class C>
C& prepare_controller(prio::Request& req)
{
	auto ptr = req.attributes.attr<std::shared_ptr<diy::Context>>("ctx")->resolve<C>();
	req.attributes.set("controller", ptr);
	C& c = *ptr;		
	return c;
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

template <typename T>
void validate( T& t)
{
     call_valid::invoke(t);
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

template<class T>
struct Entity
{
	T value;

	T* operator->()
	{
		return &value;
	}

	T& operator*()
	{
		return value;
	}
};

//////////////////////////////////////////////////////////////

template<class T>
struct Form
{
	T value;

	T* operator->()
	{
		return &value;
	}

	T& operator*()
	{
		return value;
	}
};

//////////////////////////////////////////////////////////////

template<class T>
struct Parameter
{
	T value;

	T* operator->()
	{
		return &value;
	}

	T& operator*()
	{
		return value;
	}
};

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

template<class T,class E = void>
class HandlerParam;

//////////////////////////////////////////////////////////////

template<class T>
class HandlerParam<Entity<T>>
{
public:

	static Entity<T> get(prio::Request& req,  prio::Response& res)
	{
		Json::Value json = JSON::parse(req.body());

		Entity<T> t;
		fromJson(t.value,json);
		validate(t.value);

		return t;
	}
};

//////////////////////////////////////////////////////////////

template<class T>
class HandlerParam<Parameter<T>>
{
public:

	static Parameter<T> get(prio::Request& req,  prio::Response& res)
	{
		Parameter<T> t;
		fromRequest(t.value,req);
		validate(t.value);

		return t;
	}
};

//////////////////////////////////////////////////////////////

template<class T>
class HandlerParam<Form<T>>
{
public:

	static Form<T> get(prio::Request& req,  prio::Response& res)
	{
		prio::QueryParams qp(req.body());

		Form<T> t;
		fromParams(t.value,qp);
		validate(t.value);

		return t;
	}
};

//////////////////////////////////////////////////////////////

template<>
class HandlerParam<Json::Value>
{
public:

	static Json::Value get(prio::Request& req,  prio::Response& res)
	{
		Json::Value json = JSON::parse(req.body());

		return json;
	}
};

//////////////////////////////////////////////////////////////

template<>
class HandlerParam<prio::Request&>
{
public:

	static prio::Request& get(prio::Request& req,  prio::Response& res)
	{
		return req;
	}
};

//////////////////////////////////////////////////////////////

template<>
class HandlerParam<prio::Response&>
{
public:

	static prio::Response& get(prio::Request& req,  prio::Response& res)
	{
		return res;
	}
};

//////////////////////////////////////////////////////////////

template<>
class HandlerParam<prio::Cookies>
{
public:

	static prio::Cookies get(prio::Request& req,  prio::Response& res)
	{
		return req.headers.cookies();
	}
};

//////////////////////////////////////////////////////////////

template<>
class HandlerParam<prio::MultiParts>
{
public:

	static prio::MultiParts get(prio::Request& req,  prio::Response& res)
	{
		std::string delim = req.headers.values("Content-Type").value().params()["boundary"];

		// unquote
		if(delim.size() > 1 && delim[0] == '"'  && delim[delim.size()-1] == '"' ) delim = delim.substr(1,delim.size()-2);
		if(delim.size() > 1 && delim[0] == '\'' && delim[delim.size()-1] == '\'') delim = delim.substr(1,delim.size()-2);

		return prio::MultiParts(req.body(),delim);
	}
};
	

//////////////////////////////////////////////////////////////

template<>
class HandlerParam<prio::QueryParams>
{
public:

	static prio::QueryParams get(prio::Request& req,  prio::Response& res)
	{
		return req.path.queryParams();
	}
};
				
//////////////////////////////////////////////////////////////

class FormParams : public prio::QueryParams
{
public:
	FormParams() {};
	FormParams(const std::string& s) : prio::QueryParams(s) {}
};


template<>
class HandlerParam<FormParams>
{
public:

	static FormParams get(prio::Request& req,  prio::Response& res)
	{
		FormParams fp(req.body());
		return fp;
	}
};
				
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

template<class T>
class HandlerInvoker;

//////////////////////////////////////////////////////////////

#ifdef _RESUMABLE_FUNCTIONS_SUPPORTED

template<class T>
class HandlerInvokerAsync;


//////////////////////////////////////////////////////////////

template<class C>
class HandlerInvokerAsync<void(C)>
{
public:

	template<class F, class ... VArgs>
	static Async invoke( prio::Request& req,  prio::Response& res, F fun, VArgs&& ... vargs)
	{
		C& c = prepare_controller<C>(req);
		return (c.*fun)(std::forward<VArgs&&>(vargs)...);
	}
};

//////////////////////////////////////////////////////////////

template<class C, class T, class ... Args>
class HandlerInvokerAsync<void(C,T,Args...)>
{
public:

	template<class F, class ... VArgs>
	static Async invoke(prio::Request& req,  prio::Response& res, F fun, VArgs&& ... vargs)
	{
		return HandlerInvokerAsync<void(C,Args...)>::invoke(req,res,fun, std::forward<VArgs&&>(vargs)..., HandlerParam<T>::get(req,res));
	}
};

#endif

//////////////////////////////////////////////////////////////

template<class C>
class HandlerInvoker<void(C)>
{
public:

	template<class F, class ... VArgs>
	static void invoke( prio::Request& req,  prio::Response& res, F fun, VArgs&& ... vargs)
	{
		C& c = prepare_controller<C>(req);
		(c.*fun)(std::forward<VArgs&&>(vargs)...);
	}
};

//////////////////////////////////////////////////////////////

template<class R,class C>
class HandlerInvoker<R(C)>
{
public:

	template<class F, class ... VArgs>
	static repro::Future<R> invoke( prio::Request& req,  prio::Response& res, F fun, VArgs&& ... vargs)
	{
		C& c = prepare_controller<C>(req);
		return (c.*fun)(std::forward<VArgs&&>(vargs)...);
	}
};

//////////////////////////////////////////////////////////////

template<class C, class T, class ... Args>
class HandlerInvoker<void(C,T,Args...)>
{
public:

	template<class F, class ... VArgs>
	static void invoke(prio::Request& req,  prio::Response& res, F fun, VArgs&& ... vargs)
	{
		HandlerInvoker<void(C,Args...)>::invoke(req,res,fun, std::forward<VArgs&&>(vargs)..., HandlerParam<T>::get(req,res));
	}
};

//////////////////////////////////////////////////////////////

template<class R, class C, class T, class ... Args>
class HandlerInvoker<R(C,T,Args...)>
{
public:

	template<class F, class ... VArgs>
	static repro::Future<R> invoke(prio::Request& req,  prio::Response& res, F fun, VArgs&& ... vargs)
	{
		return HandlerInvoker<R(C,Args...)>::invoke(req,res,fun, std::forward<VArgs&&>(vargs)..., HandlerParam<T>::get(req,res));
	}
};

//////////////////////////////////////////////////////////////

template<class C, class ... Args>
void invoke_handler(FrontController& fc, prio::Request& req,  prio::Response& res, void (C::*fun)(Args...) )
{
	try
	{
		HandlerInvoker<void(C,Args...)>::invoke(req,res,fun);		
	}
	catch(std::exception& ex)
	{
		fc.handle_exception(ex, req, res);
	}
}

//////////////////////////////////////////////////////////////

#ifdef _RESUMABLE_FUNCTIONS_SUPPORTED

template<class C, class ... Args>
Async invoke_coro_handler(FrontController& fc, prio::Request& req,  prio::Response& res, Async (C::*fun)(Args...) )
{
	try
	{
		co_await HandlerInvokerAsync<void(C,Args...)>::invoke(req,res,fun);		
	}
	catch(std::exception& ex)
	{
		fc.handle_exception(ex, req, res);
	}

	co_await prio::nextTick();
	co_return;
}

#endif

//////////////////////////////////////////////////////////////

template<class R,class C, class ... Args>
void invoke_handler(FrontController& fc, prio::Request& req,  prio::Response& res, repro::Future<R> (C::*fun)(Args...) )
{
	try
	{
		HandlerInvoker<R(C,Args...)>::invoke(req,res,fun)
		.then([&res](R r)
		{
			output_json(res,r);
		})
		.otherwise([&fc,&req,&res](const std::exception& ex)
		{
			fc.handle_exception(ex, req, res);
		});		
	}
	catch(std::exception& ex)
	{
		fc.handle_exception(ex, req, res);
	}
}

//////////////////////////////////////////////////////////////

#ifdef _RESUMABLE_FUNCTIONS_SUPPORTED

template<class R,class C, class ... Args>
Async invoke_coro_handler(FrontController& fc, prio::Request& req,  prio::Response& res, repro::Future<R> (C::*fun)(Args...) )
{
	try
	{
		R r = co_await HandlerInvoker<R(C,Args...)>::invoke(req,res,fun);

		output_json(res,r);
	}
	catch(std::exception& ex)
	{
		fc.handle_exception(ex, req, res);
	}

	co_await prio::nextTick();
	co_return;
}
 
#endif

//////////////////////////////////////////////////////////////


}

#endif
