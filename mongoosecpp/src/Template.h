#ifndef TEMPLATE_H_GUARD_j4329sdfd
#define TEMPLATE_H_GUARD_j4329sdfd

#include "MongoResponse.h"
#include "format.h"

namespace Mongo
{

class Template
{
    Response & response;
    std::string basePath;
    std::string loadFileContents(char const * filename);
public:
    Template(Response & response, std::string basePath = std::string());
    void print(char const * filename);
    void printf(char const * filename, ...);

    // boilerplate code for idiotic compilers like MSVC

    template<typename U>
    void print(char const * filename, U const & v1)
    {
        auto mask = loadFileContents(filename);
        auto str = strformat(mask.c_str(),v1);
        response.write(str.c_str(),str.size());
    }

    template<typename U, typename V>
    void print(char const * filename, U const & v1, V const & v2)
    {
        auto mask = loadFileContents(filename);
        auto str = strformat(mask.c_str(),v1,v2);
        response.write(str.c_str(),str.size());
    }

    template<typename U, typename V, typename X>
    void print(char const * filename, U const & v1, V const & v2, X const & v3)
    {
        auto mask = loadFileContents(filename);
        auto str = strformat(mask.c_str(),v1,v2,v3);
        response.write(str.c_str(),str.size());
    }

    template<typename U, typename V, typename X, typename Y>
    void print(char const * filename, U const & v1, V const & v2, X const & v3, Y const & v4)
    {
        auto mask = loadFileContents(filename);
        auto str = strformat(mask.c_str(),v1,v2,v3,v4);
        response.write(str.c_str(),str.size());
    }

    template<typename U, typename V, typename X, typename Y, typename Z>
    void print(char const * filename, U const & v1, V const & v2, X const & v3, Y const & v4, Z const & v5)
    {
        auto mask = loadFileContents(filename);
        auto str = strformat(mask.c_str(),v1,v2,v3,v4,v5);
        response.write(str.c_str(),str.size());
    }

    template<typename U, typename V, typename X, typename Y, typename Z, typename Z1>
    void print(char const * filename, U const & v1, V const & v2, X const & v3, Y const & v4, Z const & v5, Z1 const & v6)
    {
        auto mask = loadFileContents(filename);
        auto str = strformat(mask.c_str(),v1,v2,v3,v4,v5,v6);
        response.write(str.c_str(),str.size());
    }

    template<typename U, typename V, typename X, typename Y, typename Z, typename Z1, typename Z2>
    void print(char const * filename, U const & v1, V const & v2, X const & v3, Y const & v4, Z const & v5, Z1 const & v6, Z2 const & v7)
    {
        auto mask = loadFileContents(filename);
        auto str = strformat(mask.c_str(),v1,v2,v3,v4,v5,v6,v7);
        response.write(str.c_str(),str.size());
    }

};

}

#endif
