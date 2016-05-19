/*
  Red/Green LED driver - A Red/Green LED driver Library for Arduino.

  Supported components :
    Name - Website

  Created by Yoann Lapijover for BeMap ptoject on April 2016.

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


#ifndef RG_LED_H
#define RG_LED_H

#include "../device/config.h"

#if defined(ARDUINO) && ARDUINO >= 100
  #include <Arduino.h>
#else
  #include <WProgram.h>
#endif

#include <inttypes.h>

enum Color {C_RED, C_GREEN, C_ORANGE, C_OFF};

class rg_led_driver
{
  private:
    enum  Color blink_color;
    enum  Color color;
    bool  blink;
    int   blink_rate;
    int   blink_time;

    long   remaining_time;
    long  last_call;

    void  led_controller(int green, int red);
    void  color_driver(enum Color _color);

  public:
    rg_led_driver();

    void  init();
    void  set_color(enum Color _color) { color = _color; }
    void  set_blink_color(enum Color _color) { blink_color = _color; }
    void  set_blink_freq(int _blink_rate) { blink_rate = _blink_rate; remaining_time = _blink_rate; }
    void  set_blink_time(int _blink_time) { blink_time = _blink_time; }
    void  update();
};

#endif
