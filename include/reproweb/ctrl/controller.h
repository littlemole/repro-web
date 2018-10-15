#ifndef INCLUDE_PROMISE_WEB_CONTROLLER_H_
#define INCLUDE_PROMISE_WEB_CONTROLLER_H_

#include "reproweb/ctrl/front_controller.h"
#include "diycpp/ctx.h"
#include <future>
#include "reproweb/json/json.h"
#include "priohttp/queryparams.h"

namespace reproweb  {


#ifdef _RESUMABLE_FUNCTIONS_SUPPORTED

typedef ::repro::Future<> Async;

#endif

template <typename T>
class has_valid
{
    typedef char one;
    typedef long two;

    template <typename C> static one test( decltype(&C::validate) ) ;
    template <typename C> static two test(...);    

public:
    enum { value = sizeof(test<T>(0)) == sizeof(char) };
};

class call_valid
{
public:

    template <class T, typename std::enable_if<has_valid<T>::value>::type* = nullptr >
	static void invoke( T& t) 
	{
		t.validate();
	}

    template <class T , typename  std::enable_if<!has_valid<T>::value>::type* = nullptr >
	static void invoke( T& t) 
	{}
};


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

template<class C>
C& prepare_controller(prio::Request& req)
{
	auto ptr = req.attributes.attr<std::shared_ptr<diy::Context>>("ctx")->resolve<C>();
	req.attributes.set("controller", ptr);
	C& c = *ptr;		
	return c;
}

struct QueryParam
{
	QueryParam(const std::string& n)
		: name(n)
	{}

	std::string name;
	std::string value;
};

struct FormParam
{
	FormParam(const std::string& n)
		: name(n)
	{}

	std::string name;
	std::string value;
};


struct CookieValue
{
	FormParam(const std::string& n)
		: name(n)
	{}

	std::string  name;
	prio::Cookie value;
};

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

template<class T,class E = void>
class HandlerParam;

template<class T>
class HandlerParam<Entity<T>>
{
public:

	static Entity<T> get(prio::Request& req,  prio::Response& res)
	{
		Json::Value json = JSON::parse(req.body());

		Entity<T> t;
		fromJson(t.value,json);
		call_valid::invoke(t.value);

		return t;
	}
};

template<class T>
class HandlerParam<Form<T>>
{
public:

	static Form<T> get(prio::Request& req,  prio::Response& res)
	{
		prio::QueryParams qp(req.body());

		Form<T> t;
		fromParams(t.value,qp);
		call_valid::invoke(t.value);

		return t;
	}
};

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

template<>
class HandlerParam<prio::Request&>
{
public:

	static prio::Request& get(prio::Request& req,  prio::Response& res)
	{
		return req;
	}
};

template<>
class HandlerParam<prio::Response&>
{
public:

	static prio::Response& get(prio::Request& req,  prio::Response& res)
	{
		return res;
	}
};

template<>
class HandlerParam<prio::Cookies>
{
public:

	static prio::Cookies get(prio::Request& req,  prio::Response& res)
	{
		return req.path.cookies();
	}
};

template<class T>
class HandlerParam<T,typename std::enable_if<std::is_base_of<CookieValue,T>::value>::type>
{
public:

	static T get(prio::Request& req,  prio::Response& res)
	{
		if( !req.attributes.exists("__cookies"))
		{
			prio::Cookies cookies = req.path.queryParams();
			req.attributes.set("__cookies",cookies);
		}
		
		T param;
		param.value = req.attributes.attr<prio::Cookies >("__cookies").get(param.name);
		return param;
	}
};


#define COOKIE_DEF(name,val) 				\
struct name : public reproweb::CookieValue 	\
{ 											\
	name()									\
		: reproweb::CookieValue(val)		\
	{}										\
};


#define COOKIE(name) COOKIE_DEF(name,#name)	



template<>
class HandlerParam<prio::QueryParams>
{
public:

	static prio::QueryParams get(prio::Request& req,  prio::Response& res)
	{
		return req.path.queryParams();
	}
};


template<class T>
class HandlerParam<T,typename std::enable_if<std::is_base_of<QueryParam,T>::value>::type>
{
public:

	static T get(prio::Request& req,  prio::Response& res)
	{
		if( !req.attributes.exists("__queryparams"))
		{
			prio::QueryParams qp = req.path.queryParams();
			req.attributes.set("__queryparams",qp);
		}
		
		T param;
		param.value = req.attributes.attr<prio::QueryParams>("__queryparams").get(param.name);
		return param;
	}
};

#define QUERY_PARAM_DEF(name,val) 			\
struct name : public reproweb::QueryParam 	\
{ 											\
	name()									\
		: reproweb::QueryParam(val)			\
	{}										\
};


#define QUERY_PARAM(name) QUERY_PARAM_DEF(name,#name)					


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


template<class T>
class HandlerParam<T,typename std::enable_if<std::is_base_of<FormParam,T>::value>::type>
{
public:

	static T get(prio::Request& req,  prio::Response& res)
	{
		if( !req.attributes.exists("__formparams"))
		{
			FormParams fp(req.body());
			req.attributes.set("__formparams",fp);
		}
		
		T param;
		param.value = req.attributes.attr<FormParams>("__formparams").get(param.name);
		return param;
	}
};

#define FORM_PARAM_DEF(name,val) 			\
struct name : public reproweb::FormParam 	\
{ 											\
	name()									\
		: reproweb::FormParam(val)			\
	{}										\
};


#define FORM_PARAM(name) FORM_PARAM_DEF(name,#name)					


template<class T>
class HandlerInvoker;

#ifdef _RESUMABLE_FUNCTIONS_SUPPORTED

template<class T>
class HandlerInvokerAsync;


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


template<class F>
class router
{
public:

	router(const std::string& m, const std::string& p, F f  )
		: method(m), path(p), handler(f)
	{}

	std::string method;
	std::string path;
	F handler;

	void ctx_register(diy::Context* ctx)
	{
		auto fc = ctx->resolve<FrontController>();
		registerController(*fc,method,path,handler);
	}

private:

	template<class C>
	void registerController(FrontController& fc, const std::string& m, const std::string& p, void (C::*fun)(prio::Request&, prio::Response&))
	{
		fc.registerHandler(m,p, [fun,&fc]( prio::Request& req,  prio::Response& res)
		{
			try 
			{	
				C& c = prepare_controller<C>(req);

				(c.*fun)(req,res);
			}
			catch(std::exception& ex)
			{
				fc.handle_exception(ex, req, res);
			}
		});
	}


	template<class C,class ... Args>
	void registerController(FrontController& fc, const std::string& m, const std::string& p, void (C::*fun)(Args...) )
	{
		fc.registerHandler(m,p, [&fc,fun]( prio::Request& req,  prio::Response& res)
		{			
			invoke_handler<C,Args...>(fc,req,res,fun);
		});
	}

#ifdef _RESUMABLE_FUNCTIONS_SUPPORTED

	template<class C, class ... Args>
	void registerController(FrontController& fc, const std::string& m, const std::string& p, Async(C::*fun)(Args...))
	{
		fc.registerHandler(m, p, [fun,&fc](prio::Request& req, prio::Response& res)
		{
			invoke_coro_handler<C,Args...>(fc,req,res,fun);
		});
	}

	template<class T, class C, class ... Args>
	void registerController(FrontController& fc, const std::string& m, const std::string& p, repro::Future<T> (C::*fun)(Args ...))
	{
		fc.registerHandler(m, p, [fun,&fc](prio::Request& req, prio::Response& res)
		{
			invoke_coro_handler<T,C,Args...>(fc,req,res,fun);						
		});
	}
	
#else

	template<class T, class C, class ...Args>
	void registerController(FrontController& fc, const std::string& m, const std::string& p, repro::Future<T> (C::*fun)(Args...))
	{
		fc.registerHandler(m,p, [&fc,fun]( prio::Request& req,  prio::Response& res)
		{
			invoke_handler<T,C,Args...>(fc,req,res,fun);
		});
	}
#endif

};

template<class F>
router<F> route(const std::string& m, const std::string& p, F fun )
{
	return router<F>(m,p,fun);
}


template<class F>
auto GET(const std::string& p, F f)
{
	return route("GET",p,f);
}

template<class F>
auto POST(const std::string& p, F f)
{
	return route("POST",p,f);
}

template<class F>
auto DEL(const std::string& p, F f)
{
	return route("DELETE",p,f);
}

template<class F>
auto PUT(const std::string& p, F f)
{
	return route("PUT",p,f);
}


template<class F>
struct filter_router
{
	filter_router(const std::string& m, const std::string& p, F f, int prio = 0 )
		: method(m), path(p), handler(f),priority(prio)
	{}

	std::string method;
	std::string path;
	int priority;
	F handler;

	void ctx_register(diy::Context* ctx)
	{
		auto fc = ctx->resolve<FrontController>();
		registerFilter(*fc,method,path,handler,priority);
	}

private:

	template<class C>
	void registerFilter(FrontController& fc, const std::string& m, const std::string& p, void (C::*fun)( prio::Request&,  prio::Response&, std::shared_ptr<FilterChain> chain), int prio)
	{
		fc.registerFilter(m,p, [fun]( prio::Request& req,  prio::Response& res, std::shared_ptr<FilterChain> chain)
		{
			auto ptr = req.attributes.attr<std::shared_ptr<diy::Context>>("ctx")->resolve<C>();
			(*ptr.*fun)(req,res,chain);
		},
		prio);
	}

	template<class C>
	void registerFilter(FrontController& fc, const std::string& m, const std::string& p, repro::Future<> (C::*fun)(prio::Request&, prio::Response&, std::shared_ptr<FilterChain> chain), int prio)
	{
		fc.registerFilter(m, p, [fun](prio::Request& req, prio::Response& res, std::shared_ptr<FilterChain> chain)
		{
			auto ptr = req.attributes.attr<std::shared_ptr<diy::Context>>("ctx")->resolve<C>();
			(*ptr.*fun)(req, res, chain);
		},
		prio);
	}
};

template<class F>
filter_router<F> filter(const std::string& m, const std::string& p, F f, int priority = 0 )
{
	return filter_router<F>(m,p,f,priority);
}

template<class F>
struct completion_filter_router
{
	completion_filter_router(const std::string& m, const std::string& p, F f, int prio = 0 )
		: method(m), path(p), handler(f),priority(prio)
	{}

	std::string method;
	std::string path;
	int priority;
	F handler;

	void ctx_register(diy::Context* ctx)
	{
		auto fc = ctx->resolve<FrontController>();
		registerFilter(*fc,method,path,handler,priority);
	}

private:

	template<class C>
	void registerFilter(FrontController& fc, const std::string& m, const std::string& p, void (C::*fun)( prio::Request&,  prio::Response&, std::shared_ptr<FilterChain> chain), int prio)
	{
		fc.registerCompletionFilter(m,p, [fun]( prio::Request& req,  prio::Response& res, std::shared_ptr<FilterChain> chain)
		{
			auto ptr = req.attributes.attr<std::shared_ptr<diy::Context>>("ctx")->resolve<C>();
			(*ptr.*fun)(req,res,chain);
		},
		prio);
	}

	template<class C>
	void registerCompletionFilter(FrontController& fc, const std::string& m, const std::string& p, repro::Future<> (C::*fun)(prio::Request&, prio::Response&, std::shared_ptr<FilterChain> chain), int prio)
	{
		fc.registerFilter(m, p, [fun](prio::Request& req, prio::Response& res, std::shared_ptr<FilterChain> chain)
		{
			auto ptr = req.attributes.attr<std::shared_ptr<diy::Context>>("ctx")->resolve<C>();
			(*ptr.*fun)(req, res, chain);
		},
		prio);
	}
};

template<class F>
completion_filter_router<F> completion_filter(const std::string& m, const std::string& p, F f, int priority = 0 )
{
	return completion_filter_router<F>(m,p,f,priority);
}



template<class F>
class exception_handler
{
public:

	exception_handler(F f)
		: fun_(f)
	{}

	void ctx_register(diy::Context* ctx)
	{
		auto fc = ctx->resolve<FrontController>();
		registerExHandler(*fc,fun_);
	}

private:

	F fun_;

	template<class C,class E>
	void registerExHandler(FrontController& fc, void (C::*fun)( const E& ex, prio::Request&,  prio::Response&))
	{
		fc.registerExceptionHandler<E>( [fun]( const E& ex, prio::Request& req,  prio::Response& res)
		{
			auto ptr = req.attributes.attr<std::shared_ptr<diy::Context>>("ctx")->resolve<C>();
			(*ptr.*fun)(ex,req,res);
		});
	}


	template<class C, class E>
	void registerExHandler(FrontController& fc, repro::Future<> (C::*fun)(const E& ex, prio::Request&, prio::Response&))
	{
		fc.registerExceptionHandler<E>([fun](const E& ex, prio::Request& req, prio::Response& res)
		{
			auto ptr = req.attributes.attr<std::shared_ptr<diy::Context>>("ctx")->resolve<C>();
			(*ptr.*fun)(ex, req, res);
		});
	}
};

template<class F>
exception_handler<F> ex_handler(F fun)
{
	return exception_handler<F>(fun);
}


class WebApplicationContext : public diy::Context
{
public:

	template<class ... Args>
	WebApplicationContext(Args&& ... args)
		: diy::Context(nullptr)
	{
		// make Context itself injectable
		std::shared_ptr<diy::Context> ctx = std::shared_ptr<diy::Context>( 
			(diy::Context*)this, 
			[](diy::Context* c) {}
		);

		registerFactory(
			std::type_index(typeid(diy::Context)), 
			new diy::FactoryImpl<diy::Context>(ctx)
		);

		// register a FrontController
		registerFactory(
			std::type_index(typeid(FrontController)), 
			new diy::FactoryImpl<FrontController>(
				diy::constructor<FrontController(diy::Context)>()
			) 
		);

		register_dependencies<Args&&...>(std::forward<Args&&>(args)...);
	}

private:

	template<class ... Args>
	void register_dependencies()
	{}

	template<class T, class ... Args>
	void register_dependencies(T&& t,Args&& ... args)
	{
		//std::cout << "register " << typeid(t).name() << std::endl;
		t.ctx_register(this);
		register_dependencies<Args&&...>(std::forward<Args&&>(args)...);
	}

	WebApplicationContext(const WebApplicationContext& rhs) = delete;
};

}

#endif
