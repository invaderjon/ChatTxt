#include "Server.h"

int main(int argc, char** argv)
{
  if (argc <= 1)
    return -1;
  
  boost::asio::io_service service;
  tcp::endpoint endpoint(tcp::v4(), atoi(argv[1]));
  Server server(service, endpoint);
  server.start();
  service.run();
}
