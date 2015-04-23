#ifndef __SESSION_MANAGER__
#define __SESSION_MANAGER__

#include <deque>
#include <set>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "Message.h"
#include "Session.h"

using boost::system::error_code;

class SessionManager
  : public boost::enable_shared_from_this<SessionManager>
{
 public:
  SessionManager();
  ~SessionManager();

  void kill();
  void add(SessionPtr session);
  void remove(SessionPtr session);
  void broadcast(MessagePtr msg);
  
 private:
  void listen();
  void performBroadcast();

  bool mRunning;
  boost::thread mThread;
  boost::mutex mMutex;
  boost::mutex mBMutex;
  boost::condition_variable mBCond;
  std::deque<MessagePtr> mBroadcastQ;
  std::set<SessionPtr> mSessions;
};

#endif
