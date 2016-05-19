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


#ifndef STATE_CONTROLLER_H
#define STATE_CONTROLLER_H

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "../device/device.h"
#include "../device/config.h"

enum Button_state { TO_SLEEP, TO_TRACK, TRACK_AND_LIGHT};

class State_controller
{
  private:
    enum  Button_state  button;

    Devices* devices;
    Config* bemap_config;

    long timeout;

    float battery_voltage();

    void  lamp_on();
    void  lamp_off();

    bool  read_button();
    float  read_battery();

    void  to_state_sleeping();
    void  to_state_no_fix();
    void  to_state_tracking();
    void  to_state_fix_lost();

  public:
    State_controller();
    
    // Initialize the I/O ports for the button, the LED and the LAMP
    void  init(Config* main_config, Devices* main_devices);
    // Update the state if needed
    void  update_state();
};

#endif
