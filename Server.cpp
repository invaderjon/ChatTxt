#include "Server.h"

Server::Server(int port)
  : mRunning(FALSE), mPort(port)
{
  // initializes components
  initConnMgr();
  initBroadcaster();
}

Server::~Server()
{
}

void Server::start()
{
  // sets it up to run
  mRunning = true;
  listen();
}

void Server::stop()
{
  
}

void Server::initConnMgr()
{
  
}

void Server::initBroadcaster()
{
  
}

void Server::listen()
{
  while (mRunning)
    {
      // loop to do shit
    }
}
