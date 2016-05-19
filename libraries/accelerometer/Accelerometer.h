#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

#include <SPI.h>

#ifndef DEVICE_2_H
#define DEVICE_2_H
#include "../device/device.h"
#undef DEVICE_2_H
#endif

#define NO_DETECT       0
#define ACT_DETECT      1
#define INACT_DETECT    2
#define SINGLE_DETECT   3

class Accelerometer
{
    public:

        Accelerometer(unsigned short cs_pin, unsigned short int_pin, int int_a_pin, void (*interrupt)());
        ~Accelerometer();

        unsigned int get_bounce() { return bounce; }

        void init(Config* _config);
        void reset_bounce(void) { bounce = 0; }
        short new_int(void);
        void sleep_int_mode(void);
        void usual_int_mode(void);
        void int_start(void);
        void int_stop(void);

    protected:
    private:
        Config* bemap_config;

        /*volatile*/ unsigned int bounce;

        unsigned short state;
        unsigned short cs_pin;
        unsigned short int_pin;
        int int_a_pin;
        void (*interrupt)();

        void write_register(char registerAddress, char value);
        void read_register(char registerAddress, int numBytes, char * values);
};

#endif // ACCELEROMETER_H
