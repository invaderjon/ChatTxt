#ifndef __SESSION_MANAGER__
#define __SESSION_MANAGER__

#include <deque>
#include <set>
#include <boost/mutex.hpp>
#include <boost/thread.hpp>
#include "Message.h"
#include "Session.h"

class SessionManager
{
 public:
  SessionManager();
  ~SessionManager();

  void kill();
  void register(SessionPtr session);
  void unregister(SessionPtr session);
  void broadcast(MessagePtr msg);
  
 private:
  void listen();
  void performBroadcast();

  boost::thread mThread;
  boost::mutex mMutex;
  boost::mutex mBMutex;
  boost::condition_variable mBCond;
  std::deque<MessagePtr> mBroadcastQ;
  std::set<SessionPtr> mSessions;
  bool mRunning;
};

#endif
