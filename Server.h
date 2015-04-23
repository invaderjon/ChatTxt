#ifndef __SERVER__
#define __SERVER__

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include "Session.h"
#include "SessionManager.h"

using boost::asio::io_service;
using boost::asio::ip::tcp;
using boost::system::error_code;

class Server
  : public boost::enable_shared_from_this<Server>
{
 public:
  Server(io_service& service, const tcp::endpoint& endpoint);
  ~Server();
  void start();
  void stop();
  void listen();
  void accept(SessionPtr session, const error_code& error);
    
 private:
  io_service& mService;
  tcp::acceptor mAcceptor;

  SessionManager mSessionMgr;
  bool mRunning;
};

typedef boost::shared_ptr<Server> ServerPtr;

#endif
