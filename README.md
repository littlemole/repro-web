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

dependencies
- repro exposes the fundamental Promise abstraction to wrap async io
- prio gives basic asio primitives using promises, backed by either libevent or boost-asio
- priohttp adds support for HTTP, HTTPS, HTTP2, further backed by zlib and libnghttp2
- cryptoneat adds support for basic crypto backed by OpenSSL
- diy dependency injection for c++

middleware
- repro-curl for making HTTP client calls serverside
- repro-redis access to Redis key-value store
- repro-mysql access MySQL databases 
- repro-sqlite access SQlite databases (mainly intended for dev and prototyping)

hello world

```c++
#include "reproweb/ctrl/controller.h"
#include "reproweb/web_webserver.h"

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
    prio::init();

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
with this template index.tpl:
```html
<body>
<head></head>
<body>
<h1>{{greeting}}</h1>
</body>
</html>
```

[TBD]