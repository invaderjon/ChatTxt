#include "SessionManager.h"

SessionManager::SessionManager()
  : mRunning(true),
    mThread(boost::bind(&SessionManager::listen, this))
{
}

SessionManager::~SessionManager()
{
}

void SessionManager::kill()
{
  printf("Killing broadcast thread\n");

  // kills listener thread
  mRunning = false;
  mBCond.notify_one();
  mThread.join();

  printf("Killing all sessions\n");

  // kills all sessions
  mMutex.lock();
  std::for_each(mSessions.begin(), mSessions.end(),
		boost::bind(&Session::kill, _1)); 
  mSessions.clear();
  mMutex.unlock();
}

void SessionManager::add(SessionPtr session)
{
  // don't proceed if already killed
  if (!mRunning)
    return;

  // add the session
  mMutex.lock();
  mSessions.insert(session);
  mMutex.unlock();
  printf("Session added\n");
}

void SessionManager::remove(SessionPtr session)
{
  // don't proceed if already killed
  if (!mRunning)
    return;

  // remove the session
  mMutex.lock();
  if (mSessions.count(session))
    mSessions.erase(session);
  mMutex.unlock();
  printf("Session removed\n");
}

void SessionManager::broadcast(MessagePtr msg)
{
  // don't proceed if already killed
  if (!mRunning)
    return;
    
  // append the message
  boost::lock_guard<boost::mutex> lock(mBMutex);
  mBroadcastQ.push_back(msg);

  // notify broadcast thread of new message
  mBCond.notify_one();
}

void SessionManager::listen()
{
  // keep listening while running
  while (mRunning)
    {
      // wait for messages
      printf("Listening for new broadcasts...\n");
      boost::unique_lock<boost::mutex> lock(mBMutex);
      while (mBroadcastQ.empty() && mRunning)
	mBCond.wait(lock);
      printf("Broadcast message received!\n");

      // messages to be sent so send them
      if (mRunning)
	performBroadcast();
    }
}

void SessionManager::performBroadcast()
{
  // gets the next message
  MessagePtr msg = mBroadcastQ.front();
  mBroadcastQ.pop_front();

  // broadcasts it to all sessions
  mMutex.lock();
  std::for_each(mSessions.begin(), mSessions.end(),
		boost::bind(&Session::write, _1, msg)); 
  mMutex.unlock(); 
  printf("Message broadcasted: %s\n", msg->body());
}
