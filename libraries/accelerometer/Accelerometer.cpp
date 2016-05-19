#include <Arduino.h>
#include "Accelerometer.h"

#define CONVERSION_TO_G 0.0312

//ADXL345 Register Addresses
#define	DEVID			0x00	//Device ID Register
#define THRESH_TAP		0x1D	//Tap Threshold
#define	OFSX			0x1E	//X-axis offset
#define	OFSY			0x1F	//Y-axis offset
#define	OFSZ			0x20	//Z-axis offset
#define	DURATION		0x21	//Tap Duration
#define	LATENT			0x22	//Tap latency
#define	WINDOW			0x23	//Tap window
#define	THRESH_ACT		0x24	//Activity Threshold
#define	THRESH_INACT	0x25	//Inactivity Threshold
#define	TIME_INACT		0x26	//Inactivity Time
#define	ACT_INACT_CTL	0x27	//Axis enable control for activity and inactivity detection
#define	THRESH_FF		0x28	//free-fall threshold
#define	TIME_FF			0x29	//Free-Fall Time
#define	TAP_AXES		0x2A	//Axis control for tap/double tap
#define ACT_TAP_STATUS	0x2B	//Source of tap/double tap
#define	BW_RATE			0x2C	//Data rate and power mode control
#define POWER_CTL		0x2D	//Power Control Register
#define	INT_ENABLE		0x2E	//Interrupt Enable Control
#define	INT_MAP			0x2F	//Interrupt Mapping Control
#define	INT_SOURCE		0x30	//Source of interrupts
#define	DATA_FORMAT		0x31	//Data format control
#define DATAX0			0x32	//X-Axis Data 0
#define DATAX1			0x33	//X-Axis Data 1
#define DATAY0			0x34	//Y-Axis Data 0
#define DATAY1			0x35	//Y-Axis Data 1
#define DATAZ0			0x36	//Z-Axis Data 0
#define DATAZ1			0x37	//Z-Axis Data 1
#define	FIFO_CTL		0x38	//FIFO control
#define	FIFO_STATUS		0x39	//FIFO status

//ADXL345 Interrupt map
#define DATA_READY      0x80
#define SINGLE_TAP      0x40
#define DOUBLE_TAP      0x20
#define ACTIVITY        0x10
#define INACTIVITY      0x08
#define FREE_FALL       0x04
#define WATERMARK       0x02
#define OVERRUN         0x01

//ADXL range mode
#define RANGE_2G        0x00
#define RANGE_4G        0x01
#define RANGE_8G        0x02
#define RANGE_16G       0x03

//ADXL tap axes
#define TAP_Z           0x01
#define TAP_Y           0x02
#define TAP_X           0x04
#define TAP_NOT         0x08

//ADXL activity detection
#define ACT_AC          0x80
#define ACT_X           0x40
#define ACT_Y           0x20
#define ACT_Z           0x10
#define INACT_AC        0x08
#define INACT_X         0x04
#define INACT_Y         0x02
#define INACT_Z         0x01

enum{SLEEP, ACTIVE};

Accelerometer::Accelerometer(unsigned short cs_pin, unsigned short int_pin, int int_a_pin, void (*interrupt)())
{
    this->cs_pin = cs_pin;
    this->int_pin = int_pin;
    this->int_a_pin = int_a_pin;

    this->interrupt = interrupt;
}

Accelerometer::~Accelerometer()
{
    //dtor
}

void Accelerometer::init(Config* _config)
{
    this->bemap_config = _config;

    pinMode(int_pin, INPUT);
	//pinMode(cs_pin, OUTPUT);
	//digitalWrite(cs_pin, HIGH);
    DDRE |= (1<<2);
    PORTE |= (1<<2);
    /*
    this->sleep_int_mode();

    this->write_register(DATA_FORMAT, RANGE_16G);
    this->write_register(INT_MAP, 0xFF - INACTIVITY - ACTIVITY - SINGLE_TAP);            //Set to 0 for INT1
    this->write_register(TAP_AXES, TAP_Z);

    //Disable double tap detection
    this->write_register(LATENT, 0x00);
    this->write_register(WINDOW, 0x00);

    this->write_register(POWER_CTL, 0x08);

    this->int_start();
    */
}

short Accelerometer::new_int(void)
{
    char values[1], ret = NO_DETECT;

    this->read_register(INT_SOURCE, 1, values);

    if(values[0] & SINGLE_TAP)
    {
        this->bounce++;
        ret = SINGLE_DETECT;
    }

    if(values[0] & INACTIVITY)
    {
        ret = INACT_DETECT;
    }
    else if(values[0] & ACTIVITY)
    {
        ret = ACT_DETECT;
    }

    return values[0];
}

void Accelerometer::sleep_int_mode(void)
{
    //Active the activity detection
    this->write_register(INT_ENABLE, 0x00);
    this->write_register(THRESH_ACT, this->bemap_config->a_act_thresh);
    this->write_register(ACT_INACT_CTL, (ACT_AC | ACT_X | ACT_Y | ACT_Z));
    this->write_register(INT_ENABLE, ACTIVITY);

    char values[1];
    this->read_register(INT_SOURCE, 1, values);
}

void Accelerometer::usual_int_mode(void)
{
    //Active the inactivity and tap detection
    this->write_register(INT_ENABLE, 0x00);

    this->write_register(THRESH_INACT, this->bemap_config->a_inact_thresh);
    this->write_register(TIME_INACT, this->bemap_config->a_incat_time);

    this->write_register(THRESH_TAP, this->bemap_config->a_tap_thersh);
    this->write_register(DURATION, this->bemap_config->a_tap_time);

    this->write_register(ACT_INACT_CTL, (INACT_AC | INACT_X | INACT_Y | INACT_Z));
    this->write_register(INT_ENABLE, INACTIVITY | SINGLE_TAP);

    char values[1];
    this->read_register(INT_SOURCE, 1, values);

    this->state = ACTIVE;
}

void Accelerometer::int_start(void)
{
    char values[1];
    attachInterrupt(this->int_a_pin, this->interrupt, RISING);

    //Clear interrupt in ADXL345
    this->read_register(INT_SOURCE, 1, values);
}

void Accelerometer::int_stop(void)
{
    detachInterrupt(this->int_a_pin);
}

void Accelerometer::write_register(char registerAddress, char value)
{
	//digitalWrite(this->cs_pin, LOW);
    PORTE &= ~(1<<2);

	SPI.transfer(registerAddress);
	SPI.transfer(value);

	//digitalWrite(this->cs_pin, HIGH);
	PORTE |= (1<<2);
}

void Accelerometer::read_register(char registerAddress, int numBytes, char * values)
{
	char address = 0x80 | registerAddress;
	if(numBytes > 1)address = address | 0x40;

	//digitalWrite(this->cs_pin, LOW);
	PORTE &= ~(1<<2);

	SPI.transfer(address);
	for(int i=0; i<numBytes; i++)
	{
		values[i] = SPI.transfer(0x00);
	}

	//digitalWrite(this->cs_pin, HIGH);
	PORTE |= (1<<2);
}
