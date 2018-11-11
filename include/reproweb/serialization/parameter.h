#ifndef INCLUDE_PROMISE_WEB_CONTROLLER_XXX_PARAMETER_H_
#define INCLUDE_PROMISE_WEB_CONTROLLER_XXX_PARAMETER_H_

#include "reproweb/ctrl/front_controller.h"
//#include "diycpp/ctx.h"
//#include "reproweb/tools/serializer.h"
#include <priohttp/response.h>
#include "priohttp/multipart.h"
#include "reproweb/traits.h"

namespace reproweb  {



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


template<class C, class ... Args>
void invoke_handler(FrontController& fc, prio::Request& req,  prio::Response& res, repro::Future<std::string> (C::*fun)(Args...) )
{
	try
	{
		HandlerInvoker<std::string(C,Args...)>::invoke(req,res,fun)
		.then([&res](std::string r)
		{
			if(res.headers.content_type().empty())
			{
				res.contentType("test/html");
			}
			res.body(r).ok().flush();
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

template<class C, class ... Args>
Async invoke_coro_handler(FrontController& fc, prio::Request& req,  prio::Response& res, repro::Future<std::string> (C::*fun)(Args...) )
{
	try
	{
		std::string r = co_await HandlerInvoker<std::string(C,Args...)>::invoke(req,res,fun);

		if(res.headers.content_type().empty())
		{
			res.contentType("test/html");
		}
		res.body(r).ok().flush();
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
