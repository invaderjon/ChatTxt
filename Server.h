#ifndef __SERVER__
#define __SERVER__

class Server
{
 public:
  Server(int port);
  ~Server();
  void start();
  void stop();

 private:
  void initConnMgr();
  void initBroadcaster();
  void listen();

  bool mRunning;
  int mPort;
};

#endif
