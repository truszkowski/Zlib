#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include <unistd.h>

#include "Zlib.h"

using namespace std;

const size_t MaxBufferSize = 16384;

class Callback : public Zlib::Callback {
  public:
    Callback(void) { }

    bool operator()(char *buffer, size_t buflen)
    {
      while (buflen > 0) {
        int wr = write(1, buffer, buflen);
        if (wr <= 0) {
          if (errno == EINTR || errno == EAGAIN) continue;
          return false;
        }
        buffer += wr;
        buflen -= wr;
      }
      return true;
    }
};

int main(void)
{
  ZDeflate zdef;
  assert(zdef.get_init_code() == Zlib::Success);

  char buffer[MaxBufferSize];
  Callback callback;

  while (true) {
    int rd = read(0, buffer, MaxBufferSize);
    if (rd < 0) { 
      if (errno == EINTR || errno == EAGAIN) continue;
      cerr << "Blad czytania z stdin: " << errno << ", " << strerror(errno) << endl;
      return 1;
    }

    if (rd == 0) {
      Zlib::Code code = zdef.perform(buffer, rd, callback, true);
      if (code != Zlib::Ready) {
        cerr << "Blad kompresji: " << code << endl;
        return 1;
      }
      break;
    } else {
      Zlib::Code code = zdef.perform(buffer, rd, callback);
      if (code != Zlib::Success) {
        cerr << "Blad kompresji: " << code << endl;
        return 1;
      }    
    }
  }
  
  assert(zdef.is_ready());
  return 0;
}
