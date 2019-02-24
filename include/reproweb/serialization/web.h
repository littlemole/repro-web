#ifndef _MOL_DEF_GUARD_DEFINE_REPROWEB_SERIALIZER_WEB_DEF_GUARD_
#define _MOL_DEF_GUARD_DEFINE_REPROWEB_SERIALIZER_WEB_DEF_GUARD_


#include <reproweb/serialization/meta.h>
#include <reproweb/serialization/parameter.h>


namespace reproweb {

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

inline void fromParams(const std::string& from, int& to)
{
	std::istringstream iss(from);
	iss >> to;
}

inline void fromParams(const std::string& from, std::string& to)
{
	to = from;
}

inline void fromParams(const std::string& from, prio::HeaderValues& to)
{

}

inline void fromParams(const std::string& from, prio::Cookie& to)
{

}

inline void fromParams( const prio::Cookie& from, prio::Cookie& to )
{
	to = from;
}

inline void fromParams( const prio::Cookie& from, int& to )
{
	to = std::stoul(from.value());	
}

inline void fromParams( const prio::Cookie& from, std::string& to )
{
	to = from.value();
}



inline void fromParams( const prio::Cookie& from, prio::HeaderValues& to )
{
	// no op
}


inline void fromParams( const prio::HeaderValues& from, std::string& to )
{
	to = from.value().main();
}


inline void fromParams( const prio::HeaderValues& from, prio::HeaderValues& to )
{
	to = from;
}


inline void fromParams( const prio::HeaderValues& from, int& to )
{
	to = std::stoul(from.value().main());
}


inline void fromParams( const prio::HeaderValues& from, prio::Cookie& to )
{
	// no op
}


template<class T>
void fromParams( prio::QueryParams& qp, T& t);

template<class T>
void fromParams( prio::QueryParams& qp, std::vector<T>& t);

class ParamsSerializer
{
public:

    template<class T>
    static void deserialize( const char* name, const void* from, T& to) 
    {
				prio::QueryParams& qp = *( (prio::QueryParams*)from);

				std::string val = qp.get(name);

				fromParams(val,to);
    }
    
    template<class T>
    static void deserialize( const char* name, const void* from, std::vector<T>& to) 
    {
				prio::QueryParams& qp = *( (prio::QueryParams*)from);

				to.clear();
				std::string val = qp.get(name);
				auto v = prio::split(val,",");
				for ( auto& i : v)
				{
					T t;
					fromParams(i,t);
					to.push_back(std::move(t));
				}
    }
};

template<class T>
void fromParams( prio::QueryParams& qp, T& t)
{
	const auto& m = meta_of(t);

    m. template deserialize<ParamsSerializer>(&qp,t);
}

template<class T>
void fromParams( prio::QueryParams& qp, std::vector<T>& t)
{
	const auto& m = meta_of(t);

    m. template deserialize<ParamsSerializer>(&qp,t);
}


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

template<class T>
void fromRequest( prio::Request& req, T& t);

class RequestSerializer
{
public:


    template<class T>
    static void deserialize( const char* member, const void* from, T& to) 
    {
		prio::Request& req = *( (prio::Request*)from);

		// path param

		prio::Args args = getPathArgs(req);
		if ( args.exists(member) )
		{
			reproweb::fromParams( args.get(member), to );
			return;
		}

		// query param
		prio::QueryParams qp = getQueryParams(req);
		if (qp.exists(member))
		{
			reproweb::fromParams( qp.get(member), to );
			return;
		}

		// cookie value

		const prio::Cookies& c = req.headers.cookies();

		if(c.exists(member))
		{
			reproweb::fromParams( c.get(member), to );
			return;
		}

		// header

		if(req.headers.exists(member))
		{
			reproweb::fromParams( req.headers.values(member), to );
			return;
		}
    }
    

	static prio::QueryParams getQueryParams(prio::Request& req )
	{
		static const char* key = "__queryparams";
		if ( !req.attributes.exists(key) )
		{
			req.attributes.set(key, req.path.queryParams());
		}

		return req.attributes.attr<prio::QueryParams>(key);
	}

	static prio::Args getPathArgs(prio::Request& req )
	{
		static const char* key = "__pathargs";
		if ( !req.attributes.exists(key) )
		{
			req.attributes.set(key, req.path.args() );
		}

		return req.attributes.attr<prio::Args>(key);
	}		
};

template<class T>
void fromRequest( prio::Request& req, T& t)
{
    const auto& m = meta_of(t);

    m. template deserialize<RequestSerializer>(&req,t);
}

//////////////////////////////////////////////////////////////

template<class T>
class HandlerParam<Parameter<T>>
{
public:

	static Parameter<T> get(prio::Request& req,  prio::Response& res)
	{
		Parameter<T> t;
		fromRequest(req,t.value);
		validate(t.value);

		return t;
	}
};

//////////////////////////////////////////////////////////////

template<class T>
class HandlerParam<Form<T>>
{
public:

	static Form<T> get(prio::Request& req,  prio::Response& res)
	{
		prio::QueryParams qp(req.body());

		Form<T> t;
		fromParams(qp,t.value);
		validate(t.value);

		return t;
	}
};



//////////////////////////////////////////////////////////////

template<>
class HandlerParam<prio::Request&>
{
public:

	static prio::Request& get(prio::Request& req,  prio::Response& res)
	{
		return req;
	}
};

//////////////////////////////////////////////////////////////

template<>
class HandlerParam<prio::Response&>
{
public:

	static prio::Response& get(prio::Request& req,  prio::Response& res)
	{
		return res;
	}
};

//////////////////////////////////////////////////////////////

template<>
class HandlerParam<prio::Cookies>
{
public:

	static prio::Cookies get(prio::Request& req,  prio::Response& res)
	{
		return req.headers.cookies();
	}
};

//////////////////////////////////////////////////////////////

template<>
class HandlerParam<prio::MultiParts>
{
public:

	static prio::MultiParts get(prio::Request& req,  prio::Response& res)
	{
		std::string delim = req.headers.values("Content-Type").value().params()["boundary"];

		// unquote
		if(delim.size() > 1 && delim[0] == '"'  && delim[delim.size()-1] == '"' ) delim = delim.substr(1,delim.size()-2);
		if(delim.size() > 1 && delim[0] == '\'' && delim[delim.size()-1] == '\'') delim = delim.substr(1,delim.size()-2);

		return prio::MultiParts(req.body(),delim);
	}
};
	

//////////////////////////////////////////////////////////////

template<>
class HandlerParam<prio::QueryParams>
{
public:

	static prio::QueryParams get(prio::Request& req,  prio::Response& res)
	{
		return req.path.queryParams();
	}
};
				
//////////////////////////////////////////////////////////////

class FormParams : public prio::QueryParams
{
public:
	FormParams() {};
	FormParams(const std::string& s) : prio::QueryParams(s) {}
};


template<>
class HandlerParam<FormParams>
{
public:

	static FormParams get(prio::Request& req,  prio::Response& res)
	{
		FormParams fp(req.body());
		return fp;
	}
};




///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

}

#endif

