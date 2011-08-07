#ifndef __MY_ZLIB_H__
#define __MY_ZLIB_H__

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

#include <string>
#include <zlib.h>

class Zlib {
  public:
    enum Code {
      Interupted        =  2, // interupted compresion
      Ready             =  1, // ready
      Success           =  0, // everything ok
      ZlibDataError     = -1, // data problem
      ZlibBufError      = -2, // too small buffer
      ZlibMemError      = -3, // not enough memory
      ZlibStreamError   = -4, // invalid arguments
      ZlibVersionError  = -5, // invalid version 
      ZlibOtherError    = -6  // other error
    };
 
    /* Callback for reading data */
    struct Callback {
      /**
       * @brief you have to define this method in your class, 
       *
       * @param buffer  data after compression/decompression
       * @param buflen  size of data
       *
       * @return true - continue, everything ok
       *         false - break, some error occurs
       */
      virtual bool operator()(char *buffer, size_t buflen) = 0;
    };

    /* maximum count of bytes to read once */
    static const size_t MaxChunkSize = 16384;

    Zlib(void) : init_code(Success), ready(false) { }
    virtual ~Zlib(void) { }

    /**
     * @brief it's initialized?
     *
     * @return success, otherwise error
     */
    virtual Code get_init_code(void) const { return init_code; }

    /**
     * @brief it's finished?
     *
     * @return true/false
     */
    virtual bool is_ready(void) const { return ready; }

  protected:
    z_stream strm;
    Code init_code;
    char chunk[MaxChunkSize];
    bool ready;

    Zlib(const Zlib &); // non-copyable
};

/* to compress */
class ZDeflate : public Zlib {
  public:
    ZDeflate(void);
    ~ZDeflate(void);
  
    /**
     * @brief commpress input data with that callback function
     *
     * @param input     data to compress
     * @param len       size of that data
     * @param callback  callback, gets commpresed chunks 
     * @param finish    true == it's done, function should return Ready code
     *
     * @param Success - everything ok (in progress), 
     *        Ready - compression ended, 
     *        Interupted - some callback() returns false,
     *        otherwise error
     */
    Code perform(char *input, size_t len, Callback &callback, bool finish = false);

  protected:
};

/* to decompress */
class ZInflate : public Zlib {
  public:
    ZInflate(void);
    ~ZInflate(void);

    /**
     * @brief decompress input data with that callback function 
     *
     * @param input     data to decompress
     * @param len       size of that data
     * @param callback  callback, gets decompressed chunks
     *
     * @param Success - everything ok (in progress)
     *        Ready - decompression ended, 
     *        Interupted - some callback() returns false,
     *        otherwise error
     */
    Code perform(char *input, size_t len, Callback &callback);

  protected:
};

/* to get decompressed line */
class ZGetlineCallback : public ZInflate::Callback {
  public:
    ZGetlineCallback(void) { }

    /**
     * @brief you have to define this method in your class,
     *        end of line is recognized by: '\0' and '\n',
     *        will skipped in returned lines
     *
     * @param line  decompressed line
     *
     * @return true - continue, false - break
     */
    virtual bool operator()(const std::string &line) = 0;

    /**
     * @brief after decompress is posible some data will not printed by 
     *        callback function - when last line doesn't ended by newline -
     *        if any data remained, flush() will call callback() with that
     *        data
     *
     * @return true/false - just like operator()
     */
    virtual bool flush(void);

  private:
    std::string current_line; // last read line

    virtual bool operator()(char *buffer, size_t buflen);
};

#endif

