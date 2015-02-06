#ifndef MONGOOSE_SERVER_H_GUARD
#define MONGOOSE_SERVER_H_GUARD

struct mg_context;

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include "MongoRequest.h"
#include "MongoResponse.h"

namespace Mongo {

typedef std::function<bool(Request,Response)> Callback;

class Server {
	struct ContextDeleter {
		void operator()(::mg_context * ctx) const;
	};
	std::unique_ptr<::mg_context,ContextDeleter> ctx;
	std::vector<std::string> options;
	std::vector<char const *> getOptions() const;
public:
	Server();
	Server & setOption(std::string const & name, std::string const & value);
	Server & setStart(Callback cb);
	Server & setEnd(Callback cb);
	Server & setError(Callback cb);
	Server & setLog(Callback cb);
	Server & setInitSSL(Callback cb);
	void start();
	void stop();
	~Server();

	Callback cbStart;
	Callback cbEnd;
	Callback cbError;
	Callback cbLog;
	Callback cbInitSSL;
};

}

#endif
