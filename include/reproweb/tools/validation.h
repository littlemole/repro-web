#ifndef INCLUDE_REPROWEB_VALIDATION_TPL_H_
#define INCLUDE_REPROWEB_VALIDATION_TPL_H_

#include "priohttp/request.h"

namespace reproweb {



class ValidationEx : public repro::ReproEx<ValidationEx> 
{
public:
	ValidationEx() {}
	ValidationEx(const std::string& s) : repro::ReproEx<ValidationEx> (s) {}
};




template<class E>
class validator
{
public:

	validator(const std::regex & r)
		: r_(r)
	{}

	std::string getString(const std::string& tainted, const std::string& msg)
	{
		std::smatch match;
		 if ( !std::regex_match(tainted,match,r_) )
		 {
			 throw E(msg);
		 }

		 return match[0];
	}

	int getInteger(const std::string& tainted, const std::string& msg)
	{
		std::smatch match;
		if ( !std::regex_match(tainted,match,r_) )
		{
			throw E(msg);
		}

		std::istringstream iss(match[0]);
		int i;
		iss >> i;		 
		return i;
	}

	double getDouble(const std::string& tainted, const std::string& msg)
	{
		std::smatch match;
		if ( !std::regex_match(tainted,match,r_) )
		{
			throw E(msg);
		}

		std::istringstream iss(match[0]);
		double d;
		iss >> d;		 
		return d;
	}


private:
	std::regex r_;	
};


template<class E>
inline std::string valid(const std::string& s, const std::regex& r, const std::string& msg)
{
	validator<E> v(r);
	return v.getString(s,msg);
}

template<class E>
inline std::string valid(const std::string& s, const std::regex& r)
{
	static const std::string msg("string validation failed!");
	return valid<E>(s,r,msg);
}

inline std::string valid(const std::string& s, const std::regex& r)
{
	return valid<ValidationEx>(s,r);
}

inline std::string valid(const std::string& s, const std::regex& r, const std::string& msg)
{
	return valid<ValidationEx>(s,r,msg);
}

template<class E>
std::vector<std::string>& valid( std::vector<std::string>& v, const std::regex& r, const std::string& msg)
{
	for ( auto& i : v)
	{
		valid<E>(i,r,msg);
	}
	return v;
}

inline std::vector<std::string>& valid( std::vector<std::string>& v, const std::regex& r, const std::string& msg)
{
	for ( auto& i : v)
	{
		valid<ValidationEx>(i,r,msg);
	}
	return v;
}


template<class E>
inline int valid_int(const std::string& s,const std::string& msg)
{
	validator<E> v(std::regex("[0-9]*"));
	return v.getInteger(s,msg);
}


template<class E>
inline int valid_int(const std::string& s)
{
	static const std::string msg("integer validation failed!");
	return valid_int<E>(s,msg);
}

inline int valid_int(const std::string& s)
{
	return valid_int<ValidationEx>(s);
}

inline int valid_int(const std::string& s,const std::string& msg)
{
	return valid_int<ValidationEx>(s,msg);
}

template<class E>
inline double valid_double(const std::string& s,const std::string& msg)
{
	validator<E> v(std::regex("[0-9\\.]*"));
	return v.getDouble(s,msg);
}


template<class E>
inline double valid_double(const std::string& s)
{
	static const std::string msg("double validation failed!");
	return valid_double<E>(s,msg);
}


inline double valid_double(const std::string& s,const std::string& msg)
{
	return valid_double<ValidationEx>(s,msg);
}


inline double valid_double(const std::string& s)
{
	return valid_double<ValidationEx>(s);
}

template<class E>
std::string valid_email(const std::string& email, const std::string& msg)
{
	validator<E> v( std::regex("^[a-zA-Z0-9.!#$%&’*+/=?^_`{|}~-]+@[a-zA-Z0-9-]+(?:\\.[a-zA-Z0-9-]+)*$"));
	return v.getString(email,msg);
}

template<class E>
std::string valid_email(const std::string& email)
{
	return valid_email(email,"invalid email");
}

inline std::string valid_email(const std::string& email, const std::string& msg)
{
	return valid_email<ValidationEx>(email,msg);
}


inline std::string valid_email(const std::string& email)
{
	return valid_email<ValidationEx>(email);
}

template<class E>
std::string valid_url(const std::string& url, const std::string& msg)
{
	validator<E> v( std::regex("(http|https)://(\\w+:{0,1}\\w*@)?(\\S+)(:[0-9]+)?(/|/([\\w#!:.?+=&%@!-/]))?"));
	return v.getString(url,msg);
}

template<class E>
std::string valid_url(const std::string& url)
{
	return valid_email<E>(url,"invalid url");
}

inline std::string valid_url(const std::string& url, const std::string& msg)
{
	return valid_email<ValidationEx>(url,msg);
}

inline std::string valid_url(const std::string& url)
{
	return valid_email<ValidationEx>(url);
}

} // end namespace


#endif
