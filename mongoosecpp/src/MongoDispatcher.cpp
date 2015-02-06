#include "MongoDispatcher.h"
#include <fstream>
#include <algorithm>
#include <cctype>
#include <functional>
#include <iostream>

namespace Mongo {

using std::bind;
using std::placeholders::_1;
using std::placeholders::_2;

Dispatcher::Dispatcher(Server & server)
	:page404([](Request request, Response response) -> bool {
		response.status(404);
		if( request.getQueryString_c() ) {
			response.printf("URL resource %s?%s not found",request.getURI_c(),request.getQueryString_c());
		} else {
			response.printf("URL resource %s not found",request.getURI_c());
		}
		return true;
	})
{
	server.setStart(bind(&Dispatcher::dispatch,this,_1,_2));
}

void Dispatcher::staticPages(std::string urlpath, std::string const & path)
{
	if( urlpath.empty() || urlpath.back() != '/' ) {
		urlpath.push_back('/');
	}
	servePrefix(urlpath,bind(&Dispatcher::dispatchStatic,this,_1,_2,path));
}

void Dispatcher::staticFile(std::string const & urlpath, std::string const & filename)
{
	serve(urlpath,bind(&Dispatcher::dispatchFile,this,_1,_2,filename));
}

void Dispatcher::serve(std::string const & urlpath, Callback handler)
{
	dispatchMap[urlpath] = handler;
}

void Dispatcher::servePrefix(std::string const & urlpath, Callback handler)
{
	dispatchMapPrefix[urlpath] = handler;
}

bool Dispatcher::dispatch(Request request, Response response)
{
	// first search on absolute URIs
	auto uri = request.getURI();
	auto it = dispatchMap.find(uri);
	if( it != dispatchMap.end() ) return it->second(request,response);

	// no need to search on prefix map if we don't have a resource 
	if( request.getResource().empty() ) return page404(request,response);

	// now search on prefix maps
	auto path = request.getPath();
	auto itp = dispatchMapPrefix.find(path);
	return itp == dispatchMapPrefix.end() ? page404(request,response) : itp->second(request,response);
}

bool Dispatcher::dispatchStatic(Request request, Response response, std::string const & localPath)
{
	return dispatchFile(request,response,localPath + '/' + request.getResource());
}

bool Dispatcher::dispatchFile(Request request, Response response, std::string const & filename)
{
	std::ifstream file(filename.c_str(),std::ios::binary);
	if( ! file ) return page404(request,response);

	response.contentType(getContentType(filename));
	response.write(file);
	return true;
}

std::string Dispatcher::getContentType(std::string const & filename) const
{
	auto p = filename.rfind('.');
	if( p >= filename.length() ) return "text/plain";

	auto ext = filename.substr(p+1);
	std::transform(ext.begin(),ext.end(),ext.begin(),&std::tolower);
	// TODO use a map
	if( ext == "htm" ) ext = "html";
	if( ext == "jpg" ) ext = "jpeg";
	if( ext == "xml" || ext == "html" || ext == "css" ) return "text/" + ext;
	if( ext == "jpeg" || ext == "gif" || ext == "png" ) return "image/" + ext;

	return "text/plain";
}

}
