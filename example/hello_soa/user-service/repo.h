#ifndef _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_REPO_DEFINE_
#define _DEF_GUARD_DEFINE_REPROWEB_HELLO_WORLD_REPO_DEFINE_

#include "reprosqlite/sqlite.h"
#include "entities.h"

using namespace prio;
using namespace repro;
using namespace reproweb;


class UserRepository
{
public:

	UserRepository(std::shared_ptr<reprosqlite::SqlitePool> sqlitePool)
		: sqlite(sqlitePool)
	{}

	Future<> register_user( User user )
	{
		auto p = promise<>();

		if(user.username().empty() || user.login().empty() || user.hash().empty())
		{
			throw BadRequestEx("username, login and password may not be empty");
		}

		cryptoneat::Password pass;
		std::string hash = pass.hash(user.hash());

		sqlite->query(
					"INSERT INTO users (username,login,pwd,avatar_url) VALUES ( ? , ? , ? , ? )",
					user.username(),user.login(),hash,user.avatar_url()
		)
		.then([p,user](reprosqlite::Result r)
		{
			p.resolve();
		})
		.otherwise([p](const std::exception& ex)
		{
			p.reject(LoginAlreadyTakenEx("username already taken"));
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
			if ( r.rows() < 1) 
			{
				throw UserNotFoundEx("user not found");
			}

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
			p.reject(UserNotFoundEx("login not found"));
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
