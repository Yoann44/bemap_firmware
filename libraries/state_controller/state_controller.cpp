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

#include <config.h>
#include "state_controller.h"

State_controller::State_controller(){

}

void State_controller::init(Config* main_config, Devices* main_devices){
    this->bemap_config = main_config;
    this->devices = main_devices;

    //New arduino : INPUT_PULLUP
    pinMode(BUTTON_1, INPUT_PULLUP);
    pinMode(BUTTON_2, INPUT_PULLUP);
    pinMode(LAMP, OUTPUT);

    this->lamp_off();
    devices->rg_led.set_color(C_OFF);

    //Active internal pull-up
    //digitalWrite(BUTTON_1, HIGH);
    //digitalWrite(BUTTON_2, HIGH);

    this->button = TO_SLEEP;

    this->bemap_config->actual_state = SLEEPING;
    this->bemap_config->track_id = this->devices->memory.get_track_id();
    this->bemap_config->device_id = this->devices->memory.get_device_id();
}
void State_controller::update_state(void){

    bool button_change = this->read_button();
    
    this->read_battery();
    bemap_config->memory_state = devices->memory.get_memory_state();

    switch(this->bemap_config->actual_state){

        case SLEEPING:
            if( button_change && this->button != TO_SLEEP )
                this->to_state_no_fix();
        break;

        case NO_FIX:
            if( button_change && this->button == TO_SLEEP )
                this->to_state_sleeping();
            else if( devices->gps.get_time() != GPS_INVALID_TIME && millis() - timeout >= this->bemap_config->preheat_time)
                this->to_state_tracking();
        break;

        case TRACKING:
            if( button_change && this->button == TO_SLEEP )
                this->to_state_sleeping();
            else if( devices->gps.get_time() == GPS_INVALID_TIME )
                this->to_state_fix_lost();
        break;

        case FIX_LOST:
            if( button_change && this->button == TO_SLEEP )
                this->to_state_sleeping();
            else if( devices->gps.get_time() != GPS_INVALID_TIME)
                this->to_state_tracking();
            else if( millis() - timeout > this->bemap_config->fix_lost_time_out )
                this->to_state_no_fix();
        break;
/*
        default:
            devices->rg_led.set_blink_color(C_RED);
            devices->rg_led.set_blink_freq(300);
            devices->rg_led.set_blink_time(0);
        break;
*/
    }

    if( this->bemap_config->actual_state != SLEEPING && this->button == TRACK_AND_LIGHT )
        this->lamp_on();
    else
        this->lamp_off();
}

void  State_controller::lamp_on(){
    digitalWrite(LAMP, HIGH);
}

void  State_controller::lamp_off(){
    digitalWrite(LAMP, LOW);
}

bool  State_controller::read_button(){
    enum Button_state old_but_state = this->button;

    int but_state;
    but_state = digitalRead(BUTTON_1);
    but_state += 2 * digitalRead(BUTTON_2);

    switch(but_state){
        case 3:
            this->button = TO_SLEEP;
        break;

        case 2:
            this->button = TRACK_AND_LIGHT;
        break;

        case 1:
            this->button = TO_TRACK;
        break;
    }

    if(but_state != old_but_state)
    {
        /*
        Serial.print("BUT : ");
        Serial.print(but_state, DEC);
        Serial.print(" - ");
        Serial.println(old_but_state, DEC);
        */

        return true;
    }

    return false;

}

float  State_controller::read_battery(){
    /*
    long voltage = analogRead(BAT_VOLTAGE);
    voltage -= MIN_BAT_VOLTAGE;
    voltage = voltage * 100 / (MAX_BAT_VOLTAGE - MIN_BAT_VOLTAGE);
    */
    int voltage = ((analogRead(BAT_VOLTAGE) - 532) * 10 ) / 12;
    voltage = voltage > 0 ? voltage : 0;
    voltage = voltage < 100 ? voltage : 0;
    bemap_config->battery_state = voltage;
}

void  State_controller::to_state_sleeping(){
    this->devices->memory.save_status();
    //this->devices->accel.sleep_int_mode();
    this->devices->gps.sleep();
    this->devices->sensor.stop();
    this->lamp_off();
    this->bemap_config->actual_state = SLEEPING;

    devices->rg_led.set_blink_freq(0);
    this->devices->rg_led.set_color(C_OFF);
    this->devices->rg_led.update();
}

void  State_controller::to_state_no_fix(){
    if(this->bemap_config->actual_state == SLEEPING)
    {
        this->timeout = millis();

        devices->rg_led.set_blink_color(C_ORANGE);
        devices->rg_led.set_blink_freq(300);
        devices->rg_led.set_blink_time((int) bemap_config->preheat_time/300);
        
        this->devices->gps.hot_start(0);
        //this->devices->accel.usual_int_mode();
        this->devices->sensor.start();
    }

    this->bemap_config->actual_state = NO_FIX;

    this->devices->rg_led.set_color(C_ORANGE);
    this->devices->rg_led.update();
}

void  State_controller::to_state_tracking(){
    if(this->bemap_config->actual_state == NO_FIX)
    {
        this->bemap_config->track_id++;
        //this->devices->accel.reset_bounce();
        this->devices->memory.set_track_id(this->bemap_config->track_id);
    }

    this->bemap_config->actual_state = TRACKING;

    this->devices->rg_led.set_color(C_GREEN);
    this->devices->rg_led.update();
}

void  State_controller::to_state_fix_lost(){
    this->bemap_config->actual_state = FIX_LOST;

    timeout = millis();

    this->devices->rg_led.set_color(C_RED);
    this->devices->rg_led.update();
}
