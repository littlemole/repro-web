#ifndef _MOL_DEF_GUARD_DEFINE_REPROWEB_SERIALIZER_META_DEF_GUARD_
#define _MOL_DEF_GUARD_DEFINE_REPROWEB_SERIALIZER_META_DEF_GUARD_

#include "reprocpp/ex.h"
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
    return GetterSetterConst<T,M>(n,s);
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

    template<class S>
    void serialize( const T& from, void* to ) const
    {
    }


    template<class S>
    void deserialize( const void* from, T& to ) const
    {
    }

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
    {
    }

    const char* entity = 0;
    const char* name;
    M member;

    template<class S>
    void serialize( const T& from, void* to ) const
    {
        S::serialize( name, from.*member, to );
    }


    template<class S>
    void deserialize( const void* from, T& to ) const
    {
        S::deserialize( name, from, to.*member );
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
    {
    }

    const char* entity = 0;
    const char* name;
    M member;

    template<class S>
    void serialize( const T& from, void* to ) const
    {
        S::serialize( name, from.*member, to);
        MetaData<T(Args...)>:: template serialize<S>(from,to);
    }


    template<class S>
    void deserialize(const void* from, T& to ) const
    {
        S::deserialize( name, from, to.*member);
        MetaData<T(Args...)>:: template deserialize<S>(from,to);
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
    {
    }

    const char* entity = 0;
    Member<T,M> member;

    template<class S>
    void serialize( const T& from, void* to ) const
    {
        S::serialize( member.name, from.*(member.member), to);
        MetaData<T(Args...)>:: template serialize<S>(from,to);
    }


    template<class S>
    void deserialize( const void* from, T& to ) const
    {
        S::deserialize( member.name, from, to.*(member.member));
        MetaData<T(Args...)>:: template deserialize<S>(from,to);
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
    {
    }

    const char* entity = 0;
    GetterSetter<T,M> getterSetter;

    template<class S>
    void serialize( const T& from, void* to ) const
    {
        if(getterSetter.getter)
        {
            std::remove_const_t<std::remove_reference_t<M>> m = (from.*getterSetter.getter)();
            S::serialize( getterSetter.name, m, to);
        }
        MetaData<T(Args...)>:: template serialize<S>(from,to);
    }


    template<class S>
    void deserialize( const void* from, T& to ) const
    {
        if(getterSetter.setter)
        {
            std::remove_const_t<std::remove_reference_t<M>> m;
            S::deserialize( getterSetter.name, from, m);
            (to.*getterSetter.setter)(m);
        }
        MetaData<T(Args...)>:: template deserialize<S>(from,to);
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
    {
    }

    const char* entity = 0;
    GetterSetterConst<T,M> getterSetter;

    template<class S>
    void serialize( const T& from, void* to ) const
    {
        if(getterSetter.getter)
        {
            std::remove_const_t<std::remove_reference_t<M>> m = (from.*getterSetter.getter)();
            S::serialize( getterSetter.name, m, to);
        }
        MetaData<T(Args...)>:: template serialize<S>(from,to);
    }


    template<class S>
    void deserialize( const void* from, T& to ) const
    {
        if(getterSetter.setter)
        {
            std::remove_const_t<std::remove_reference_t<M>> m;
            S::deserialize( getterSetter.name, from, m);
            (to.*getterSetter.setter)(m);
        }
        MetaData<T(Args...)>:: template deserialize<S>(from,to);
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
    return MetaData<T(const char* , M T::*, typename std::decay<Args>::type ...)>( name, m, std::forward<Args&&>(args)...);
}

template<class T, class M, class ... Args>
auto metadata( Member<T,M>&& member, Args&& ... args)
{
    return MetaData<T( Member<T,M>, typename std::decay<Args>::type ...)>( member, std::forward<Args&&>(args)...);
}


template<class T, class M, class ... Args>
auto metadata( GetterSetter<T,M>&& getterSetter, Args&& ... args)
{
    return MetaData<T( GetterSetter<T,M>, typename std::decay<Args>::type ...)>( getterSetter, std::forward<Args&&>(args)...);
}

template<class T, class M, class ... Args>
auto metadata( GetterSetterConst<T,M>&& getterSetter, Args&& ... args)
{
    return MetaData<T( GetterSetterConst<T,M>, typename std::decay<Args>::type ...)>( getterSetter, std::forward<Args&&>(args)...);
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
auto meta_of(const T& t, typename std::enable_if<std::is_class<T>::value && !has_meta<T>::value>::type* = nullptr) 
{
    typedef decltype( meta(t) ) meta_t;

    static meta_t m = meta(t);
    return m;
}

    

///////////////////////////////////////////////////////////////////////////////////////////

template<class T>
auto meta_of(const std::vector<T>& t)
{
    typedef decltype( meta(std::declval<T>()) ) meta_t;
    static meta_t m = meta(t[0]);

    return m;
}


///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

}

#endif

