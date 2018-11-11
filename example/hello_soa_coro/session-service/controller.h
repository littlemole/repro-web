#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_CONTROLLER_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_CONTROLLER_DEFINE_

#include "repo.h"
#include "cryptoneat/cryptoneat.h"

using namespace reproweb;

class Controller
{
public:

	Controller( std::shared_ptr<SessionRepository> repo)
		: sessionRepository(repo)
	{}

	Future<Json::Value> get_session( Parameter<SessionId> params, Request& req, Response& res)
	{
		//std::string sid = Valid::session_id(req.path.args().get("sid"));

		Session session = co_await sessionRepository->get_user_session(params->sid);

		co_return session.profile();
	}

	Future<Json::Value> write_session( json_t<User> user, Request& req, Response& res)
	{
		Session session = co_await sessionRepository->write_user_session(*user);

		co_return toJson(session);
	}	

	reproweb::Async remove_session( Parameter<SessionId> params,Request& req, Response& res)
	{
		//std::string sid = Valid::session_id(req.path.args().get("sid"));

		co_await sessionRepository->remove_user_session(params->sid);

		res.ok().flush();

		co_return;
	}

private:

	std::shared_ptr<SessionRepository> sessionRepository;
};


class Exceptions
{
public:

	Exceptions()
	{}

	void on_no_session_ex(const NoSessionEx& ex,Request& req, Response& res)
	{
		std::cout << typeid(ex).name() << ":" << ex.what() << std::endl;

		Json::Value json = exToJson(ex);

		res
		.not_found()
		.body(JSON::flatten(json))
		.contentType("application/json")
		.flush();
	}	

	void on_std_ex(const std::exception& ex,Request& req, Response& res)
	{
		std::cout << typeid(ex).name() << ":" << ex.what() << std::endl;

		Json::Value json = exToJson(ex);

		res
		.error()
		.body(JSON::flatten(json))
		.contentType("application/json")
		.flush();
	}		
};


#endif