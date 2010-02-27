#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include <sys/time.h>

using namespace std;

int main(int argc, char **argv) 
{
  if (argc != 3) {
    cerr << "uzycie: " << argv[0] << " <nazwa-pliku> <wielkosc-pliku>" << endl;
    return 1;
  }

  string filename = argv[1];
  size_t filesize = atoi(argv[2]);

  ofstream f(filename.c_str());
  if (!f.is_open()) {
    cerr << "brak dostepu do pliku: " << filename << endl;
    return 1;
  }

  struct timeval tv;
  gettimeofday(&tv, 0);
  srand(tv.tv_usec);

  const size_t max_buflen = 4096;
  char buffer[max_buflen];

  while (filesize > 0) {
    size_t to_write = filesize < max_buflen ? filesize : max_buflen;

    for (size_t i = 0; i < max_buflen; ++i)
      buffer[i] = (rand() & 255);

    f.write(buffer, to_write);

    filesize -= to_write;
  }

  return 0;
}
