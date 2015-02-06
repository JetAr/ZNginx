#ifndef MONGOOSE_REQ_H_GUARD_kjdff49w8ur43bvf67
#define MONGOOSE_REQ_H_GUARD_kjdff49w8ur43bvf67

#include <string>
#include <vector>

struct mg_connection;
struct mg_request_info;

namespace Mongo
{

enum Method { UNKNOWN_METHOD, GET, HEAD, POST, PUT, DELETE, TRACE, CONNECT, };

class Request
{
    struct mg_request_info const * request_info;
    struct mg_connection * conn;
    mutable char const * pathPos;
    mutable char const * pathEnd;
    mutable std::vector<char> postBuffer;
    void computePathPos() const;
    void computePostBuffer() const;
    bool hasVar(char const * name, char const * buf, size_t size) const;
    std::string getVar(char const * name, char const * buf, size_t size) const;
public:
    Request(struct mg_request_info const * request_info, struct mg_connection * conn);
    bool hasGet(char const * name) const;
    std::string get(char const * name) const;
    bool hasPost(char const * name) const;
    std::string post(char const * name) const;
    std::string post(std::string const & name) const;
    char const * getURI_c() const;
    std::string getURI() const;
    char const * getQueryString_c() const;
    std::string getQueryString() const;
    std::string getPath() const;
    std::string getResource() const;
    Method getMethod() const;
    char const * getMethod_c() const;
    char const * getHeader_c(char const * name) const;
    char const * getContentType_c() const;
    unsigned long getContentLength() const;
};

}

#endif

