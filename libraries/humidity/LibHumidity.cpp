/*
  LibHumidity - A Humidity Library for Arduino.

  Supported Sensor modules:
    SHT21-Breakout Module - http://www.moderndevice.com/products/sht21-humidity-sensor

  Created by Christopher Ladden at Modern Device on December 2009.
  modified by Paul Badger March 2010
  modified by Yoann Lapijover for the BeMap project on March 2015.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include <stdlib.h>
#include <inttypes.h>
#include <Wire.h>
#include "LibHumidity.h"

/******************************************************************************
 * Constructors
 ******************************************************************************/

/**********************************************************
 * Initialize the sensor based on the specified type.
 **********************************************************/
LibHumidity::LibHumidity(uint8_t sensorType = NULL) {
    Wire.begin();
    readDelay = 100;
}

/******************************************************************************
 * Global Functions
 ******************************************************************************/

 float LibHumidity::SetResolution10(void){
    Wire.beginTransmission(eSHT21Address);
    Wire.write(eWriteRegister);
    Wire.write(0b00000001);
    Wire.endTransmission();
 }

 short LibHumidity::getStatus(void){
    short vreturn;

    Wire.beginTransmission(eSHT21Address);
    Wire.write(eReadRegister);
    Wire.endTransmission();

    delay(10);

    Wire.requestFrom(eSHT21Address, 1);

    delay(10);

    vreturn = Wire.available() ? Wire.read() : 0;

    while(Wire.available())
      Wire.read();

    return vreturn;
 }

/**********************************************************
 * GetHumidity
 *  Gets the current humidity from the sensor.
 *
 * @return float - The relative humidity in %RH
 **********************************************************/
float LibHumidity::GetHumidity(void) {

    float humidity;

    humidity = calculateHumidity(readSensor(eRHumidityNoHoldCmd));

    return humidity;
}

/**********************************************************
 * GetTemperatureC
 *  Gets the current temperature from the sensor.
 *
 * @return float - The temperature in Deg C
 **********************************************************/
float LibHumidity::GetTemperatureC(void) {

    float temperature;

    temperature = calculateTemperatureC(readSensor(eTempNoHoldCmd));

    return temperature;
}


/**********************************************************
 * SetReadDelay
 *  Set the I2C Read delay from the sensor.
 *
 *  The SHT21 humidity sensor datasheet says:
 *  Parameter Resolution typ max Units
 *    14 bit      66        85      ms
 *    13 bit      33        43      ms
 *    12 Bit      17        22      ms
 *    11 bit       8        11      ms
 *    10 bit       4         6      ms
 *
 *      Measurement time
 *      (max values for -40°C
 *        125°C.)
 *      8 bit 1 3 ms
 *
 **********************************************************/

/******************************************************************************
 * Private Functions
 ******************************************************************************/

uint16_t LibHumidity::readSensor(uint8_t command) {

    uint16_t result;

    Wire.beginTransmission(eSHT21Address);
    Wire.write(command);
    Wire.endTransmission();

    delay(readDelay);

    Wire.requestFrom(eSHT21Address, 3);

    delay(1);

    if(Wire.available() < 3)
      return 0; //wait


    //Store the result
    result = ((Wire.read()) << 8);
    result += Wire.read();
    result &= ~0x0003;   // clear two low bits (status bits)
    return result;
}

void  LibHumidity::rollMeasure(void){
  static uint8_t i = 0;
  uint16_t result;
  switch(i){
    case 0:
      Wire.beginTransmission(eSHT21Address);
      Wire.write(eTempNoHoldCmd);
      Wire.endTransmission();
    break;

    case 1:
      Wire.requestFrom(eSHT21Address, 3);

      if(Wire.available() >= 3)
      {
        result = ((Wire.read()) << 8);
        result += Wire.read();
        result &= ~0x0003;   // clear two low bits (status bits)

        this->temp_last_sample = (uint16_t)(calculateTemperatureC(result)*100);
      }
    break;

    case 2:
      Wire.beginTransmission(eSHT21Address);
      Wire.write(eRHumidityNoHoldCmd);
      Wire.endTransmission();
    break;

    case 3:
      Wire.requestFrom(eSHT21Address, 3);

      if(Wire.available() >= 3)
      {
        result = ((Wire.read()) << 8);
        result += Wire.read();
        result &= ~0x0003;   // clear two low bits (status bits)

        this->hum_last_sample = (uint16_t)(calculateHumidity(result)*100);
      }
    break;
  }
  i++; i=i%4;
}

void LibHumidity::getLastSample(int* temp, int* hum){
  *temp = (int) this->temp_last_sample;
  *hum = (int) this->hum_last_sample;
}


float LibHumidity::calculateTemperatureC(uint16_t analogTempValue) {

  float st;
  float temperatureC;

st = analogTempValue;
  temperatureC =  (((175.72/65536.0) * (float)analogTempValue) - 46.85); //T= -46.85 + 175.72 * ST/2^16
  return temperatureC;
}


float LibHumidity::calculateHumidity(uint16_t analogHumValue) {

float srh = analogHumValue;
float humidityRH;                       // variable for result

//-- calculate relative humidity [%RH] --

 humidityRH = -6.0 + 125.0/65536.0 * srh;       // RH= -6 + 125 * SRH/2^16
 return humidityRH;
}


