/*
  NO2/CO gaz sensor - A NO2/CO gaz sensor Library for Arduino.

  Supported Sensor modules:
    MICS-4514 - http://www.moderndevice.com/products/sht21-humidity-sensor

  Created by Yoann Lapijover for BeMap ptoject on March 2016.

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


#ifndef GAZ_SENSOR_H
#define GAZ_SENSOR_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define GAZ_BUFFER_SIZE 10

#define GAZ_NO2 0
#define GAZ_CO  1

class Gaz_sensor
{
  private:
    int buffer[2][GAZ_BUFFER_SIZE];
    int index;
    void  mean(int* co, int* no2);

  public:
    Gaz_sensor();
    
    // Initialize the I/O ports for the gaz sensor
    void  init();
    // Add a measure to the buffer at the current index
    void  measure();
    //  Return the last data in the buffer
    void  get_last_data(int* co, int* no2);
    // Return the mean value of data stored in the buffer
    void  get_measure(int* co, int* no2);
    // Start the alimentation of the sensor
    void  start();
    // Stop the alimentation of the sensor
    void  stop();
};

#endif
