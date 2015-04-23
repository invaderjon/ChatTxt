#include <string>
#include <iostream>
#include "Message.h"
#include "Client.h"

#define LOCALHOST "127.0.0.1"
#define DEF_PORT "8080"

int main(int argc, char** argv)
{
  if (argc <= 1)
  {
  	printf("Usage: ./server [server address] [port]\n");
    printf("[server address] defaults to localhost if unspecified\n");
    return 0;
  }

  std::string addr(LOCALHOST);
  std::string port(DEF_PORT);

  if (argc <= 2)
  {
    addr = LOCALHOST;
    port = std::string(argv[1]);
  }
  else
  {
    addr = std::string(argv[1]);
    port = std::string(argv[2]);
  }
 
  boost::asio::io_service service;

  tcp::resolver resolver(service);
  tcp::resolver::query query(addr, port);
  tcp::resolver::iterator iterator = resolver.resolve(query);

  Client client(service, iterator);

  boost::thread t(boost::bind(&boost::asio::io_service::run, &service));

  MessagePtr msg(new Message("Hello world!"));  
  client.write(msg);

  std::string input; 
  while (true)
  {
    getline(std::cin, input);
    msg = MessagePtr(new Message(input));
    client.write(msg);
  }

  t.join();
  return 0;
}
