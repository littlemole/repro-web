#ifndef INCLUDE_PROMISE_WEB_CONTROLLER_H_
#define INCLUDE_PROMISE_WEB_CONTROLLER_H_

#include "reproweb/ctrl/front_controller.h"
#include "diycpp/ctx.h"
#include <future>

namespace reproweb  {


template<class F>
struct router
{
	router(const std::string& m, const std::string& p, F f )
		: method(m), path(p), handler(f)
	{}

	std::string method;
	std::string path;
	F handler;
};

template<class F>
router<F> route(const std::string& m, const std::string& p, F f)
{
	return router<F>(m,p,f);
}


template<class F>
router<F> GET(const std::string& p, F f)
{
	return router<F>("GET",p,f);
}

template<class F>
router<F> POST(const std::string& p, F f)
{
	return router<F>("POST",p,f);
}

template<class F>
router<F> DEL(const std::string& p, F f)
{
	return router<F>("DELETE",p,f);
}

template<class F>
router<F> PUT(const std::string& p, F f)
{
	return router<F>("PUT",p,f);
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
};

template<class F>
filter_router<F> filter(const std::string& m, const std::string& p, F f, int priority = 0 )
{
	return filter_router<F>(m,p,f,priority);
}

#ifdef _RESUMABLE_FUNCTIONS_SUPPORTED

typedef ::repro::Future<> Async;

#endif

class http_controller
{
public:

	template<class ... VArgs>
	http_controller(VArgs ... vargs)
		: frontController_(frontController())
	{
		registerHandlers(vargs...);
	}


	template<class ... VArgs>
	http_controller(FrontController& c, VArgs ... vargs)
		: frontController_(c)
	{
		registerHandlers(vargs...);
	}

private:

	FrontController& frontController_;

	template<class F, class ... VArgs>
	void registerHandlers(router<F> r, VArgs ... vargs)
	{
		registerController(r.method, r.path, r.handler);
		registerHandlers(vargs...);
	}

	void registerHandlers()
	{
		// terminator, do nothing
	}


	template<class C>
	void registerController(const std::string& m, const std::string& p, void (C::*fun)(prio::Request&, prio::Response&))
	{
		frontController_.registerHandler(m,p, [fun]( prio::Request& req,  prio::Response& res)
		{
			auto ptr = req.attributes.attr<std::shared_ptr<diy::Context>>("ctx")->resolve<C>();
			req.attributes.set("controller", ptr);
			C& c = *ptr;
			(c.*fun)(req,res);
		});
	}

#ifdef _RESUMABLE_FUNCTIONS_SUPPORTED

	template<class C>
	void registerController(const std::string& m, const std::string& p, Async(C::*fun)(prio::Request&, prio::Response&))
	{
		FrontController& fc = frontController_;
		fc.registerHandler(m, p, [fun,&fc](prio::Request& req, prio::Response& res)
		{
			auto ptr = req.attributes.attr<std::shared_ptr<diy::Context>>("ctx")->resolve<C>();
			req.attributes.set("controller", ptr);
			C& c = *ptr;
			coro_handler(fc,c, fun, req, res);
		});
	}
#endif
};

#ifdef _RESUMABLE_FUNCTIONS_SUPPORTED

template<class T, class C>
Future<> coro_handler(FrontController& fc, T& t, Async(C::*fun)(prio::Request&, prio::Response&), prio::Request& req, prio::Response& res)
{
	try
	{
		co_await(t.*fun)(req, res);
	}
	catch (const std::exception& ex)
	{
		fc.handle_exception(ex, req, res);
	}
}

#endif

class http_filter
{
public:

	template<class ... VArgs>
	http_filter(VArgs ... vargs)
		: frontController_(frontController())
	{
		registerHandlers(vargs...);
	}

	template<class ... VArgs>
	http_filter(FrontController& fc, VArgs ... vargs)
		: frontController_(fc)
	{
		registerHandlers(vargs...);
	}

private:

	FrontController& frontController_;

	template<class F, class ... VArgs>
	void registerHandlers(filter_router<F> r, VArgs ... vargs)
	{
		registerFilter(r.method, r.path, r.handler, r.priority);
		registerHandlers(vargs...);
	}

	void registerHandlers()
	{
		// terminator, do nothing
	}


	template<class C>
	void registerFilter(const std::string& m, const std::string& p, void (C::*fun)( prio::Request&,  prio::Response&, std::shared_ptr<FilterChain> chain), int priority)
	{
		frontController_.registerFilter(m,p, [fun]( prio::Request& req,  prio::Response& res, std::shared_ptr<FilterChain> chain)
		{
			auto ptr = req.attributes.attr<std::shared_ptr<diy::Context>>("ctx")->resolve<C>();
			(*ptr.*fun)(req,res,chain);
		},
		priority);
	}

	template<class C>
	void registerFilter(const std::string& m, const std::string& p, repro::Future<> (C::*fun)(prio::Request&, prio::Response&, std::shared_ptr<FilterChain> chain), int priority)
	{
		frontController_.registerFilter(m, p, [fun](prio::Request& req, prio::Response& res, std::shared_ptr<FilterChain> chain)
		{
			auto ptr = req.attributes.attr<std::shared_ptr<diy::Context>>("ctx")->resolve<C>();
			(*ptr.*fun)(req, res, chain);
		},
		priority);
	}
};


//template<class T>
class http_completion_filter
{
public:

	template<class ... VArgs>
	http_completion_filter(VArgs ... vargs)
		:frontController_(frontController())
	{
		registerHandlers(vargs...);
	}

	template<class ... VArgs>
	http_completion_filter(FrontController& fc, VArgs ... vargs)
		: frontController_(fc)
	{
		registerHandlers(vargs...);
	}

private:

	FrontController& frontController_;

	template<class F, class ... VArgs>
	void registerHandlers(filter_router<F> r, VArgs ... vargs)
	{
		registerFilter(r.method, r.path, r.handler, r.priority);
		registerHandlers(vargs...);
	}

	void registerHandlers()
	{
		// terminator, do nothing
	}


	template<class C>
	void registerFilter(const std::string& m, const std::string& p, void (C::*fun)( prio::Request&,  prio::Response&, std::shared_ptr<FilterChain> chain), int priority)
	{
		frontController_.registerCompletionFilter(m,p, [fun]( prio::Request& req,  prio::Response& res, std::shared_ptr<FilterChain> chain)
		{
			auto ptr = req.attributes.attr<std::shared_ptr<diy::Context>>("ctx")->resolve<C>();
			(*ptr.*fun)(req,res,chain);
		},
		priority);
	}


	template<class C>
	void registerFilter(const std::string& m, const std::string& p, repro::Future<> (C::*fun)(prio::Request&, prio::Response&, std::shared_ptr<FilterChain> chain), int priority)
	{
		frontController_.registerCompletionFilter(m, p, [fun](prio::Request& req, prio::Response& res, std::shared_ptr<FilterChain> chain)
		{
			auto ptr = req.attributes.attr<std::shared_ptr<diy::Context>>("ctx")->resolve<C>();
			(*ptr.*fun)(req, res, chain);
		},
		priority);
	}
};



template<class T>
class exception_handler
{
public:

	template<class ... VArgs>
	exception_handler(VArgs ... vargs)
		: frontController_(frontController())
	{
		registerHandlers(vargs...);
	}

	template<class ... VArgs>
	exception_handler(FrontController& fc, VArgs ... vargs)
		: frontController_(fc)
	{
		registerHandlers(vargs...);
	}

private:

	FrontController& frontController_;

	template<class F, class ... VArgs>
	void registerHandlers(F f, VArgs ... vargs)
	{
		registerExHandler(f);
		registerHandlers(vargs...);
	}

	void registerHandlers()
	{
		// terminator, do nothing
	}


	template<class C,class E>
	void registerExHandler(void (C::*fun)( const E& ex, prio::Request&,  prio::Response&))
	{
		frontController_.registerExceptionHandler<E>( [fun]( const E& ex, prio::Request& req,  prio::Response& res)
		{
			auto ptr = req.attributes.attr<std::shared_ptr<diy::Context>>("ctx")->resolve<C>();
			(*ptr.*fun)(ex,req,res);
		});
	}


	template<class C, class E>
	void registerExHandler(repro::Future<> (C::*fun)(const E& ex, prio::Request&, prio::Response&))
	{
		frontController_.registerExceptionHandler<E>([fun](const E& ex, prio::Request& req, prio::Response& res)
		{
			auto ptr = req.attributes.attr<std::shared_ptr<diy::Context>>("ctx")->resolve<C>();
			(*ptr.*fun)(ex, req, res);
		});
	}
};



template<class C, class E>
exception_handler<C> ex_handler(void (C::*fun)( const E& ex, prio::Request&,  prio::Response&) )
{
	return ex_handler(fun,frontController());
}


template<class C, class E>
exception_handler<C> ex_handler(repro::Future<> (C::*fun)(const E& ex, prio::Request&, prio::Response&) )
{
	return ex_handler(fun, frontController());
}


template<class C, class E>
exception_handler<C> ex_handler(void (C::*fun)(const E& ex, prio::Request&, prio::Response&), FrontController& fc )
{
	return exception_handler<C>(fc, fun);
}


template<class C, class E>
exception_handler<C> ex_handler(repro::Future<>(C::*fun)(const E& ex, prio::Request&, prio::Response&), FrontController& fc )
{
	return exception_handler<C>(fc, fun);
}


}

#endif
