#include "SessionManager.h"

SessionManager::SessionManager()
  : mRunning(true),
    mThread(boost::bind(&SessionManager::listen,
			shared_from_this()))
{
}

SessionManager::~SessionManager()
{
}

void SessionManager::kill()
{
  // kills listener thread
  mRunning = false;
  mBCond.notify_one();
  mThread.join();

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
      boost::unique_lock<boost::mutex> lock(mBMutex);
      while (mBroadcastQ.empty() && mRunning)
	mBCond.wait(lock);

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
}
