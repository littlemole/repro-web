# repro-web
reactive non blocling c++ web development with promises. coroutines optional.

# motivation
study to explore modern c++' fitness for modern, server side web development.

# requirements
- nonblocking io
- single thread model
- no callback hell
- support for HTTP,HTTPS,HTTP2
- declarative http routing
- http filters
- support for JSON
- support for templating
- Dependency Injection

# dependencies
- [repro](http://github.com/littlemole/repro) exposes the fundamental Promise abstraction to wrap async io
- [prio](http://github.com/littlemole/prio) gives basic asio primitives using promises, backed by either [libevent](http://libevent.org/) or [boost-asio](https://www.boost.org/doc/libs/1_66_0/doc/html/boost_asio.html)
- [priohttp](http://github.com/littlemole/priohttp) adds support for HTTP, HTTPS, HTTP2, further backed by [zlib](https://zlib.net/) and [libnghttp2](https://nghttp2.org/)
- [cryptoneat](http://github.com/littlemole/cryptoneat) adds support for basic crypto backed by [OpenSSL](https://www.openssl.org/)
- [diy](http://github.com/littlemole/diy) dependency injection for c++

# middleware
- [repro-curl](http://github.com/littlemole/repro-curl) for making HTTP client calls serverside
- [repro-redis](http://github.com/littlemole/repro-redis) access to Redis key-value store
- [repro-mysql](http://github.com/littlemole/repro-mysql) access MySQL databases 
- [repro-sqlite](http://github.com/littlemole/repro-sqlite) access SQlite databases (mainly intended for dev and prototyping)

# hello world

main.cpp
```c++
#include "reproweb/ctrl/controller.h"
#include "reproweb/web_webserver.h"
#include "reproweb/view/tpl.h"

using namespace prio;
using namespace reproweb;

class Controller
{
public:
    Controller()
    {
        templates_.load("/view/");
    }

    void index( Request& req, Response& res)
    {
        Json::Value viewModel(Json::objectValue);
        viewModel["greeting"] = "Hello World";
        
        res
        .body(templates_.render("index", viewModel ))
        .contentType("text/html")
        .ok()
        .flush();
    }

private:
    reproweb::TplStore templates_;
};

int main(int argc, char** argv)
{
    init();

    WebApplicationContext ctx 
    {
        GET( "/", &Controller::index),

        singleton<Controller()>()
    };	

    WebServer server(ctx);
    server.listen(9876);
        
    theLoop().run();

    return 0;
}
``` 

index.tpl:
```html
<body>
<head></head>
<body>
<h1>{{greeting}}</h1>
</body>
</html>
```
compile with
```bash
g++ main.cpp -std=c++14 $(pkg-config --libs --cflags reproweb jsoncpp libnghttp2  openssl zlib libevent_pthreads)
``` 
see examples/readme/ for code.

# going async

now extend the example by simulating making an async service call using libcurl. 
actually we will just make an http call to our own webserver that
servers a static json file at /api/service.json.

```c++
...

class Model 
{
public:

    Model()
        :service_endpoint("http://localhost:9876/api/service.json")
    {}

    Future<Json::Value> fetch_greeting()
    {
        auto p = promise<Json::Value>();

        request req(service_endpoint);
        
        fetch(req)
        .then([p](response res)
        {
            p.resolve(JSON::parse(res.content()));
        })
        .otherwise(reject(p));

        return p.future();
    }

private:
    Url service_endpoint;
};

class Controller
{
public:

    Controller(std::shared_ptr<Model> m, std::shared_ptr<View> v)
        : model(m), view(v)
    {}

    void index( Request& req, Response& res)
    {
        model->fetch_greeting()
        .then([this,&res](Json::Value greeting)
        {
            view->render_greeting(res,greeting);
        })
        .otherwise([this,&res](const std::exception& ex)
        {
            view->render_error(res,ex.what());
        });        
    }

private:
    std::shared_ptr<Model> model;
    std::shared_ptr<View> view;
};


int main(int argc, char** argv)
{
    ...

    WebApplicationContext ctx 
    {
        GET( "/", &Controller::index),

        static_content("/htdocs/","/etc/mime.types"),

        diy::singleton<Model()>(),
        diy::singleton<View()>(),
        diy::singleton<Controller(Model,View)>()
    };

    ...
}

``` 

compile with
```bash
g++ main.cpp -std=c++14 $(pkg-config --libs --cflags reproweb reprocurl jsoncpp libnghttp2  openssl zlib libevent_pthreads libcurl)
``` 

full source in examples/mvc/main.cpp

# coroutines

using a compiler with support for coroutines TS, this becomes:

```c++
...

class Model 
{
public:

    Model()
        :service_endpoint("http://localhost:9876/api/service.json")
    {}

    Future<Json::Value> fetch_greeting()
    {
        request req(service_endpoint);
        
        response res = co_await fetch(req);

        co_return JSON::parse(res.content());
    }

private:
    Url service_endpoint;
};

class Controller
{
public:

    Controller(std::shared_ptr<Model> m, std::shared_ptr<View> v)
        : model(m), view(v)
    {}

    Async index( Request& req, Response& res)
    {
        try
        {
            Json::Value greeting = co_await model->fetch_greeting();
            view->render_greeting(res,greeting);
        }
        catch(const std::exception& ex)
        {        
            view->render_error(res,ex.what());
        }        
    }

private:
    std::shared_ptr<Model> model;
    std::shared_ptr<View> view;
};

...

``` 

[TBD]