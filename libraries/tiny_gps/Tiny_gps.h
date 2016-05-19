#ifndef TINY_GPS_H
#define TINY_GPS_H

#include "../base_com/base_com.h"

enum {
    GPS_INVALID_AGE = 0xFFFFFFFF,      GPS_INVALID_ANGLE = 999999999,
    GPS_INVALID_ALTITUDE = 999999999,  GPS_INVALID_DATE = 0,
    GPS_INVALID_TIME = 0xFFFFFFFF,
    GPS_INVALID_FIX_TIME = 0xFFFFFFFF
};

class Tiny_gps : public Core_communication
{
    public:
        Tiny_gps(unsigned short on_off_pin, unsigned short wake_up_pin);
        // lat/long in MILLIONTHs of a degree and age of fix in milliseconds
        // (note: versions 12 and earlier gave lat/long in 100,000ths of a degree.
        void get_position(long *latitude, long *longitude, unsigned long *fix_age = 0);

        // date as ddmmyy, time as hhmmsscc, and age in milliseconds
        void get_datetime(unsigned long *date, unsigned long *time);

        long get_last_time_fix(void);

        long get_time(void){return _time;}

        void init(void);
        void hot_start(unsigned long elapsed_time);
        void sleep(void);

    protected:
        enum {_GPS_SENTENCE_GPGGA, _GPS_SENTENCE_GPRMC, _GPS_SENTENCE_OTHER};

        unsigned long _time, _new_time;
        unsigned long _date, _new_date;
        long _latitude, _new_latitude;
        long _longitude, _new_longitude;
        long _altitude, _new_altitude;

        unsigned long _last_time_fix, _new_time_fix;
        unsigned long _last_position_fix, _new_position_fix;

        unsigned short on_off_pin;
        unsigned short wake_up_pin;

        long gpsatol(const char *str);
        unsigned long parse_decimal(void);
        unsigned long parse_degrees(void);

        bool term_complete(void);

        void start(void);
        void stop(void);
        void toggle(void);
};

#endif // TINY_GPS_H
