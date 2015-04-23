
#include "Session.h"
#include "SessionManager.h"

Session::Session(io_service& service, SessionManager& mgr)
  : mSocket(service), mManager(mgr)
{
}

Session::~Session()
{
}

tcp::socket& Session::socket()
{
  return mSocket;
}

void Session::start()
{
  // register this session
  mManager.add(shared_from_this());

  printf("Session starting...\n");

  // start listening for messages
  listen();
}

void Session::kill()
{
  // unregisters this session
  mManager.remove(shared_from_this());

  printf("Session killed\n");

  // kills the socket
  error_code ec;
  mSocket.close(ec);
}

void Session::write(MessagePtr msg)
{
  // prepares message for delivery
  msg->encode();

  // appends the message to the queue
  mMutex.lock();
  mWriteQ.push_back(msg);

  // if a write isn't in progress then write
  if (mWriteQ.size() == 1)
    {
      boost::asio::async_write(mSocket,
			       boost::asio::buffer(msg->raw(), msg->rawLength()),
			       boost::bind(&Session::onWritten,
					   shared_from_this(),
					   boost::asio::placeholders::error));
    }
  mMutex.unlock();
}

void Session::listen()
{
  MessagePtr msg(new Message());
  boost::asio::async_read(mSocket,
			  boost::asio::buffer(msg->raw(), Message::HeaderLength),
			  boost::bind(&Session::onReadHeader,
				      shared_from_this(),
				      msg,
				      boost::asio::placeholders::error));
}

void Session::onReadHeader(MessagePtr msg, const error_code& error)
{
  // make sure everything was successful
  if (!error && msg->decodeHeader())
    {
      // read content
      boost::asio::async_read(mSocket,
			      boost::asio::buffer(msg->rawBody(), msg->rawBodyLength()),
			      boost::bind(&Session::onReadBody,
					  shared_from_this(),
					  msg,
					  boost::asio::placeholders::error));
    }
  else // error
    mManager.remove(shared_from_this());
}

void Session::onReadBody(MessagePtr msg, const error_code& error)
{
  // make sure everything was successful
  if (!error && msg->decode())
    {
      // broadcast the message
      mManager.broadcast(msg);
      
      // start listening for the next message
      listen();
    }
  else // error
    mManager.remove(shared_from_this());
}

void Session::onWritten(const error_code& error)
{
  //  make sure everything was successful
  if (!error)
    {
      // remove written message from queue
      mMutex.lock();
      mWriteQ.pop_front();

      // if there is more to write send out the next message
      if (!mWriteQ.empty())
	{
	  MessagePtr o = mWriteQ.front();
	  boost::asio::async_write(mSocket,
				   boost::asio::buffer(o->raw(),
						       o->rawLength()),
				   boost::bind(&Session::onWritten,
					       shared_from_this(),
					       boost::asio::placeholders::error));
	}
      mMutex.unlock();
    }
  else // error
    mManager.remove(shared_from_this());
}
