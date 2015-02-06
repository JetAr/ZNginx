#ifndef MongoDispatcher_H_guard_dsja2384rfrgds
#define MongoDispatcher_H_guard_dsja2384rfrgds

#include <string>
#include "MongoServer.h"
#include <unordered_map>

namespace Mongo {

class Dispatcher {
	std::unordered_map<std::string,Callback> dispatchMap;
	std::unordered_map<std::string,Callback> dispatchMapPrefix;
	bool dispatch(Request request, Response response);
	bool dispatchStatic(Request request, Response response, std::string const & localPath);
	bool dispatchFile(Request request, Response response, std::string const & filename);
	std::string getContentType(std::string const & filename) const;
public:
	explicit Dispatcher(Server & server);
	void staticFile(std::string const & urlpath, std::string const & filename);
	void staticPages(std::string urlpath, std::string const & path);
	void serve(std::string const & urlpath, Callback handler);
	void servePrefix(std::string const & urlpath, Callback handler);
	Callback page404;
};

}

#endif
