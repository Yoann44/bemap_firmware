/*
  Configuration file for the BeMap Hardware v0.3

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

#ifndef CONFIG_H
#define CONFI_H

//#define ACCL_CS     24
#define ACCL_INT    7
#define ACCL_A_INT  4

#define FLASH_H     10
#define FLASH_W     8
#define FLASH_CS    9

#define GPS_ON_OFF  6
#define GPS_WAKE_UP 4

#define LED_GREEN   13
#define LED_RED     5
#define BUTTON_1    23
#define BUTTON_2    11
#define LAMP        18
#define BAT_VOLTAGE A4

#define MONOXYDE    A1
#define DIOXYDE     A2
#define GAS_ON_OFF  21

#define BUTTON_STATE_STOP   3
#define BUTTON_STATE_LED    2
#define BUTTON_STATE_TRACK  1

#define LIPO_MIN  34
#define LIPO_MAX  44
#define ALIM_REG  33

#define MIN_BAT_VOLTAGE LIPO_MIN*1024/ALIM_REG/2
#define MAX_BAT_VOLTAGE LIPO_MAX*1024/ALIM_REG/2

#define DEFAULT_SLEEP_RATE		10000
#define DEFAULT_SENSOR_RATE		20
#define DEFAULT_SENSIRION_RATE	200
#define DEFAULT_TRACKING_RATE	2000
#define DEFAULT_NO_FIX_TIME_OUT 120000
#define DEFAULT_PREHEAT_TIME    30000

#define DEFAULT_A_TAP_THERSH    0x6A
#define DEFAULT_A_TAP_TIME      0x10
#define DEFAULT_A_ACT_THERSH    0x20
#define DEFAULT_A_INACT_THERSH  0x10
#define DEFAULT_A_INACT_TIME    0x28


#define INVALID_DATA        -1
#define INVALID_DEVICE_ID		-1
#define INVALID_TRACK_ID		-1

#define FW_VERSION          17

#endif // CONFIG_H
