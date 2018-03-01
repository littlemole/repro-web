#ifndef _MOL_DEF_GUARD_DEFINE_MOD_HTTP_REQUEST_CRYPT_JWT_DEF_GUARD_
#define _MOL_DEF_GUARD_DEFINE_MOD_HTTP_REQUEST_CRYPT_JWT_DEF_GUARD_

#include "priohttp/common.h"
#include "priohttp/response.h"
#include "reproweb/json/json.h"
#include "cryptoneat/cryptoneat.h"

namespace reproweb  {


class JWT
{
public:
    JWT();
    JWT(const std::string& token);
    JWT(const Json::Value& c);    
    JWT(const Json::Value& h,const Json::Value& c);
    ~JWT();

    bool parse(const std::string& token);

    std::string sign(const std::string& secret,size_t expires_s);
    bool verify(const std::string& secret);

    std::string sign(cryptoneat::EVP_PKEY* privkey,size_t expires_s);
    bool verify( cryptoneat::EVP_PKEY* pubkey);

    JWT& header(const Json::Value& h);
    JWT& claim(const Json::Value& c);
    
    Json::Value header();
    Json::Value claim();

    std::string signature();

private:

    void expire(size_t expires_s);
    std::string payload();
    std::string token(const std::string& body);
    size_t inspect(const std::string& token);

    Json::Value header_;
    Json::Value claim_;
    std::string signature_;
    std::string payload_;
};

namespace http {

	class Request;
	class Response;
}

void json_response(prio::Response& res, Json::Value json);

auto inline json_err(prio::Response& res)
{
    return  [&res] ( const std::exception& ex)
    {
        Json::Value r(Json::objectValue);
        r["error"] = ex.what();

        res
            .body(JSON::stringify(r))
            .contentType("application/json")
            .error()
            .flush();
    };
}

} // end namespace csgi

#endif

