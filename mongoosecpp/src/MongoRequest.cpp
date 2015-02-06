#include "MongoRequest.h"
#include "mongoose.h"
#include <cstring>

#ifndef MAXGETSIZE
#define MAXGETSIZE 200
#endif

namespace Mongo {

using std::strcmp;

Request::Request(struct mg_request_info const * request_info, struct mg_connection * conn):
	request_info(request_info),
	conn(conn),
	pathPos(0)
{}

bool Request::hasVar(char const * name, char const * buf, size_t size) const
{
	char outbuf[MAXGETSIZE];
	return mg_get_var(buf,size,name,outbuf,sizeof(outbuf)) > 0;
}

std::string Request::getVar(char const * name, char const * buf, size_t size) const
{
	std::string outbuf;
	outbuf.resize(MAXGETSIZE);
	auto sz = mg_get_var(buf,size,name,&outbuf[0],outbuf.size());
	if( sz == -1 ) return std::string();

	outbuf.resize(sz);
	return outbuf;
}

bool Request::hasGet(char const * name) const
{
	if( ! request_info->query_string ) return false;
	return hasVar(name,request_info->query_string,std::strlen(request_info->query_string));
}

std::string Request::get(char const * name) const
{
	if( ! request_info->query_string ) return std::string();
	return getVar(name,request_info->query_string,std::strlen(request_info->query_string));
}

bool Request::hasPost(char const * name) const
{
	computePostBuffer();
	return hasVar(name,&postBuffer[0],postBuffer.size());
}


std::string Request::post(std::string const & name) const
{
	return post(name.c_str());
}

std::string Request::post(char const * name) const
{
	computePostBuffer();
	return getVar(name,&postBuffer[0],postBuffer.size());
}


char const * Request::getURI_c() const
{
	return request_info->uri;
}

std::string Request::getURI() const
{
	return request_info->uri;
}

char const * Request::getQueryString_c() const
{
	return request_info->query_string;
}

std::string Request::getQueryString() const
{
	return request_info->query_string;
}

void Request::computePathPos() const
{
	if( pathPos ) return;

	char const * slash = 0;
	char const * it = request_info->uri;
	while( *it ) {
		if( *it == '/' ) {
			slash = it;
		}
		++it;
	}

	pathEnd = it;
	pathPos = slash == 0 ? pathEnd : slash + 1;
}

void Request::computePostBuffer() const
{
	if( ! postBuffer.empty() ) return;
	if( strcmp(getContentType_c(),"application/x-www-form-urlencoded") != 0 ) return;

	postBuffer.resize(getContentLength());
	mg_read(conn,&postBuffer[0],postBuffer.size());
}


std::string Request::getPath() const
{
	computePathPos();
	return std::string(request_info->uri,pathPos);
}

std::string Request::getResource() const
{
	computePathPos();
	return std::string(pathPos,pathEnd);
}

Method Request::getMethod() const
{
	if( strcmp("GET",request_info->request_method) == 0 ) return GET;
	if( strcmp("HEAD",request_info->request_method) == 0 ) return HEAD;
	if( strcmp("POST",request_info->request_method) == 0 ) return POST;
	if( strcmp("PUT",request_info->request_method) == 0 ) return PUT;
	if( strcmp("DELETE",request_info->request_method) == 0 ) return DELETE;
	if( strcmp("TRACE",request_info->request_method) == 0 ) return TRACE;
	if( strcmp("CONNECT",request_info->request_method) == 0 ) return CONNECT;
	return UNKNOWN_METHOD;
}

char const * Request::getMethod_c() const
{
	return request_info->request_method;
}


char const * Request::getHeader_c(char const * name) const
{
	return get_header(request_info,name);
}

char const * Request::getContentType_c() const
{
	auto result = getHeader_c("Content-Type");
	if( ! result ) return "";

	return result;
}

unsigned long Request::getContentLength() const
{
	auto value = getHeader_c("Content-Length");
	if( ! value ) return 0;

	return atol(value);
}

}
