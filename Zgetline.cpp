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

class Getline : public ZGetlineCallback { 
  public:
    Getline(void) : add_newline(true) { }

    bool add_newline;

    virtual bool operator()(const std::string &line) 
    {
      if (add_newline) cout << line << endl;
      else cout << line;

      return true;
    }
};

int main(void)
{
  ZInflate zinf;
  assert(zinf.get_init_code() == Zlib::Success);

  char buffer[MaxBufferSize];
  Getline getline;

  while (true) {
    int rd = read(0, buffer, MaxBufferSize);
    if (rd == 0) break;
    if (rd < 0) { 
      if (errno == EINTR || errno == EAGAIN) continue;
      cerr << "Blad czytania z stdin: " << errno << ", " << strerror(errno) << endl;
      return 1;
    }

    Zlib::Code code = zinf.perform(buffer, rd, getline);
    if (code != Zlib::Success && code != Zlib::Ready) {
      cerr << "Blad dekompresji: " << code << endl;
      return 1;
    }
  }
  
  getline.add_newline = false;

  getline.flush();

  if (!zinf.is_ready()) {
    cerr << "Dekomresja nie kompletna" << endl;
    return 1;
  }

  return 0;
}
