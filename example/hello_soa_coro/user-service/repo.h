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
		if(user.username().empty() || user.login().empty() || user.hash().empty())
		{
			throw BadRequestEx("username, login and password may not be empty");
		}

		cryptoneat::Password pass;
		std::string hash = pass.hash(user.hash());

		try
		{
			reprosqlite::Result r = co_await sqlite->query(
						"INSERT INTO users (username,login,pwd,avatar_url) VALUES ( ? , ? , ? , ? )",
						user.username(),user.login(),hash,user.avatar_url()
			);

			co_return;
		}
		catch(const std::exception& ex)
		{
			throw LoginAlreadyTakenEx("error.msg.login.already.taken");
		}
	}

	Future<User> get_user( const std::string& login )
	{
		try
		{
			reprosqlite::Result r = co_await sqlite->query(
					"SELECT username,login,pwd,avatar_url FROM users WHERE login = ? ;",
					login
			);

			if ( r.rows() < 1) 
			{
				throw UserNotFoundEx("error.msg.login.failed");
			}

			User result(
				r[0][0],
				r[0][1],
				r[0][2],
				r[0][3]
			);

			co_return result;

		}
		catch(const std::exception& ex)
		{
			throw UserNotFoundEx("error.msg.login.failed");
		}
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
