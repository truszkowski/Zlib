#include "Zlib.h"

#include <cassert>
#include <iostream>

using namespace std;

ZDeflate::ZDeflate(void) : Zlib()
{
  strm.zalloc = Z_NULL; // te pola musimy ustawic przed deflateInit2()
  strm.zfree = Z_NULL;  // j.w.
  strm.opaque = Z_NULL; // j.w.

  int ret = deflateInit2(&strm, 
      Z_DEFAULT_COMPRESSION, // = 6, mozna podawac z przedzialu 0-9
      Z_DEFLATED,            // musi byc
      15+16,                 // rozmiar okna do kompresji(= maks.) + gzip
      8,                     // wielkosc danych trzymanych w pamieci: 1-9
      Z_DEFAULT_STRATEGY);   // lub Z_HUFFMAN_ONLY, Z_FILTERED, Z_FIXED

  switch (ret) {
    case Z_OK:
      init_code = Success;
      break;
    case Z_MEM_ERROR:
      init_code = ZlibMemError;
      break;
    case Z_STREAM_ERROR:
      init_code = ZlibOtherError;
      break;
    default:
      init_code = ZlibOtherError;
  }
}

ZDeflate::~ZDeflate(void)
{
  if (init_code == Success) 
    deflateEnd(&strm);
}

ZDeflate::Code ZDeflate::perform(char *input, size_t len, 
    ZDeflate::Callback &callback, bool finish)
{
  if (init_code != Success) return ZlibBufError;
  if (ready) return Ready;
  if (finish) ready = true;

  strm.next_in = (Bytef*)input;
  strm.avail_in = len;

  do {
    // Jesli brak danych do przerobienia to wychodzimy
    if (!finish && strm.avail_in == 0) break;

    strm.next_out = (Bytef*)chunk;
    strm.avail_out = MaxChunkSize;

    int ret = deflate(&strm, finish ? Z_FINISH : Z_NO_FLUSH);
    assert(ret != Z_STREAM_ERROR);
    size_t have = MaxChunkSize - strm.avail_out;

    if (!callback(chunk, have)) return Interupted;

    // Kontunuujemy dopoki wynikowy bufor jest pelny
  } while (strm.avail_out == 0);
  // ... bufor wejsciowy musi byc juz pusty
  assert(strm.avail_in == 0);

  return ready ? Ready:Success;
}

ZInflate::ZInflate(void) : Zlib()
{
  strm.zalloc = Z_NULL; // te pola musimy ustawic przed inflateInit2()
  strm.zfree = Z_NULL;  // j.w.
  strm.opaque = Z_NULL; // j.w.
  strm.avail_in = 0;    // j.w.
  strm.next_in = Z_NULL;// j.w.

  // rozmiar okna do dekompresji (= maks.) + rozpoznawanie czy gzip czy co...
  int ret = inflateInit2(&strm, 15 + 32);

  switch (ret) {
    case Z_OK:
      init_code = Success;
      break;
    case Z_MEM_ERROR:
      init_code = ZlibMemError;
      break;
    case Z_VERSION_ERROR:
      init_code = ZlibVersionError;
      break;
    case Z_STREAM_ERROR:
      init_code = ZlibOtherError;
      break;
    default:
      init_code = ZlibOtherError;
  }
}

ZInflate::~ZInflate(void)
{
  if (init_code == Success) 
    inflateEnd(&strm);
}

ZInflate::Code ZInflate::perform(char *input, size_t len, ZInflate::Callback &callback)
{
  if (init_code != Success) return ZlibBufError;
  if (ready) return Ready;
 
  strm.next_in = (Bytef*)input;
  strm.avail_in = len;

  do { 
    // Jesli brak danych do przerobienia to wychodzimy
    if (strm.avail_in == 0) break;

    strm.next_out = (Bytef*)chunk;
    strm.avail_out = MaxChunkSize;

    int ret = inflate(&strm, Z_NO_FLUSH);
    assert(ret != Z_STREAM_ERROR);

    if (ret == Z_STREAM_END) {
      ready = true;
    } else if (ret != Z_OK) {
      switch (ret) {
        /*case Z_BUF_ERROR:*/
        case Z_NEED_DICT:
        case Z_DATA_ERROR:
          return ZlibDataError;
        case Z_MEM_ERROR:
          return ZlibMemError;
        default:
          return ZlibOtherError;
      }
    }

    size_t have = MaxChunkSize - strm.avail_out;
    if (!callback(chunk, have)) return Interupted;

    // Kontynuujemy dopoki wynikowy bufor jest pelny
  } while (strm.avail_out == 0);
  // ... bufor wejsciowy musi byc juz pusty
  assert(strm.avail_in == 0);

  return ready ? Ready:Success;
}


bool ZGetlineCallback::operator()(char *buffer, size_t buflen)
{
  size_t bufpos = 0;

  while (bufpos < buflen) {
    if (buffer[bufpos] != '\0' &&
        buffer[bufpos] != '\n') ++bufpos;
    else {
      if (bufpos > 0) 
        current_line.append(buffer, 0, bufpos);

      buffer += bufpos + 1;
      buflen -= bufpos + 1;
      bufpos = 0;

      // Przekazujemy, nawet jesli bedzie to pusta linnia
      bool ret = this->operator()(current_line);
      current_line = "";

      if (!ret) return false;
    }
  }

  // Jesli czegos nie przeczytalismy, odkladamy na pozniej
  if (buflen > 0) 
    current_line.append(buffer, 0, buflen);

  return true;
}

bool ZGetlineCallback::flush(void)
{
  bool ret = true;

  if (current_line != "") {
    ret = this->operator()(current_line);
    current_line = "";
  }

  return ret;
}

