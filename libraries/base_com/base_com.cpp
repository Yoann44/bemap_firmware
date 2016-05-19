#include "base_com.h"

#define COMBINE(sentence_type, term_number) (((unsigned)(sentence_type) << 5) | term_number)

Core_communication::Core_communication()
  :  _parity(0)
  ,  _is_checksum_term(false)
  ,  _sentence_type(SENTENCE_OTHER)
  ,  _term_number(0)
  ,  _term_offset(0)
  ,  _data_good(false)
{
  _term[0] = '\0';
}

bool Core_communication::encode(char c)
{
  bool valid_sentence = false;

  switch(c)
  {
  case ',': // term terminators
    _parity ^= c;
  case '\r':
  case '\n':
  case '*':
    if (_term_offset < sizeof(_term))
    {
      _term[_term_offset] = 0;
      valid_sentence = term_complete();
    }
    ++_term_number;
    _term_offset = 0;
    _is_checksum_term = c == '*';
    return valid_sentence;

  case '$': // sentence begin
    _term_number = _term_offset = 0;
    _parity = 0;
    _sentence_type = SENTENCE_OTHER;
    _is_checksum_term = false;
    _data_good = false;
    return valid_sentence;
  }

  // ordinary characters
  if (_term_offset < sizeof(_term) - 1)
    _term[_term_offset++] = c;
  if (!_is_checksum_term)
    _parity ^= c;

  return valid_sentence;
}

int Core_communication::from_hex(char a)
{
  if (a >= 'A' && a <= 'F')
    return a - 'A' + 10;
  else if (a >= 'a' && a <= 'f')
    return a - 'a' + 10;
  else
    return a - '0';
}

int Core_communication::strcmp(const char *str1, const char *str2)
{
  while (*str1 && *str1 == *str2)
    ++str1, ++str2;
  return *str1;
}



