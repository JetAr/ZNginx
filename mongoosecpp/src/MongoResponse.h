#ifndef MONGOOSE_RESP_H_GUARD_kjdff49w8ur43bvf67432353465
#define MONGOOSE_RESP_H_GUARD_kjdff49w8ur43bvf67432353465

#include <stdarg.h>
#include <string>
#include <istream>

struct mg_connection;

namespace Mongo {

class Response {
	struct mg_connection * conn;
	bool statusSet;
	bool contentTypeSet;
	bool headersSent;
	int do_printf(const char *fmt, ...);
	void checkHeadersSent();
public:
	Response(struct mg_connection * conn);
	Response & status(int code);
	Response & contentType(char const * type);
	Response & contentType(std::string const & type);
	int printf(const char *fmt, ...);	// :-(
	int vprintf(const char *fmt, va_list ap);  // :-[
	void write(char const * buf, size_t size);
	void write(std::istream & is);
};

}

#endif

