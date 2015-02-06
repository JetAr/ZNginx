#include "MongoResponse.h"
#include "mongoose.h"

namespace Mongo {

int const BUFSIZE = 8192;

Response::Response(struct mg_connection * conn):
	conn(conn),
	statusSet(false),
	contentTypeSet(false),
	headersSent(false)
{}

Response & Response::status(int code)
{
	if( statusSet ) return *this;

	do_printf("HTTP/1.1 %d OK\r\n",code);
	statusSet = true;
	return *this;
}

Response & Response::contentType(char const * type)
{
	if( contentTypeSet ) return *this;

	if( ! statusSet ) {
		status(200);
	}

	do_printf("Content-Type: %s\r\n",type);
	contentTypeSet = true;
	return *this;
}

Response & Response::contentType(std::string const & type)
{
	return contentType(type.c_str());
}

int Response::printf(const char *fmt, ...)
{
	checkHeadersSent();
	va_list ap;
	va_start(ap, fmt);
	int result = vprintf(fmt,ap);
	va_end(ap);
	return result;
}

void Response::checkHeadersSent()
{
	if( ! headersSent ) {
		if( ! contentTypeSet ) {
			contentType("text/plain");
		}
		mg_write(conn, "\r\n", 2);
		headersSent = true;
	}
}

int Response::do_printf(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	int result = vprintf(fmt,ap);
	va_end(ap);
	return result;
}

int Response::vprintf(const char *fmt, va_list ap)
{
	char buf[BUFSIZE];
	int len = mg_vsnprintf(conn, buf, sizeof(buf), fmt, ap);
	return mg_write(conn, buf, (size_t)len);
}

void Response::write(char const * buf, size_t size)
{
	checkHeadersSent();
	mg_write(conn, buf, size);
}

void Response::write(std::istream & is)
{
	char buf[BUFSIZE];
	while( is ) {
		is.read(&buf[0],BUFSIZE);
		write(&buf[0],is.gcount());
	}
}


}
