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

#include "rg_led.h"

rg_led_driver::rg_led_driver()
{
	remaining_time = 0;
	last_call = 0;
}

void rg_led_driver::init()
{
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    color = C_OFF;
    blink_color = C_OFF;
    blink_rate = 0;
    update();
}

void rg_led_driver::update()
{
	static enum Color actual_color = C_OFF;
	if(blink_rate > 0)
	{
		remaining_time -= millis() - last_call;
		last_call = millis();
		if(remaining_time <= 0)
		{
			static bool state = false;
			if(state)
			{
				color_driver(C_OFF);
				actual_color = C_OFF;
				state = false;
			}
			else
			{
				color_driver(blink_color);
				actual_color = blink_color;
				state = true;
			}

			if(blink_time >= 2)
				blink_time--;
			else if(blink_time == 1)
			{
				blink_rate = 0;
			}
			remaining_time = blink_rate;
		}
	}
	else
	{
		if(actual_color != color)
			color_driver(color);

		actual_color = color;
	}
	
}

void rg_led_driver::color_driver(enum Color _color)
{
	switch(_color){
		case C_OFF:
			led_controller( 0, 0 );
		break;

		case C_GREEN:
			led_controller( 255, 0 );
		break;

		case C_ORANGE:
			led_controller( 255, 130 );
		break;

		case C_RED:
			led_controller( 0, 255 );
		break;
	}
}

void rg_led_driver::led_controller(int green, int red)
{
	if(green <= 255 && green >= 0)
		analogWrite(LED_GREEN, green);
	if(red <= 255 && red >= 0)
		analogWrite(LED_RED, red);
}