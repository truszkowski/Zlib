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
