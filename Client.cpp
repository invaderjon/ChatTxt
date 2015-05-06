#include "Client.h"

Client::Client(io_service& service, tcp::resolver::iterator endpointIter)
  : mRunning(true),
    mService(service),
    mSocket(service),
    mThread(boost::bind(&Client::flush, this))
{
  boost::asio::async_connect(mSocket, endpointIter,
			     boost::bind(&Client::onConnected, this,
					 boost::asio::placeholders::error));
}

Client::~Client()
{
}

void Client::write(MessagePtr msg)
{
  if (!mRunning)
    return;

  // encodes message before sending
  msg->encode();
  
  // append the message
  boost::lock_guard<boost::mutex> lock(mWMutex);
  mWriteQ.push_back(msg);
  boost::lock_guard<boost::mutex> slock(mSMutex);
  mSent.push_back(std::string(msg->body()));

  // notify of new message
  mWCond.notify_one();
}

void Client::kill()
{
  // kills the writing thread
  printf("Killed\n");
  mRunning = false;
  mWCond.notify_one();
  mThread.join();
}

void Client::onConnected(const error_code& error)
{
  if (!error)
    listen();
  else
    {
      printf("Fatal error while connecting\n");
      kill();
    }
}

void Client::listen()
{
  MessagePtr msg(new Message());
  boost::asio::async_read(mSocket,
			  boost::asio::buffer(msg->raw(), Message::HeaderLength),
			  boost::bind(&Client::onReadHeader,
				      this,
				      msg,
				      boost::asio::placeholders::error));
}

void Client::flush()
{
  while (mRunning)
    {
      // waits for messages to send
      boost::unique_lock<boost::mutex> lock(mWMutex);
      while (mWriteQ.empty() && mRunning)
	mWCond.wait(lock);

      if (mRunning)
	{
	  // get and remove next item
	  MessagePtr msg = mWriteQ.front();
	  mWriteQ.pop_front();
	  
	  // send it to the thread, but wait for other writes to finish
	  mWSMutex.lock();
	  boost::asio::async_write(mSocket,
				   boost::asio::buffer(msg->raw(),
						       msg->rawLength()),
				   boost::bind(&Client::onWritten,
					       this,
					       boost::asio::placeholders::error));
	}
    }
    printf("Done flushing...\n");
}

void Client::onReadHeader(MessagePtr msg, const error_code& error)
{
  // make sure everything was successful
  if (!error && msg->decodeHeader())
    {
      // read content
      boost::asio::async_read(mSocket,
			      boost::asio::buffer(msg->rawBody(), msg->rawBodyLength()),
			      boost::bind(&Client::onReadBody,
					  this,
					  msg,
					  boost::asio::placeholders::error));
    }
  else // error
    {
      printf("Error reading header\n");
      kill();
    }
}

void Client::onReadBody(MessagePtr msg, const error_code& error)
{
  // make sure everything was successful
  if (!error && msg->decode())
    {
      // list of found messages
      bool found = false;
      boost::unique_lock<boost::mutex> slock(mSMutex);
      for (auto iter = mSent.begin(); iter != mSent.end() && !found; ++iter)
	{
	  if (!strcmp(iter->c_str(), msg->body()))
	    {
	      found = true;
	      mSent.erase(iter);
	    }
	}
            
      // prints the body
      if (!found)
	printf("%s\n", msg->body());
      
      // start listening for the next message
      listen();
    }
  else // error
    {
      printf("Error reading body\n");
      kill();
    }
}

void Client::onWritten(const error_code& error)
{
  // let other threads know write operation complete
  mWSMutex.unlock();
  
  // if it failed to write exit
  if (error)
    {
      printf("Error occurred while writing\n");
      kill();
    }
}
