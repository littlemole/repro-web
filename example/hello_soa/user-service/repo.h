#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_REPO_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_REPO_DEFINE_

#include "reprosqlite/sqlite.h"
#include "model.h"

using namespace prio;
using namespace repro;
using namespace reproweb;


class UserRepository
{
public:

	UserRepository(std::shared_ptr<reprosqlite::SqlitePool> sqlitePool)
		: sqlite(sqlitePool)
	{}

	Future<User> register_user( User & user )
	{
		auto p = promise<User>();

		if(user.username().empty() || user.login().empty() || user.hash().empty())
		{
			nextTick( [p]() 
			{
				p.reject(RegistrationEx("username, login and password may not be empty"));
			});

			return p.future();
		}

		cryptoneat::Password pass;
		std::string hash = pass.hash(user.hash());

		auto result = std::make_shared<User>(user);

		sqlite->query(
					"INSERT INTO users (username,login,pwd,avatar_url) VALUES ( ? , ? , ? , ? )",
					user.username(),user.login(),user.hash(),user.avatar_url()
		)
		.then([p,result](reprosqlite::Result r)
		{
			p.resolve(*result);
		})
		.otherwise([p](const std::exception& ex)
		{
			p.reject(RegistrationEx("error.msg.login.alreaady.taken"));
		});

		return p.future();
	}

	Future<User> get_user( const std::string& login )
	{
		auto p = promise<User>();

		sqlite->query(
				"SELECT username,login,pwd,avatar_url FROM users WHERE login = ? ;",
				login
		)
		.then([p](reprosqlite::Result r)
		{
			if ( r.rows() < 1) throw repro::Ex("user not found");

			User result(
				r[0][0],
				r[0][1],
				r[0][2],
				r[0][3]
			);
			p.resolve(result);
		})
		.otherwise([p](const std::exception& ex)
		{
			p.reject(LoginEx("error.msg.login.failed"));
		});

		return p.future();
	}

private:

	std::shared_ptr<reprosqlite::SqlitePool> sqlite;
};

struct UserPool : public reprosqlite::SqlitePool
{
	UserPool(std::shared_ptr<Config> config) 
	  : SqlitePool(config->getString("sqlite")) 
	{}
};

#endif
