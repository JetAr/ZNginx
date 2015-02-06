#include "MongoServer.h"
#include "mongoose.h"
#include <algorithm>
#include <iterator>

namespace Mongo
{

static void * CallbackWrapper(enum ::mg_event event, struct ::mg_connection * conn, const struct ::mg_request_info * request_info)
{
    auto server = reinterpret_cast<Server const *>(request_info->user_data);
    Request req(request_info,conn);
    Response resp(conn);
    switch(event)
    {
    case MG_NEW_REQUEST:
        return server->cbStart(req,resp) ? "" : 0;
    case MG_HTTP_ERROR:
        return server->cbError(req,resp) ? "" : 0;
    case MG_EVENT_LOG:
        return server->cbLog(req,resp) ? "" : 0;
    case MG_INIT_SSL:
        return server->cbInitSSL(req,resp) ? "" : 0;
    case MG_REQUEST_COMPLETE:
        return server->cbEnd(req,resp) ? "" : 0;
    default:
        return 0;
    }
}

void Server::ContextDeleter::operator()(::mg_context * ctx) const
{
    mg_stop(ctx);
}

Server::Server():
    cbStart([](Request,Response)
{
    return true;
}),
cbEnd(cbStart),
cbError(cbStart),
cbLog(cbStart),
cbInitSSL(cbStart)
{}

Server & Server::setOption(std::string const & name, std::string const & value)
{
    options.push_back(name);
    options.push_back(value);
    return *this;
}

#define SCB(m_cb) { \
	m_cb = cb; \
	return *this; \
}

Server & Server::setStart(Callback cb) SCB(cbStart)
Server & Server::setEnd(Callback cb) SCB(cbEnd)
Server & Server::setError(Callback cb) SCB(cbError)
Server & Server::setLog(Callback cb) SCB(cbLog)
Server & Server::setInitSSL(Callback cb) SCB(cbInitSSL)

void Server::start()
{
    stop();
    auto cOpt = getOptions();
    auto ptr = mg_start(&CallbackWrapper,this,&cOpt[0]);
    ctx.reset(ptr);
}

std::vector<char const *> Server::getOptions() const
{
    using namespace std;
    vector<char const *> result;
    transform(options.begin(),options.end(),back_inserter<>(result),[](string const & str)
    {
        return str.c_str();
    });
    result.push_back(0);
    return result;
}

void Server::stop()
{
    ctx.reset();
}

Server::~Server()
{
}

}
