#ifndef _MOL_DEF_GUARD_DEFINE_REPROWEB_SERIALIZER_SQL_DEF_GUARD_
#define _MOL_DEF_GUARD_DEFINE_REPROWEB_SERIALIZER_SQL_DEF_GUARD_

#include <repromysql/mysql-bindings.h>
#include <repromysql/mysql-async.h>
#include <reproweb/serialization/meta.h>

namespace reproweb {


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

inline void fromSQL( const char* name, repromysql::result_async::Ptr r, std::string& t)
{
    const repromysql::Retval& rv = r->field(std::string(name));
    if( !rv.null() )
    {
        t = r->field(std::string(name)).getString(); 
    }
}

inline void fromSQL( const char* name, repromysql::result_async::Ptr r, int& t)
{
    const repromysql::Retval& rv = r->field(std::string(name));
    if( !rv.null() )
    {
        t = r->field(std::string(name)).getInt(); 
    }
}

inline void fromSQL( const char* name, repromysql::result_async::Ptr r, double& t)
{
    const repromysql::Retval& rv = r->field(std::string(name));
    if( !rv.null() )
    {
        t = r->field(std::string(name)).getDouble(); 
    }
}

inline void fromSQL( const char* name, repromysql::result_async::Ptr r, float& t)
{
    const repromysql::Retval& rv = r->field(std::string(name));
    if( !rv.null() )
    {
        t = r->field(std::string(name)).getFloat(); 
    }    
}

inline void fromSQL( const char* name, repromysql::result_async::Ptr r, bool& t)
{
    const repromysql::Retval& rv = r->field(std::string(name));
    if( !rv.null() )
    {
        t = r->field(std::string(name)).getInt() != 0; 
    }    
}

inline void fromSQL( const char* name, repromysql::result_async::Ptr r, long long& t)
{
    const repromysql::Retval& rv = r->field(std::string(name));
    if( !rv.null() )
    {
        t = r->field(std::string(name)).getLongLong(); 
    }    
}

inline void fromSQL( const char* name, repromysql::result_async::Ptr r, char& t)
{
    const repromysql::Retval& rv = r->field(std::string(name));
    if( !rv.null() )
    {
        t = r->field(std::string(name)).getInt(); 
    }
}

template<class T>
void fromSQL(const char* name, repromysql::result_async::Ptr r, std::vector<T>& t)
{
    // do nothing
}

template<class T>
void fromSQL(repromysql::result_async::Ptr r, T& t);

template<class T>
void fromSQL(repromysql::result_async::Ptr r, std::vector<T>& v);


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


template<class T>
void fromSQL(repromysql::result_async::Ptr r, T& t)
{
    if( r->fetch() )
    {
        const auto& m = meta_of(t);

        auto visitor = [&r, &t]( const char* name, auto& m)
        {	
            std::remove_reference_t<typename std::remove_reference_t<decltype(m)>::value_t> value;
            try 
            {
                fromSQL(name,r,value);
                m.set(t,value);
            }
            catch(...)
            {
                // memmber might not be in result set so I will throw
            }            
        };
        m.visit(t,visitor);
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

        auto visitor = [&r, &t]( const char* name, auto& m)
        {	
            std::remove_reference_t<typename std::remove_reference_t<decltype(m)>::value_t> value;
            try 
            {
                fromSQL(name,r,value);
                m.set(t,value);
            }
            catch(...)
            {
                // memmber might not be in result set so I will throw
            }                
        };
        m.visit(t,visitor);        

        v.push_back(std::move(t));
    }
}

///////////////////////////////////////////////////////////////////////////////////////////
/*
inline void bondage(repromysql::statement_async::Ptr st, int i, std::string t)
{
	st->bind(i,t);
}

template<class T>
void bondage(repromysql::statement_async::Ptr st, int i, T t, typename std::enable_if<std::is_arithmetic<T>::value>::type = nullptr)
{
	st->bind(i,t);
}

template<class T>
void bondage(repromysql::statement_async::Ptr st, int i, T t, typename std::enable_if<std::is_class<T>::value>::type = nullptr)
{}

template<class T>
void bondage(repromysql::statement_async::Ptr st, int i,std::vector<T>& v)
{}

struct named_args_statement
{
    named_args_statement(const std::string& query)
    {
        std::ostringstream oss;
        bool inside_quote = false;

        char last = 0;
        size_t pos = 0;
        while( pos < query.size() )
        {
            char c = query[pos];
            if ( c == '\'' && last != '\\')
            {
                if(inside_quote) 
                    inside_quote = false;
                else
                    inside_quote = true;
            } 

            if(!inside_quote)
            {
                if ( c == ':' )
                {
                    size_t p = query.find_first_of(" \t\r\n",pos);
                    if( p != std::string::npos)
                    {
                        std::string named_arg = query.substr(pos+1,p-pos-1);
                        named_args.push_back(named_arg);
                        oss << "? ";
                        pos += p-pos;
                        last = ' ';
                        continue;
                    }
                    else
                    {
                        std::string named_arg = query.substr(pos+1);
                        named_args.push_back(named_arg);
                        oss << "? ";
                        pos += named_arg.size()+1;
                        last = ' ';
                        continue;
                    }
                }
                else
                {
                    oss << c;
                }
            }
            else
            {
                oss << c;
            }
            pos++;
            last = c;
        }

        sql = oss.str();
    }

    std::vector<std::string> named_args;
    std::string sql;
};

template<class T>
repro::Future<repromysql::result_async::Ptr> objQuery( std::shared_ptr<repromysql::mysql_async> mysql, T from, const std::string& sql)
{
    named_args_statement prepared_sql(sql);

    std::string psql = prepared_sql.sql;
    std::vector<std::string> named_args = prepared_sql.named_args;

    return mysql->prepare(psql)
    .then([named_args,from](repromysql::statement_async::Ptr st)
    {
        const auto& m = meta_of(from);

        for( unsigned int i = 0; i < named_args.size(); i++)
        {
            m.value(from,named_args[i].c_str(), [st,i]( auto v)
            {
                bondage(st,i,v);
            });
        }
        return st->query();
    });
}


template<class T>
repro::Future<repromysql::mysql_async::Ptr> persist( std::shared_ptr<repromysql::mysql_async> mysql, T from, const std::string& sql)
{
    named_args_statement prepared_sql(sql);

    std::string psql = prepared_sql.sql;
    std::vector<std::string> named_args = prepared_sql.named_args;

    return mysql->prepare(psql)
    .then([named_args,from](repromysql::statement_async::Ptr st)
    {
        const auto& m = meta_of(from);

        for( unsigned int i = 0; i < named_args.size(); i++)
        {
            m.value(from,named_args[i].c_str(), [st,i]( auto v)
            {
                bondage(st,i,v);
            });
        }
        return st->execute();
    });
}
*/

}

#endif

