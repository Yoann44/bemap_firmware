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

#include "gaz_sensor.h"
#include "../device/config.h"

Gaz_sensor::Gaz_sensor(){
  index = 0;
}

void  Gaz_sensor::start(){
    digitalWrite(GAS_ON_OFF, LOW);
}

void  Gaz_sensor::stop(){
    digitalWrite(GAS_ON_OFF, HIGH);
}

void  Gaz_sensor::mean(int* co, int* no2){
    long mean_co = 0;
    long mean_no2 = 0;

    for (int i = 0; i < GAZ_BUFFER_SIZE; i++)
    {
        mean_co += buffer[GAZ_CO][i];
        mean_no2 += buffer[GAZ_NO2][i];
    }

    *co = (int) (mean_co/GAZ_BUFFER_SIZE);
    *no2 = (int) (mean_no2/GAZ_BUFFER_SIZE);
}

void  Gaz_sensor::init(){
    pinMode(GAS_ON_OFF, OUTPUT);
    this->stop();
}

void  Gaz_sensor::measure(){
    buffer[GAZ_NO2][index] = analogRead(DIOXYDE);
    buffer[GAZ_CO][index] = analogRead(MONOXYDE);

    index++;

    if(index >= GAZ_BUFFER_SIZE)
      index = 0;
}

void  Gaz_sensor::get_last_data(int* co, int* no2){
    *no2 = buffer[GAZ_NO2][index];
    *co = buffer[GAZ_CO][index];
}

void  Gaz_sensor::get_measure(int* co, int* no2){
    this->mean(co, no2);
}
