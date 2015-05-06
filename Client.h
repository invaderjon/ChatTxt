#ifndef __CLIENT__
#define __CLIENT__

#include <cstdlib>
#include <cstring>
#include <deque>
#include <list>
#include <string>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include "Message.h"

using boost::asio::io_service;
using boost::asio::ip::tcp;
using boost::system::error_code;

class Client
{
 public:
  Client(io_service& service, tcp::resolver::iterator endpointIter);
  ~Client();

  void write(MessagePtr msg);
  void kill();  

 private:
  void onConnected(const error_code& error);
  void listen();
  void flush();
  void onReadHeader(MessagePtr msg, const error_code& error);
  void onReadBody(MessagePtr msg, const error_code& error);
  void onWritten(const error_code& error);

  bool mRunning;
  io_service& mService;
  tcp::socket mSocket;
  std::deque<MessagePtr> mWriteQ;
  std::list<std::string> mSent;
  boost::mutex mWMutex; // write queue mutex
  boost::mutex mWSMutex; // write stream mutex
  boost::mutex mSMutex; // sent mutex
  boost::condition_variable mWCond;
  boost::thread mThread;
};

#endif
