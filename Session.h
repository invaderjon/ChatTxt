#ifndef __SESSION__
#define __SESSION__

#include <deque>
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "Message.h"

using boost::asio::io_service;
using boost::asio::ip::tcp;
using boost::system::error_code;

class SessionManager;

class Session
  : public boost::enable_shared_from_this<Session>
{

 public:
  Session(io_service& service, SessionManager& mgr);
  ~Session();

  tcp::socket& socket();
  void start();
  void kill();
  void write(MessagePtr msg);

 private:
  void listen();
  void onReadHeader(MessagePtr msg, const error_code& error);
  void onReadBody(MessagePtr msg, const error_code& error);
  void onWritten(const error_code& error);

  boost::mutex mMutex;
  std::deque<MessagePtr> mWriteQ;
  tcp::socket mSocket;
  SessionManager& mManager;
};

typedef boost::shared_ptr<Session> SessionPtr;

#endif
