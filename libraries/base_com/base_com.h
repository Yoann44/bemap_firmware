#ifndef BASE_COM
#define BASE_COM

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <stdlib.h>

#define COMBINE(sentence_type, term_number) (((unsigned)(sentence_type) << 5) | term_number)

class Core_communication
{
public:

  Core_communication();
  bool encode(char c); // process one character received from GPS
  Core_communication &operator << (char c) {encode(c); return *this;}

protected:
  enum {SENTENCE_OTHER};

  byte _parity;
  bool _is_checksum_term;
  char _term[15];
  byte _sentence_type;
  byte _term_number;
  byte _term_offset;
  bool _data_good;

  // internal utilities
  int from_hex(char a);
  virtual bool term_complete() = 0;
  bool isdigit(char c) { return c >= '0' && c <= '9'; }
  int strcmp(const char *str1, const char *str2);
};

#if !defined(ARDUINO)
// Arduino 0012 workaround
#undef int
#undef char
#undef long
#undef byte
#undef float
#undef abs
#undef round
#endif

#endif
