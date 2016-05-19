/*
  Packet handler - A packet handler Library for Arduino.

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

#include "Bemap_packet.h"
#define OFFSET_LAT      0
#define OFFSET_LON      4
#define OFFSET_TIME     8
#define OFFSET_DATE     12
#define OFFSET_MONOXYDE 16
#define OFFSET_NITROGEN 18
#define OFFSET_TEMP     20
#define OFFSET_HUMIDITY 22
#define OFFSET_BOUNCE   24
#define OFFSET_TRACK    26

Bemap_packet::Bemap_packet()
{
    //ctor
}

Bemap_packet::~Bemap_packet()
{
    //dtor
}

void Bemap_packet::get_buf(char* buffer)
{
    if(buffer)
    {
        *((long*)(buffer + OFFSET_LAT)) = lattitude;
        *((long*)(buffer + OFFSET_LON)) = longitude;
        *((unsigned long*)(buffer + OFFSET_TIME)) = time;
        *((unsigned long*)(buffer + OFFSET_DATE)) = date;
        *((int*)(buffer + OFFSET_MONOXYDE)) = monoxyde;
        *((int*)(buffer + OFFSET_NITROGEN)) = nitrogen;
        *((int*)(buffer + OFFSET_TEMP)) = temperature;
        *((int*)(buffer + OFFSET_HUMIDITY)) = humidity;
        *((int*)(buffer + OFFSET_BOUNCE)) = bounce;
        *((int*)(buffer + OFFSET_TRACK)) = track_id;
    }
}

void Bemap_packet::set_from_buf(char* buffer)
{
    if(buffer)
    {
        lattitude = *((long*)(buffer + OFFSET_LAT));
        longitude = *((long*)(buffer + OFFSET_LON));
        time = *((unsigned long*)(buffer + OFFSET_TIME));
        date = *((unsigned long*)(buffer + OFFSET_DATE));
        monoxyde = *((int*)(buffer + OFFSET_MONOXYDE));
        nitrogen = *((int*)(buffer + OFFSET_NITROGEN));
        temperature = *((int*)(buffer + OFFSET_TEMP));
        humidity = *((int*)(buffer + OFFSET_HUMIDITY));
        bounce = *((int*)(buffer + OFFSET_BOUNCE));
        track_id = *((int*)(buffer + OFFSET_TRACK));
    }
}
