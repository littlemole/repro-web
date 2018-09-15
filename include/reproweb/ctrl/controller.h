#ifndef INCLUDE_PROMISE_WEB_CONTROLLER_H_
#define INCLUDE_PROMISE_WEB_CONTROLLER_H_

#include "reproweb/ctrl/front_controller.h"
#include "diycpp/ctx.h"
#include <future>
#include "reproweb/json/json.h"

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

#ifdef _RESUMABLE_FUNCTIONS_SUPPORTED

template<class T, class C>
repro::Future<> coro_handler(FrontController& fc, T& t, Async(C::*fun)(prio::Request&, prio::Response&), prio::Request& req, prio::Response& res)
{
	try
	{
		co_await(t.*fun)(req, res);
	}
	catch (const std::exception& ex)
	{
		fc.handle_exception(ex, req, res);
	}
	co_await prio::nextTick();
	co_return;
}

template<class T, class V, class C>
repro::Future<> coro_handler_output(FrontController& fc, T& t, repro::Future<V> (C::*fun)(prio::Request&, prio::Response&), prio::Request& req, prio::Response& res)
{
	try
	{
		V v = co_await(t.*fun)(req, res);

		output_json(res,v);

		co_return;
	}
	catch (const std::exception& ex)
	{
		fc.handle_exception(ex, req, res);
	}
	co_await prio::nextTick();
	co_return;
}


template<class T, class C>
repro::Future<> coro_handler_output_json(FrontController& fc, T& t, repro::Future<Json::Value> (C::*fun)(prio::Request&, prio::Response&), prio::Request& req, prio::Response& res)
{
	try
	{
		Json::Value v = co_await(t.*fun)(req, res);

		output_json(res,v);

		co_return;
	}
	catch (const std::exception& ex)
	{
		fc.handle_exception(ex, req, res);
	}
	co_await prio::nextTick();
	co_return;
}


template<class T, class V, class C, class R>
repro::Future<> coro_handler_input_output(FrontController& fc, T& t, repro::Future<V> (C::*fun)(R r,prio::Request&, prio::Response&), prio::Request& req, prio::Response& res)
{
	try
	{
		Json::Value json = JSON::parse(req.body());

		R r;
		fromJson(r,json);

		call_valid::invoke(r);

		V v = co_await(t.*fun)(r, req, res);

		output_json(res,v);

		co_return;
	}
	catch (const std::exception& ex)
	{
		fc.handle_exception(ex, req, res);
	}
	co_await prio::nextTick();
	co_return;
}


template<class T, class C>
repro::Future<> coro_handler_input_output_json(FrontController& fc, T& t, repro::Future<Json::Value> (C::*fun)(Json::Value,prio::Request&, prio::Response&), prio::Request& req, prio::Response& res)
{
	try
	{
		Json::Value json = JSON::parse(req.body());

		Json::Value v = co_await(t.*fun)(json, req, res);

		output_json(res,v);

		co_return;
	}
	catch (const std::exception& ex)
	{
		fc.handle_exception(ex, req, res);
	}
	co_await prio::nextTick();
	co_return;
}

template<class T,class C, class R>
repro::Future<> coro_handler_input_void(FrontController& fc, T& t, Async (C::*fun)(R r,prio::Request&, prio::Response&), prio::Request& req, prio::Response& res)
{
	try
	{
		Json::Value json = JSON::parse(req.body());

		R r;
		fromJson(r,json);
		call_valid::invoke(r);

		co_await(t.*fun)(r, req, res);

		co_return;
	}
	catch (const std::exception& ex)
	{
		fc.handle_exception(ex, req, res);
	}
	co_await prio::nextTick();
	co_return;
}

template<class T,class C>
repro::Future<> coro_handler_input_void_json(FrontController& fc, T& t, Async (C::*fun)(Json::Value,prio::Request&, prio::Response&), prio::Request& req, prio::Response& res)
{
	try
	{
		Json::Value json = JSON::parse(req.body());

		co_await(t.*fun)(json, req, res);

		co_return;
	}
	catch (const std::exception& ex)
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
		fc.registerHandler(m,p, [this,fun,&fc]( prio::Request& req,  prio::Response& res)
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


#ifdef _RESUMABLE_FUNCTIONS_SUPPORTED

	template<class C>
	void registerController(FrontController& fc, const std::string& m, const std::string& p, Async(C::*fun)(prio::Request&, prio::Response&))
	{
		fc.registerHandler(m, p, [this,fun,&fc](prio::Request& req, prio::Response& res)
		{
			C& c = prepare_controller<C>(req);

			coro_handler(fc,c, fun, req, res)
			.then([](){})
			.otherwise([](const std::exception&ex){});
		});
	}

	template<class T, class C>
	void registerController(FrontController& fc, const std::string& m, const std::string& p, repro::Future<T> (C::*fun)(prio::Request&, prio::Response&))
	{
		fc.registerHandler(m, p, [this,fun,&fc](prio::Request& req, prio::Response& res)
		{
			C& c = prepare_controller<C>(req);

			coro_handler_output(fc,c, fun, req, res)
			.then([](){})
			.otherwise([](const std::exception&ex){});
		});
	}	


	template<class C>
	void registerController(FrontController& fc, const std::string& m, const std::string& p, repro::Future<Json::Value> (C::*fun)(prio::Request&, prio::Response&))
	{
		fc.registerHandler(m, p, [this,fun,&fc](prio::Request& req, prio::Response& res)
		{
			C& c = prepare_controller<C>(req);

			coro_handler_output_json(fc,c, fun, req, res)
			.then([](){})
			.otherwise([](const std::exception&ex){});
		});
	}	

	template<class T, class C, class V>
	void registerController(FrontController& fc, const std::string& m, const std::string& p, repro::Future<T> (C::*fun)(V v,prio::Request&, prio::Response&))
	{
		fc.registerHandler(m, p, [this,fun,&fc](prio::Request& req, prio::Response& res)
		{
			C& c = prepare_controller<C>(req);

			coro_handler_input_output(fc,c, fun, req, res)
			.then([](){})
			.otherwise([](const std::exception&ex){});
		});
	}

	template<class C>
	void registerController(FrontController& fc, const std::string& m, const std::string& p, repro::Future<Json::Value> (C::*fun)(Json::Value,prio::Request&, prio::Response&))
	{
		fc.registerHandler(m, p, [this,fun,&fc](prio::Request& req, prio::Response& res)
		{
			C& c = prepare_controller<C>(req);

			coro_handler_input_output_json(fc,c, fun, req, res)
			.then([](){})
			.otherwise([](const std::exception&ex){});
		});
	}

	template<class C>
	void registerController(FrontController& fc, const std::string& m, const std::string& p, Async(C::*fun)(Json::Value,prio::Request&, prio::Response&))
	{
		fc.registerHandler(m, p, [this,fun,&fc](prio::Request& req, prio::Response& res)
		{
			C& c = prepare_controller<C>(req);

			coro_handler_input_void_json(fc,c, fun, req, res)
			.then([](){})
			.otherwise([](const std::exception&ex){});
		});
	}			
#else

	template<class T, class C>
	void registerController(FrontController& fc, const std::string& m, const std::string& p, repro::Future<T> (C::*fun)(prio::Request&, prio::Response&))
	{
		fc.registerHandler(m,p, [this,&fc,fun]( prio::Request& req,  prio::Response& res)
		{
			try
			{
				C& c = prepare_controller<C>(req);

				(c.*fun)(req,res)
				.then([&res](T t)
				{
					output_json(res,t);
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
		});
	}

	template<class C>
	void registerController(FrontController& fc, const std::string& m, const std::string& p, repro::Future<Json::Value> (C::*fun)(prio::Request&, prio::Response&))
	{
		fc.registerHandler(m,p, [this,&fc,fun]( prio::Request& req,  prio::Response& res)
		{
			try
			{
				C& c = prepare_controller<C>(req);

				(c.*fun)(req,res)
				.then([&res](Json::Value json)
				{
					output_json(res,json);
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
		});
	}

	template<class T, class C, class V>
	void registerController(FrontController& fc, const std::string& m, const std::string& p, repro::Future<T> (C::*fun)(V v, prio::Request&, prio::Response&))
	{
		fc.registerHandler(m,p, [this,&fc,fun]( prio::Request& req,  prio::Response& res)
		{
			try
			{
				Json::Value json = JSON::parse(req.body());
				V v;
				fromJson(v,json);
				call_valid::invoke(v);

				C& c = prepare_controller<C>(req);

				(c.*fun)(v,req,res)
				.then([&res](T t)
				{
					output_json(res,t);
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
		});
	}

	template<class C>
	void registerController(FrontController& fc, const std::string& m, const std::string& p, repro::Future<Json::Value> (C::*fun)(Json::Value, prio::Request&, prio::Response&))
	{
		fc.registerHandler(m,p, [this,&fc,fun]( prio::Request& req,  prio::Response& res)
		{
			try
			{
				Json::Value json = JSON::parse(req.body());

				C& c = prepare_controller<C>(req);

				(c.*fun)(json,req,res)
				.then([&res](Json::Value value)
				{
					output_json(res,value);
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
		});
	}

	template<class C,class V>
	void registerController(FrontController& fc, const std::string& m, const std::string& p, void (C::*fun)(V v, prio::Request&, prio::Response&))
	{
		fc.registerHandler(m,p, [this,&fc,fun]( prio::Request& req,  prio::Response& res)
		{
			try
			{
				Json::Value json = JSON::parse(req.body());
				V v;
				fromJson(v,json);
				call_valid::invoke(v);

				C& c = prepare_controller<C>(req);
				(c.*fun)(v,req,res);
			}
			catch(std::exception& ex)
			{
				fc.handle_exception(ex, req, res);
			}
		});
	}

	template<class C>
	void registerController(FrontController& fc, const std::string& m, const std::string& p, void (C::*fun)(Json::Value, prio::Request&, prio::Response&))
	{
		fc.registerHandler(m,p, [this,&fc,fun]( prio::Request& req,  prio::Response& res)
		{
			try
			{
				Json::Value json = JSON::parse(req.body());

				C& c = prepare_controller<C>(req);
				(c.*fun)(json,req,res);
			}
			catch(std::exception& ex)
			{
				fc.handle_exception(ex, req, res);
			}
		});
	}

#endif

	template<class C>
	C& prepare_controller(prio::Request& req)
	{
		auto ptr = req.attributes.attr<std::shared_ptr<diy::Context>>("ctx")->resolve<C>();
		req.attributes.set("controller", ptr);
		C& c = *ptr;		
		return c;
	}

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
