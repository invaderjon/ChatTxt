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
		{
		printf("%s\n", msg->body());
      	if(0==strncmp(msg->body(),"/banksy",7))
			{
					printf("%s\n", "                       .s$$$Ss.");
					printf("%s\n", "            .8,         $$$. _. .              ..sS$$$$$'  ...,.;");
					printf("%s\n", "o.   ,@..  88        =.$'$'  '          ..sS$$$$$$$$$$$$s. _;''");
					printf("%s\n", "  @@@.@@@. .88.   `  ` ""l. .sS$$.._.sS$$$$$$$$$$$$S'''");
					printf("%s\n", "   .@@@q@@.8888o.         .s$$$$$$$$$$$$$$$$$$$$$'");
					printf("%s\n", "     .:`@@@@33333.       .>$$$$$$$$$$$$$$$$$$$$'");
					printf("%s\n", "     .: `@@@@333'       ..>$$$$$$$$$$$$$$$$$$$'");
					printf("%s\n", "      :  `@@333.     `.,   s$$$$$$$$$$$$$$$$$'");
					printf("%s\n", "      :   `@33       $$ S.s$$$$$$$$$$$$$$$$$'");
					printf("%s\n", "      .S   `Y      ..`  ,'$' `$$$$$$$$$$$$$$");
					printf("%s\n", "      $s  .       ..S$s,    . .`$$$$$$$$$$$$.");
					printf("%s\n", "      $s .,      ,s ,$$$$,,sS$s.$$$$$$$$$$$$$.");
					printf("%s\n", "      / /$$SsS.s. ..s$$$$$$$$$$$$$$$$$$$$$$$$$.");
					printf("%s\n", "     /`.`$$$$$dN.ssS$$'`$$$$$$$$$$$$$$$$$$$$$$$.");
					printf("%s\n", "    /     `$$$$$$$$$'    `$$$$$$$$$$$$$$$$$$$$$$.");
					printf("%s\n", "   /  |     `S$$S$'       `$$$$$$$$$$$$$$$$$$$$$$.");
					printf("%s\n", "  / /                      $$$$$$$$$$$$$$$$$$$$$.");
					printf("%s\n", "                           `$$$$$$$$$$$$$$$$$$$$$s.");
					printf("%s\n", "                            $$$''        .?T$$$$$$$");
					printf("%s\n", "                           .$'        ...      ?$$# ");
					printf("%s\n", "                           !       -=S$$$$$s");
					printf("%s\n", "                         .!       -=s$$'  `$=-_      :");
					printf("%s\n", "                        ,        .$$$'     `$,       .|");
					printf("%s\n", "                       ,       .$$$'          .        ,");
					printf("%s\n", "                      ,     ..$$$'");
					printf("%s\n", "                          .s$$$'                 `s     .");
					printf("%s\n", "                   .   .s$$$$'                    $s. ..$s");
					printf("%s\n", "                  .  .s$$$$'                      `$s=s$$$");
					printf("%s\n", "                    .$$$$'                         ,    $$s");
			}
      	if(0==strncmp(msg->body(),"/shrek",6))
			{
					printf("%s\n", "                      _____");
					printf("%s\n", "                   ,-'     `._");
					printf("%s\n", "                 ,'           `.        ,-. ");
					printf("%s\n", "               ,'               \       ),.\ ");
					printf("%s\n", "     ,.       /                  \     /(  \; ");
					printf("%s\n", "    /'\\     ,o.        ,ooooo.   \  ,'  `-') ");
					printf("%s\n", "    )) )`. d8P'Y8.    ,8P'''''Y8.  `'  .--'' ");
					printf("%s\n", "   (`-'   `Y'  `Y8    dP       `'     / ");
					printf("%s\n", "    `----.(   __ `    ,' ,---.       ( ");
					printf("%s\n", "           ),--.`.   (  ;,---.        )");
					printf("%s\n", "          / \O_,' )   \  \O_,'        | ");
					printf("%s\n", "         ;  `-- ,'       `---'        | ");
					printf("%s\n", "         |    -'         `.           | ");
					printf("%s\n", "        _;    ,            )          : ");
					printf("%s\n", "     _.'|     `.:._   ,.::' `..       | ");
					printf("%s\n", "  --'   |   .'     ""'         `      |`.");
					printf("%s\n", "        |  :;      :   :     _.       |`.`.-'--. ");
					printf("%s\n", "        |  ' .     :   :__.,'|/       |  \ ");
					printf("%s\n", "        `     \--.__.-'|_|_|-/        /   ) ");
					printf("%s\n", "         \     \_   `--^'__,'        ,    | ");
					printf("%s\n", "   -hrr- ;  `    `--^---'          ,'     | ");
					printf("%s\n", "          \  `                    /      / ");
					printf("%s\n", "           \   `    _ _          /");
					printf("%s\n", "            \           `       / ");
					printf("%s\n", "             \           '    ,' ");
					printf("%s\n", "              `.       ,   _,' ");
					printf("%s\n", "                `-.___.---' ");
			}

		}
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
