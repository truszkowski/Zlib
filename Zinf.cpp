/*****************************************************************************
 *                                                                           *
 *            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE                    *
 *                    Version 2, December 2004                               *
 *                                                                           *
 * Copyright (C) 2004 Sam Hocevar                                            *
 *  14 rue de Plaisance, 75014 Paris, France                                 *
 * Everyone is permitted to copy and distribute verbatim or modified         *
 * copies of this license document, and changing it is allowed as long       *
 * as the name is changed.                                                   *
 *                                                                           *
 *            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE                    *
 *   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION         *
 *                                                                           *
 *  0. You just DO WHAT THE FUCK YOU WANT TO.                                *
 *                                                                           *
 *****************************************************************************/

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
  ZInflate zinf;
  assert(zinf.get_init_code() == Zlib::Success);

  char buffer[MaxBufferSize];
  Callback callback;

  while (true) {
    int rd = read(0, buffer, MaxBufferSize);
    if (rd == 0) break;
    if (rd < 0) { 
      if (errno == EINTR || errno == EAGAIN) continue;
      cerr << "read error from stdin: " << errno << ", " << strerror(errno) << endl;
      return 1;
    }

    Zlib::Code code = zinf.perform(buffer, rd, callback);
    if (code != Zlib::Success && code != Zlib::Ready) {
      cerr << "decompression error: " << code << endl;
      return 1;
    }
  }
  
  if (!zinf.is_ready()) {
    cerr << "incomplete decomression" << endl;
    return 1;
  }

  return 0;
}
