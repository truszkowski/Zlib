#ifndef __MY_ZLIB_H__
#define __MY_ZLIB_H__

#include <string>
#include <zlib.h>

class Zlib {
  public:
    enum Code {
      Interupted        =  2, // Przerwano kompresje
      Ready             =  1, // Gotowe
      Success           =  0, // Wszystko OK
      ZlibDataError     = -1, // Blad danych
      ZlibBufError      = -2, // Za maly bufor na dane
      ZlibMemError      = -3, // Za malo pamieci
      ZlibStreamError   = -4, // Bledne parametry
      ZlibVersionError  = -5, // Zla wersja zliba
      ZlibOtherError    = -6  // Jakis inny blad
    };
 
    /* Callback na odczytane dane */
    struct Callback {
      /**
       * @brief Funkcja do zdefiniowania w podklasach, funkcja dostaje w
       * parametrach odczytane dane po kompresji/dekompresji.
       *
       * @param buffer  dane po kompresji/dekompresji
       * @param buflen  rozmiar tych danych
       *
       * @return true - kontunuuj kompresje/dekompresje,
       *         false - przerwij kompresje/dekompresje
       */
      virtual bool operator()(char *buffer, size_t buflen) = 0;
    };

    /* Maksymalny liczba bajtow do odczytania na raz */
    static const size_t MaxChunkSize = 16384;

    Zlib(void) : init_code(Success), ready(false) { }
    virtual ~Zlib(void) { }

    /**
     * @brief Czy zainicjowano poprawnie dane
     *
     * @return Success, wpp blad
     */
    virtual Code get_init_code(void) const { return init_code; }

    /**
     * @brief Czy kompresja/dekompresja jest juz zakonczona
     *
     * @return true/false
     */
    virtual bool is_ready(void) const { return ready; }

  protected:
    z_stream strm;
    Code init_code;
    char chunk[MaxChunkSize];
    bool ready;

    Zlib(const Zlib &); // uniemozliwiamy kopiowanie obiektu
};

/* Do kompresji */
class ZDeflate : public Zlib {
  public:
    ZDeflate(void);
    ~ZDeflate(void);
  
    /**
     * @brief Skompresuj dane wejsciowe z funkcja callback
     *
     * @param input     dane do kompresji
     * @param len       rozmiar tych danych
     * @param callback  funkcja dostajaca juz skompresowane dane
     * @param finish    true == konczymy kompresje, funkcja powinna wtedy
     *                  zwrocic kod Ready
     *
     * @param Success -ok, Ready -kompresja zakonczona, Interupted -w callback()
     *        przerwano kompresje, wpp blad
     */
    Code perform(char *input, size_t len, Callback &callback, bool finish = false);

  protected:
};

/* Do dekompresji */
class ZInflate : public Zlib {
  public:
    ZInflate(void);
    ~ZInflate(void);

    /**
     * @brief Zdekompresuj dane wejscione z funkcja callback
     *
     * @param input     dane do dekompresowania
     * @param len       rozmiar tych danych
     * @param callback  funkcja dostajaca juz zdekompresowane dane
     *
     * @param Success -ok, Ready -zdekompresowano, Interupted -w callback()
     *        przerwano dekompresje, wpp blad
     */
    Code perform(char *input, size_t len, Callback &callback);

  protected:
};

/* do pobierania zdekompresowanej linni  */
class ZGetlineCallback : public ZInflate::Callback {
  public:
    ZGetlineCallback(void) { }

    /**
     * @brief Do zdefiniowana w podklasie, jako argument dostaje linie z
     * zdekompresowanego wyjscia. Pomijane sa znaki '\0' i '\n', ich wystapinie
     * w tekscie jest interpretowane jako koniec linni. 
     *
     * @param line  linia
     *
     * @return true - kontutuuj, false - przerwij dekompresje
     */
    virtual bool operator()(const std::string &line) = 0;

    /**
     * @brief Po zakonczonej kompresji moga jeszcze pozostac jakies dane do
     * odczytania. Wywolanie flush() uruchomi this->operator()(current_line)
     * jesli jakies dane istotnie pozostaly w pamieci.
     *
     * @return true/false - to co zwroci wywolane this->operator()(line) 
     */
    virtual bool flush(void);

  private:
    std::string current_line; // ostatnio wczytana linna

    // do wewnetrznego uzytku, odpowiednio odczytuje dane wolajac
    // this->operator(current_line) gdy zostanie wlasnie wczytana nowa linnia.
    virtual bool operator()(char *buffer, size_t buflen);
};

#endif

