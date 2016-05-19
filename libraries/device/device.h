/*
  Handle the components and configuration in BeMap hardware v0.3

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

#ifndef DEVICE_1_H
#define DEVICE_1_H

enum Device_state { SLEEPING, NO_FIX , TRACKING, FIX_LOST };

typedef struct config {
  long sleep_time_ms;
  int tracking_time_ms;

  char battery_state;
  char memory_state;

  int sensor_sampling_rate;
  int sensirion_sampling_rate;

  int device_id;
  int track_id;
  int fw_version;

  long fix_lost_time_out;
  long preheat_time;

  char a_tap_thersh;
  char a_tap_time;
  char a_act_thresh;
  char a_inact_thresh;
  char a_incat_time;

  enum Device_state actual_state;
} Config;
#endif

#ifndef DEVICE_2_H
#define DEVICE_2_H

#include "../accelerometer/Accelerometer.h"
#include "../tiny_gps/Tiny_gps.h"
#include "../flash/Flash.h"
#include "../humidity/LibHumidity.h"
#include "../gaz_sensor/gaz_sensor.h"
#include "../rg_led/rg_led.h"
#include "../bemap_packet/Bemap_packet.h"

extern void accl_handler(void);

typedef struct devices
{
    Accelerometer accel = Accelerometer( 0, ACCL_INT, ACCL_A_INT, NULL );
    F_memory memory = F_memory( PACKET_SIZE, FLASH_H, FLASH_W, FLASH_CS );
    LibHumidity sensirion = LibHumidity(0);
    Tiny_gps gps = Tiny_gps(GPS_ON_OFF, GPS_WAKE_UP);
    Gaz_sensor sensor = Gaz_sensor();
    rg_led_driver rg_led = rg_led_driver();
} Devices;

#endif