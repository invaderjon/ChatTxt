#ifndef __MESSAGE__
#define __MESSAGE__

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <stdio.h>
#include <boost/shared_ptr.hpp>

class Message
{
 public:
  Message();
  Message(const std::string& str);
  ~Message();
  
  char* raw();
  int rawLength();

  char* rawBody();
  int rawBodyLength();

  char* body();
  int length();

  bool encode();
  bool decode();

  bool decodeHeader();

  enum { HeaderLength = 4 };
  enum { MaxLength = 128 }; // max length is 128 visible characters
  
 private:
  std::vector<char> mRaw;
  std::vector<char> mBody;
};

typedef boost::shared_ptr<Message> MessagePtr;

#endif
