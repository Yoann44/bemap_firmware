#include "Tiny_gps.h"

#define _GPRMC_TERM   "GPRMC"
#define _GPGGA_TERM   "GPGGA"

Tiny_gps::Tiny_gps(unsigned short on_off_pin, unsigned short wake_up_pin)  :  _time(GPS_INVALID_TIME)
                                                                              ,  _date(GPS_INVALID_DATE)
                                                                              ,  _latitude(GPS_INVALID_ANGLE)
                                                                              ,  _longitude(GPS_INVALID_ANGLE)
                                                                              ,  _altitude(GPS_INVALID_ALTITUDE)
                                                                              ,  _last_time_fix(GPS_INVALID_FIX_TIME)
                                                                              ,  _last_position_fix(GPS_INVALID_FIX_TIME)
{
    this->on_off_pin = on_off_pin;
    this->wake_up_pin = wake_up_pin;
}

void Tiny_gps::init(void)
{
    Serial1.begin(4800);
    pinMode(on_off_pin, OUTPUT);
    pinMode(wake_up_pin, INPUT);
    digitalWrite(on_off_pin, LOW);
    delay(100);
    start();
}

bool Tiny_gps::term_complete()
{
  if (_is_checksum_term)
  {
    byte checksum = 16 * from_hex(_term[0]) + from_hex(_term[1]);
    if (checksum == _parity)
    {
      if (_data_good)
      {
        _last_time_fix = _new_time_fix;
        _last_position_fix = _new_position_fix;

        switch(_sentence_type)
        {
        case _GPS_SENTENCE_GPRMC:
              _time      = _new_time;
              _date      = _new_date;
              _latitude  = _new_latitude;
              _longitude = _new_longitude;
          break;
        case _GPS_SENTENCE_GPGGA:
              _time      = _new_time;
              _date      = _new_date;
              _latitude  = _new_latitude;
              _longitude = _new_longitude;
          break;
        }

        return true;
      }
      else if( _sentence_type == _GPS_SENTENCE_GPGGA || _sentence_type == _GPS_SENTENCE_GPRMC)
        _time = GPS_INVALID_TIME;
    }

    return false;
  }

  // the first term determines the sentence type
  if (_term_number == 0)
  {
    if (!strcmp(_term, _GPRMC_TERM))
      _sentence_type = _GPS_SENTENCE_GPRMC;
    else if (!strcmp(_term, _GPGGA_TERM))
      _sentence_type = _GPS_SENTENCE_GPGGA;
    else
      _sentence_type = _GPS_SENTENCE_OTHER;
    return false;
  }

  if (_sentence_type != _GPS_SENTENCE_OTHER && _term[0])
    switch(COMBINE(_sentence_type, _term_number))
  {
    case COMBINE(_GPS_SENTENCE_GPRMC, 1): // Time in both sentences
    case COMBINE(_GPS_SENTENCE_GPGGA, 1):
      _new_time = parse_decimal();
      _new_time_fix = 0;
      break;
    case COMBINE(_GPS_SENTENCE_GPRMC, 2): // GPRMC validity
      _data_good = _term[0] == 'A';
      break;
    case COMBINE(_GPS_SENTENCE_GPRMC, 3): // Latitude
    case COMBINE(_GPS_SENTENCE_GPGGA, 2):
      _new_latitude = parse_degrees();
      _new_position_fix = millis();
      break;
    case COMBINE(_GPS_SENTENCE_GPRMC, 4): // N/S
    case COMBINE(_GPS_SENTENCE_GPGGA, 3):
      if (_term[0] == 'S')
        _new_latitude = -_new_latitude;
      break;
    case COMBINE(_GPS_SENTENCE_GPRMC, 5): // Longitude
    case COMBINE(_GPS_SENTENCE_GPGGA, 4):
      _new_longitude = parse_degrees();
      break;
    case COMBINE(_GPS_SENTENCE_GPRMC, 6): // E/W
    case COMBINE(_GPS_SENTENCE_GPGGA, 5):
      if (_term[0] == 'W')
        _new_longitude = -_new_longitude;
      break;
    case COMBINE(_GPS_SENTENCE_GPGGA, 6): // Fix data (GPGGA)
      _data_good = _term[0] > '0';
      break;
    case COMBINE(_GPS_SENTENCE_GPGGA, 9): // Altitude (GPGGA)
      _new_altitude = parse_decimal();
      break;
    case COMBINE(_GPS_SENTENCE_GPRMC, 9): // Date (GPRMC)
      _new_date = gpsatol(_term);
      break;
  }

  return false;
}

// Parse a string in the form ddmm.mmmmmmm...
unsigned long Tiny_gps::parse_degrees(void)
{
  char *p;
  unsigned long left_of_decimal = gpsatol(_term);
  unsigned long hundred1000ths_of_minute = (left_of_decimal % 100UL) * 100000UL;
  for (p=_term; isdigit(*p); ++p);
  if (*p == '.')
  {
    unsigned long mult = 10000;
    while (isdigit(*++p))
    {
      hundred1000ths_of_minute += mult * (*p - '0');
      mult /= 10;
    }
  }
  return (left_of_decimal / 100) * 1000000 + (hundred1000ths_of_minute + 3) / 6;
}

unsigned long Tiny_gps::parse_decimal()
{
  char *p = _term;
  bool isneg = *p == '-';
  if (isneg) ++p;
  unsigned long ret = 100UL * gpsatol(p);
  while (isdigit(*p)) ++p;
  if (*p == '.')
  {
    if (isdigit(p[1]))
    {
      ret += 10 * (p[1] - '0');
      if (isdigit(p[2]))
        ret += p[2] - '0';
    }
  }
  return isneg ? -ret : ret;
}


long Tiny_gps::gpsatol(const char *str)
{
  long ret = 0;
  while (isdigit(*str))
    ret = 10 * ret + *str++ - '0';
  return ret;
}

// lat/long in MILLIONTHs of a degree and age of fix in milliseconds
// (note: versions 12 and earlier gave this value in 100,000ths of a degree.
void Tiny_gps::get_position(long *latitude, long *longitude, unsigned long *fix_age)
{
  if (latitude) *latitude = _latitude;
  if (longitude) *longitude = _longitude;
  if (fix_age) *fix_age = _last_position_fix == GPS_INVALID_FIX_TIME ?
   GPS_INVALID_AGE : millis() - _last_position_fix;
}

// date as ddmmyy, time as hhmmsscc, and age in milliseconds
void Tiny_gps::get_datetime(unsigned long *date, unsigned long *time)
{
  if (date) *date = _date;
  if (time) *time = _time;
}

long Tiny_gps::get_last_time_fix(void)
{
  return _last_position_fix == GPS_INVALID_FIX_TIME ? GPS_INVALID_AGE : millis() - _last_position_fix;
}

void Tiny_gps::hot_start(unsigned long elapsed_time)
{
    this->start();
}

void Tiny_gps::sleep(void)
{
    this->stop();

    _time = GPS_INVALID_TIME;
}

void Tiny_gps::toggle(void)
{
	digitalWrite(on_off_pin, HIGH);
	delay(200);
	digitalWrite(on_off_pin, LOW);
}

void Tiny_gps::start(void)
{
	while(!digitalRead(wake_up_pin))
	{
		delay(50);
		toggle();
    delay(100);
	}
}

void Tiny_gps::stop(void)
{
	if(digitalRead(wake_up_pin))
		toggle();
    delay(50);
}
