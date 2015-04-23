#include "Message.h"

Message::Message()
  : mRaw(HeaderLength, '\0'), mBody('\0', 1)
{
}

Message::Message(char* body, int length)
  : mRaw(HeaderLength, '\0'), mBody('\0', length)
{
  // trim to max size if necessary
  if (length > MaxLength)
    {
      mBody.resize(MaxLength);
      length = MaxLength - 1;
    }
  
  memcpy(&mBody[0], body, length);
}

Message::~Message()
{
}

char* Message::raw()
{
  return &mRaw[0];
}

int Message::rawLength()
{
  return mRaw.size();
}

char* Message::rawBody()
{
  if (mRaw.size() > HeaderLength)
    return &mRaw[HeaderLength];
  return NULL;
}

int Message::rawBodyLength()
{
  return mRaw.length() - HeaderLength;
}

char* Message::body()
{
  if (mBody.size() > 0)
    return &mBody[0];
  return NULL;
}

int Message::length()
{
  return mBody.size();
}

bool Message::encode()
{
  using namespace std;
  
  // maybe eventually base64 encode
  char* encoded = &mBody[0];
  int len = mBody.length();

  // resize the raw string appropriately
  mRaw.resize(HeaderLength + len);

  // encodes the header
  char header[HeaderLength + 1] = "";
  sprintf(header, "%4d", len);
  memcpy(&mRaw[0], header, HeaderLength);

  // copies the encoded message
  if (len > 0)
    memcpy(&mRaw[HeaderLength], encoded, len);
}

bool Message::decode()
{
  using namespace std;

  // maybe eventually base64 encode
  char* decoded = &mRaw[HeaderLength];
  int length = mRaw.length() - HeaderLength;

  // resizes the body to the appropriate size
  mBody.resize(length);

  // copies the data to the body
  if (length > 0)
    memcpy(&mBody[0], decoded, length);
}

bool Message::decodeHeader()
{
  // extracts the header
  char header[HeaderLength + 1] = "";
  strncat(header, &mRaw[0], HeaderLength);

  // resizes the raw buffer to the specified size
  mRaw.resize(atoi(header) + HeaderLength);
}
