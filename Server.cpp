#include "Server.h"

Server::Server(io_service& service, const tcp::endpoint& endpoint)
  : mRunning(false), mPort(port),
    mService(service),
    mAcceptor(service, endpoint),
    mSessionMgr()
{
}

Server::~Server()
{
}

void Server::start()
{
  // start listening
  mRunning = true;
  listen();
}

void Server::stop()
{
  // stop listening
  mRunning = false;
  mConnMgr.kill();
}

void Server::listen()
{
  // if we're not still running stop listening
  if (!mRunning)
    return;

  SessionPtr session(new Session(mService, mSessionMgr));
  mAccepter.async_accept(session->socket(),
			 boost::bind(&Server::accept,
				     this,
				     session,
				     boost::asio::placeholders::error));
}

void Server::accept(SessionPtr session, const error_code& error)
{
  if (mRunning)
    {
      if (!error)
	session->start();

      listen();
    }
}
