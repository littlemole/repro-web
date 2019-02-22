#ifndef _MOL_DEF_GUARD_DEFINE_REPROWEB_SERIALIZER_SQL_DEF_GUARD_
#define _MOL_DEF_GUARD_DEFINE_REPROWEB_SERIALIZER_SQL_DEF_GUARD_

#include <repromysql/mysql-bindings.h>
#include <repromysql/mysql-async.h>
#include <reproweb/serialization/meta.h>
//#include <reproweb/serialization/parameter.h>
//#include "reproweb/ctrl/front_controller.h"

namespace reproweb {


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

inline void fromSQL( const char* name, repromysql::result_async::Ptr r, std::string& t)
{
    t = r->field(std::string(name)).getString(); 
}

inline void fromSQL( const char* name, repromysql::result_async::Ptr r, int& t)
{
    t = r->field(std::string(name)).getInt();
}

inline void fromSQL( const char* name, repromysql::result_async::Ptr r, double& t)
{
    t = r->field(std::string(name)).getDouble();
}

inline void fromSQL( const char* name, repromysql::result_async::Ptr r, float& t)
{
    t = r->field(std::string(name)).getFloat();
}

inline void fromSQL( const char* name, repromysql::result_async::Ptr r, bool& t)
{
    t = r->field(std::string(name)).getInt() != 0;
}

inline void fromSQL( const char* name, repromysql::result_async::Ptr r, long long& t)
{
    t = r->field(std::string(name)).getLongLong();
}

inline void fromSQL( const char* name, repromysql::result_async::Ptr r, char& t)
{
    t = r->field(std::string(name)).getInt();
}


template<class T>
void fromSQL(repromysql::result_async::Ptr r, T& t);

template<class T>
void fromSQL(repromysql::result_async::Ptr r, std::vector<T>& v);


///////////////////////////////////////////////////////////////////////////////////////////

class SQLSerializer
{
public:

    template<class T>
    static void deserialize( const char* name, const void* from, T& to) 
    {
        repromysql::result_async::Ptr r = *((repromysql::result_async::Ptr*)from);

        try {
            fromSQL( name, r, to);
        }
        catch(...)
        {
            // memmber might not be in result set so I will throw
        }
    }
};

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


template<class T>
void fromSQL(repromysql::result_async::Ptr r, T& t)
{
    if( r->fetch() )
    {
        const auto& m = meta_of(t);

	    m. template deserialize<SQLSerializer>(&r,t);
    }
    else
    {
        throw repro::Ex("empty result");
    }    
}

template<class T>
void fromSQL(repromysql::result_async::Ptr r, std::vector<T>& v)
{
	v.clear();
    while( r->fetch() )
    {
        T t;
        const auto& m = meta_of(t);

	    m. template deserialize<SQLSerializer>(&r,t);
        v.push_back(std::move(t));
    }
}

///////////////////////////////////////////////////////////////////////////////////////////

}

#endif

