#ifndef FORMAT_H_GUARD_dkjaksdjsadksajdkjd
#define FORMAT_H_GUARD_dkjaksdjsadksajdkjd

#include <ostream>
#include <sstream>

template<typename T, typename U>
char const * format_consume(std::basic_ostream<T> & os, T const * str, U const & v1)
{
    bool pending = false;
    auto pch = str;
    for( ; *pch ; ++pch )
    {
        auto ch = *pch;
        if( pending )
        {
            if( ch == '%' )
            {
                os.put(ch);
                pending = false;
            }
            else
            {
                os << v1;
                ++pch;
                break;
            }
        }
        else
        {
            if( ch == '%' )
            {
                pending = true;
            }
            else
            {
                os.put(ch);
            }
        }
    }
    return pch;
}

template<typename T, typename U>
std::basic_ostream<T> & format(std::basic_ostream<T> & os, T const * str, U const & v1)
{
    str = format_consume(os,str,v1);
    return os << str;
}

template<typename T, typename U>
std::basic_string<T> strformat(T const * str, U const & v1)
{
    std::basic_stringstream<T> ss;
    format(ss,str,v1);
    return ss.str();
}

#ifdef __GNUC__
#  include <features.h>
#  if __GNUC_PREREQ(4,3)
#     define HAS_VARIADIC_TEMPLATES
#  endif
#elif defined(__clang__)
#  if __has_feature(cxx_variadic_templates)
#     define HAS_VARIADIC_TEMPLATES
#  endif
#endif

#ifdef HAS_VARIADIC_TEMPLATES

template<typename T, typename U, typename... V>
std::basic_ostream<T> & format(std::basic_ostream<T> & os, T const * str, U const & v1, V... rest)
{
    str = format_consume(os,str,v1);
    return format(os,str,rest...);
}

template<typename T, typename U, typename... V>
std::basic_string<T> strformat(T const * str, U const & v1, V... rest)
{
    std::basic_stringstream<T> ss;
    return format(ss,str,v1,rest...).str();
}

#else

// we have to define all overloads by hand. MSVC is a pain on the ass

template<typename T, typename U, typename V>
std::basic_ostream<T> & format(std::basic_ostream<T> & os, T const * str, U const & v1, V const & v2)
{
    str = format_consume(os,str,v1);
    str = format_consume(os,str,v2);
    return os << str;
}

template<typename T, typename U, typename V, typename X>
std::basic_ostream<T> & format(std::basic_ostream<T> & os, T const * str, U const & v1, V const & v2, X const & v3)
{
    str = format_consume(os,str,v1);
    str = format_consume(os,str,v2);
    str = format_consume(os,str,v3);
    return os << str;
}

template<typename T, typename U, typename V, typename X, typename Y>
std::basic_ostream<T> & format(std::basic_ostream<T> & os, T const * str, U const & v1, V const & v2, X const & v3, Y const & v4)
{
    str = format_consume(os,str,v1);
    str = format_consume(os,str,v2);
    str = format_consume(os,str,v3);
    str = format_consume(os,str,v4);
    return os << str;
}

template<typename T, typename U, typename V, typename X, typename Y, typename Z>
std::basic_ostream<T> & format(std::basic_ostream<T> & os, T const * str, U const & v1, V const & v2, X const & v3, Y const & v4, Z const & v5)
{
    str = format_consume(os,str,v1);
    str = format_consume(os,str,v2);
    str = format_consume(os,str,v3);
    str = format_consume(os,str,v4);
    str = format_consume(os,str,v5);
    return os << str;
}

template<typename T, typename U, typename V, typename X, typename Y, typename Z, typename Z1>
std::basic_ostream<T> & format(std::basic_ostream<T> & os, T const * str, U const & v1, V const & v2, X const & v3, Y const & v4, Z const & v5, Z1 const & v6)
{
    str = format_consume(os,str,v1);
    str = format_consume(os,str,v2);
    str = format_consume(os,str,v3);
    str = format_consume(os,str,v4);
    str = format_consume(os,str,v5);
    str = format_consume(os,str,v6);
    return os << str;
}

template<typename T, typename U, typename V, typename X, typename Y, typename Z, typename Z1, typename Z2>
std::basic_ostream<T> & format(std::basic_ostream<T> & os, T const * str, U const & v1, V const & v2, X const & v3, Y const & v4, Z const & v5, Z1 const & v6, Z2 const & v7)
{
    str = format_consume(os,str,v1);
    str = format_consume(os,str,v2);
    str = format_consume(os,str,v3);
    str = format_consume(os,str,v4);
    str = format_consume(os,str,v5);
    str = format_consume(os,str,v6);
    str = format_consume(os,str,v7);
    return os << str;
}


template<typename T, typename U, typename V>
std::basic_string<T> strformat(T const * str, U const & v1, V const & v2)
{
    std::basic_stringstream<T> ss;
    format(ss,str,v1,v2);
    return ss.str();
}

template<typename T, typename U, typename V, typename X>
std::basic_string<T> strformat(T const * str, U const & v1, V const & v2, X const & v3)
{
    std::basic_stringstream<T> ss;
    format(ss,str,v1,v2,v3);
    return ss.str();
}

template<typename T, typename U, typename V, typename X, typename Y>
std::basic_string<T> strformat(T const * str, U const & v1, V const & v2, X const & v3, Y const & v4)
{
    std::basic_stringstream<T> ss;
    format(ss,str,v1,v2,v3,v4);
    return ss.str();
}

template<typename T, typename U, typename V, typename X, typename Y, typename Z>
std::basic_string<T> strformat(T const * str, U const & v1, V const & v2, X const & v3, Y const & v4, Z const & v5)
{
    std::basic_stringstream<T> ss;
    format(ss,str,v1,v2,v3,v4,v5);
    return ss.str();
}

template<typename T, typename U, typename V, typename X, typename Y, typename Z, typename Z1>
std::basic_string<T> strformat(T const * str, U const & v1, V const & v2, X const & v3, Y const & v4, Z const & v5, Z1 const & v6)
{
    std::basic_stringstream<T> ss;
    format(ss,str,v1,v2,v3,v4,v5,v6);
    return ss.str();
}

template<typename T, typename U, typename V, typename X, typename Y, typename Z, typename Z1, typename Z2>
std::basic_string<T> strformat(T const * str, U const & v1, V const & v2, X const & v3, Y const & v4, Z const & v5, Z1 const & v6, Z2 const & v7)
{
    std::basic_stringstream<T> ss;
    format(ss,str,v1,v2,v3,v4,v5,v6,v7);
    return ss.str();
}


#endif

#endif
