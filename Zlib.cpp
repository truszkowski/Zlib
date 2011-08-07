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


#include "Zlib.h"

#include <cassert>
#include <iostream>

using namespace std;

ZDeflate::ZDeflate(void) : Zlib()
{
  strm.zalloc = Z_NULL; // need to set before deflateInit2()
  strm.zfree = Z_NULL;  // --,,--
  strm.opaque = Z_NULL; // --,,--

  int ret = deflateInit2(&strm, 
      Z_DEFAULT_COMPRESSION, // = 6, also 0-9
      Z_DEFLATED,            // must be
      15+16,                 // window size to decompress (= max) + gzip
      8,                     // size of data in memory: 1-9
      Z_DEFAULT_STRATEGY);   // or Z_HUFFMAN_ONLY, Z_FILTERED, Z_FIXED

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
    // if no data to read, break 
    if (!finish && strm.avail_in == 0) break;

    strm.next_out = (Bytef*)chunk;
    strm.avail_out = MaxChunkSize;

    int ret = deflate(&strm, finish ? Z_FINISH : Z_NO_FLUSH);
    assert(ret != Z_STREAM_ERROR);
    size_t have = MaxChunkSize - strm.avail_out;

    if (!callback(chunk, have)) return Interupted;

    // continue while output buffer is full
  } while (strm.avail_out == 0);
  // ... input buffer must be empty
  assert(strm.avail_in == 0);

  return ready ? Ready:Success;
}

ZInflate::ZInflate(void) : Zlib()
{
  strm.zalloc = Z_NULL; // need to set before inflateInit2()
  strm.zfree = Z_NULL;  // --,,--
  strm.opaque = Z_NULL; // --,,--
  strm.avail_in = 0;    // --,,--
  strm.next_in = Z_NULL;// --,,--

  // window size to decompress (= max) + detect gzip or ...
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
    // if no data to read, break
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

    // continue while output buffer buffer is full 
    } while (strm.avail_out == 0);
  // ... input buffer must be empty
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

      // even it's empty line
      bool ret = this->operator()(current_line);
      current_line = "";

      if (!ret) return false;
    }
  }

  // if is something more to read, we will try later
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

