#ifndef __MESSAGE__
#define __MESSAGE__

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

class Message
{
 public:
  Message();
  Message(char* body, int length);
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
  enum { MaxLength = 129 }; // max length is 128 visible characters
  
 private:
  std::vector<char> mRaw;
  std::vector<char> mBody;
};

#endif
