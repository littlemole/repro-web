#ifndef _MOL_DEF_GUARD_DEFINE_REPROWEB_SERIALIZER_META_DEF_GUARD_
#define _MOL_DEF_GUARD_DEFINE_REPROWEB_SERIALIZER_META_DEF_GUARD_

#include "reprocpp/ex.h"
#include "reproweb/traits.h"
#include <type_traits>


namespace reproweb {

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

template<class T, class M>
class Member
{
public:
    const char* name;
    M T::* member;
};

///////////////////////////////////////////////////////////////////////////////////////////

template<class T, class M>
Member<T,M> member( const char* n, M T::* m)
{
    return Member<T,M> {n,m};
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

template<class T, class M>
class GetterSetter
{
public:

    typedef M (T::*getter_t)();
    typedef void (T::*setter_t)(M);

    GetterSetter( const char* n, getter_t g, setter_t s)
        : name(n), getter(g), setter(s)
    {}

    GetterSetter( const char* n, getter_t g)
        : name(n), getter(g), setter(nullptr)
    {}    

    GetterSetter( const char* n,  setter_t s)
        : name(n), getter(nullptr), setter(s)
    {}    

    const char* name;
    getter_t getter;
    setter_t setter;
};

///////////////////////////////////////////////////////////////////////////////////////////

template<class T, class M>
class GetterSetterConst
{
public:

    typedef M (T::*getter_t)() const;
    typedef void (T::*setter_t)(M);

    GetterSetterConst( const char* n, getter_t g, setter_t s)
        : name(n), getter(g), setter(s)
    {}

    GetterSetterConst( const char* n, getter_t g)
        : name(n), getter(g), setter(nullptr)
    {}

    GetterSetterConst( const char* n,  setter_t s)
        : name(n), getter(nullptr), setter(s)
    {}   

    const char* name;
    getter_t getter;
    setter_t setter;
};

///////////////////////////////////////////////////////////////////////////////////////////

template<class T, class M>
GetterSetter<T,M> getter_setter( const char* n, M (T::*g)(), void (T::*s)(M) )
{
    return GetterSetter<T,M>(n,g,s);
}

template<class T,class M>
GetterSetter<T,M> getter_setter( const char* n, M (T::*g)() )
{
    return GetterSetter<T,M>(n,g);
}

template<class T, class M>
GetterSetterConst<T,M> getter_setter( const char* n, void (T::*s)(M) )
{
    return GetterSetter<T,M>(n,s);
}

template<class T, class M>
GetterSetterConst<T,M> getter_setter( const char* n, M (T::*g)() const, void (T::*s)(M) )
{
    return GetterSetterConst<T,M>(n,g,s);
}


template<class T, class M>
GetterSetterConst<T,M> getter_setter( const char* n, M (T::*g)() const )
{
    return GetterSetterConst<T,M>(n,g);
}


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

template<class T>
class MetaData;

template<class T>
class MetaData<T()>
{
public:

    const char* entity = 0;
    const char* name = 0;

    using value_t = void;

    template<class V>
    void visit( const T& from, V visitor ) const
    {}

    MetaData<T()>& operator[](const char* n)
    {
        entity = n;
		return *this;
    }    
};

///////////////////////////////////////////////////////////////////////////////////////////

template<class T, class M>
class MetaData<T(const char*, M )>
{
public:

    MetaData( const char* n, M m )
        : name(n), member(m)
    {}

    const char* entity = 0;
    const char* name;
    M member;

    using value_t = decltype( std::declval<T>().*member );

    auto get(const T& from) const
    {
        return from.*member;
    }

    template<class P>
    void set(T& to, P value) const
    {
        to.*member = value;
    }

    template<class V>
    void visit( const T& from, V visitor ) const
    {
        visitor(name, *this);
    }

    MetaData<T(const char*, M )>& operator[](const char* n)
    {
        entity = n;
		return *this;
    }
};

///////////////////////////////////////////////////////////////////////////////////////////


template<class T, class M, class ... Args>
class MetaData<T(const char*, M, Args... )> : public MetaData<T(Args ...)>
{
public:

    MetaData( const char* n, M m, Args ... args )
        : MetaData<T(Args ...)>(args...), name(n), member(m)
    {}

    const char* entity = 0;
    const char* name;
    M member;

    using value_t = decltype(  std::declval<T>().*member );

    auto get(const T& from) const
    {
        return from.*member;
    }

    template<class P>
    void set(T& to, P value) const
    {
        to.*member = value;
    }

    template<class V>
    void visit( const T& from, V visitor ) const
    {
        visitor(name, *this);
        MetaData<T(Args...)>:: template visit<V>(from,visitor);
    }

    MetaData<T(const char*, M, Args... )>& operator[](const char* n)
    {
        entity = n;
		return *this;
    }
};

///////////////////////////////////////////////////////////////////////////////////////////

template<class T, class M, class ... Args>
class MetaData<T( Member<T,M>, Args... )> : public MetaData<T(Args ...)>
{
public:

    MetaData( const Member<T,M>& m, Args ... args )
        : MetaData<T(Args ...)>(args...), member(m)
    {}

    const char* entity = 0;
    Member<T,M> member;

    using value_t = M;
    
    M get(const T& from) const
    {
        return from.*member.member;
    }

    void set(T& to, M value) const
    {
        to.*member.member = value;
    }

    template<class V>
    void visit( const T& from, V visitor ) const
    {
        visitor(member.name, *this);
        MetaData<T(Args...)>:: template visit<V>(from,visitor);
    }

    MetaData<T( Member<T,M>, Args... )>& operator[](const char* n)
    {
        entity = n;
		return *this;
    }    
};

///////////////////////////////////////////////////////////////////////////////////////////

template<class T, class M, class ... Args>
class MetaData<T( GetterSetter<T,M>, Args... )> : public MetaData<T(Args ...)>
{
public:

    MetaData( const GetterSetter<T,M>& m, Args ... args )
        : MetaData<T(Args ...)>(args...), getterSetter(m)
    {}

    const char* entity = 0;
    GetterSetter<T,M> getterSetter;

    using value_t = M;

    M get(const T& from) const
    {
        if(!getterSetter.getter)
        {
            return M();
        }
        return (from.*getterSetter.getter)();
    }

    void set(T& to, M value) const
    {
        if(!getterSetter.setter)
        {
            return;
        }        
        (to.*getterSetter.setter)(value);
    }

    template<class V>
    void visit( const T& from, V visitor ) const
    {
        visitor(getterSetter.name, *this );
        MetaData<T(Args...)>:: template visit<V>(from,visitor);
    }

    MetaData<T( GetterSetter<T,M>, Args... )>& operator[](const char* n)
    {
        entity = n;
		return *this;
    }     
};

///////////////////////////////////////////////////////////////////////////////////////////

template<class T, class M, class ... Args>
class MetaData<T( GetterSetterConst<T,M>, Args... )> : public MetaData<T(Args ...)>
{
public:

    MetaData( const GetterSetterConst<T,M>& m, Args ... args )
        : MetaData<T(Args ...)>(args...), getterSetter(m)
    {}

    const char* entity = 0;
    GetterSetterConst<T,M> getterSetter;

    using value_t = M;

    M get(const T& from) const
    {
        if(!getterSetter.getter)
        {
            return M();
        }        
        return (from.*getterSetter.getter)();
    }

    void set(T& to, M value) const
    {
        if(!getterSetter.setter)
        {
            return;
        }        
        (to.*getterSetter.setter)(value);
    }


    template<class V>
    void visit( const T& from, V visitor ) const
    {
        visitor(getterSetter.name, *this );
        MetaData<T(Args...)>:: template visit<V>(from,visitor);
    }

    MetaData<T( GetterSetterConst<T,M>, Args... )>& operator[](const char* n)
    {
        entity = n;
		return *this;
    }     
};

///////////////////////////////////////////////////////////////////////////////////////////


template<class T, class M, class ... Args>
auto metadata( const char* name, M T::*m, Args&& ... args)
{
    return MetaData<T(const char* , M T::*, typename std::decay<Args>::type ...)>( name, m, std::forward<Args>(args)...);
}

template<class T, class M, class ... Args>
auto metadata( Member<T,M>&& member, Args&& ... args)
{
    return MetaData<T( Member<T,M>, typename std::decay<Args>::type ...)>( member, std::forward<Args>(args)...);
}


template<class T, class M, class ... Args>
auto metadata( GetterSetter<T,M>&& getterSetter, Args&& ... args)
{
    return MetaData<T( GetterSetter<T,M>, typename std::decay<Args>::type ...)>( getterSetter, std::forward<Args>(args)...);
}

template<class T, class M, class ... Args>
auto metadata( GetterSetterConst<T,M>&& getterSetter, Args&& ... args)
{
    return MetaData<T( GetterSetterConst<T,M>, typename std::decay<Args>::type ...)>( getterSetter, std::forward<Args>(args)...);
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

template<class T>
auto meta_of(const T& t, typename std::enable_if<std::is_class<T>::value && has_meta<T>::value>::type* = nullptr) 
{
    typedef decltype( t.meta() ) meta_t;

    static meta_t m = t.meta();
    return m;
}


template<class T>
auto meta_of(typename std::enable_if<std::is_class<T>::value && has_meta<T>::value>::type* = nullptr) 
{
    static T t;
    static auto m = t.meta();
    return m;
}

template<class T>
auto meta_of(const T& t, typename std::enable_if<std::is_class<T>::value && !has_meta<T>::value>::type* = nullptr) 
{
    typedef decltype( meta(t) ) meta_t;

    static meta_t m = meta(t);
    return m;
}


template<class T>
auto meta_of(typename std::enable_if<std::is_class<T>::value && !has_meta<T>::value>::type* = nullptr) 
{
    static T t;
    return meta(t);
}

template<class T>
auto meta_of(const std::vector<T>& t)
{
    typedef decltype( meta(std::declval<T>()) ) meta_t;
    static meta_t m = meta(t[0]);

    return m;
}


template<class T>
std::vector<const char*> meta_fields_of(const T& t)
{
    auto m = meta_of<T>();
    std::vector<const char*> result;

    auto visitor = [&result]( const char* n, auto& )
    {	
        result.push_back(n);
    };

    m.visit(t,visitor);     
    return result;
}

template<class T>
std::vector<const char*> meta_fields_of()
{
    static T t;
    return meta_fields_of(t);
}
///////////////////////////////////////////////////////////////////////////////////////////

 template<class R, class T>
 R meta_get(const T& t, const char* name)
 {
    R result;

    auto visitor = [&t, &result, name]( const char* n, auto& m)
    {	
        using value_t = std::remove_reference_t<typename std::remove_reference_t<decltype(m)>::value_t>;
        if constexpr( std::is_assignable<R,value_t>::value )
        {
            if(strcmp(name,n)==0)
            {
                result = m.get(t);
            }
        }
    };

	auto m = meta_of(t);
    m.visit(t,visitor);    

    return result;
 }

  
 template<class T, class P>
 void meta_set( T& t, const char* name,P p)
 {
    auto visitor = [&t, &p, name]( const char* n, auto& m)
    {	
        using value_t = std::remove_reference_t<typename std::remove_reference_t<decltype(m)>::value_t>;
        if constexpr( std::is_assignable<value_t,P>::value )
        {
            if(strcmp(name,n)==0)
            {
                m.set(t,p);
            }
        }
    };

	auto m = meta_of(t);
    m.visit(t,visitor);  
 }


  
 template<class R, class T>
 bool meta_has( T& t, const char* name)
 {
    bool result = false;

    auto visitor = [&t, &result, name]( const char* n, auto& m)
    {	
        using value_t = std::remove_reference_t<typename std::remove_reference_t<decltype(m)>::value_t>;
        if constexpr( std::is_assignable<R,value_t>::value )
        {
            if(strcmp(name,n)==0)
            {
                result = true;
            }
        }
    };

	auto m = meta_of(t);
    m.visit(t,visitor);

    return result;
 }

///////////////////////////////////////////////////////////////////////////////////////////

}

#endif

