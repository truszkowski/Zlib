#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include <sys/time.h>

using namespace std;

const string chars = "abcdefghijklmnopqrstuvuwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789`~!@#$%^&*()_+-={}|[]\\:\";'<>?,./ ";
const size_t charc = chars.length();

int main(int argc, char **argv) 
{
  if (argc != 3) {
    cerr << "uzycie: " << argv[0] << " <nazwa-pliku> <wielkosc-pliku>" << endl;
    return 1;
  }

  const string filename = argv[1];
  const size_t filesize = atoi(argv[2]);

  ofstream f(filename.c_str());
  if (!f.is_open()) {
    cerr << "brak dostepu do pliku: " << filename << endl;
    return 1;
  }

  struct timeval tv;
  gettimeofday(&tv, 0);
  srand(tv.tv_usec);

  for (size_t i = 1; i <= filesize; ++i) {
    if (!(rand() % 80)) f << endl;
    else f << chars[rand() % charc];
  }

  return 0;
}
