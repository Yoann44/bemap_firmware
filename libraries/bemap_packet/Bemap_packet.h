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

#ifndef BEMAP_PACKET_H
#define BEMAP_PACKET_H

#define PACKET_SIZE 32


class Bemap_packet
{
    public:
        Bemap_packet();
        virtual ~Bemap_packet();

        long Getlattitude() { return lattitude; }
        void Setlattitude(long val) { lattitude = val; }
        long Getlongitude() { return longitude; }
        void Setlongitude(long val) { longitude = val; }
        unsigned long Gettime() { return time; }
        void Settime(unsigned long val) { time = val; }
        unsigned long Getdate() { return date; }
        void Setdate(unsigned long val) { date = val; }
        int Gethumidity() { return humidity; }
        void Sethumidity(int val) { humidity = val; }
        int Gettemperature() { return temperature; }
        void Settemperature(int val) { temperature = val; }
        int Getmonoxyde() { return monoxyde; }
        void Setmonoxyde(int val) { monoxyde = val; }
        int Getnitrogen() { return nitrogen; }
        void Setnitrogen(int val) { nitrogen = val; }
        int Getbounce() { return bounce; }
        void Setbounce(int val) { bounce = val; }
        int Gettrack() { return track_id; }
        void Settrack(int val) { track_id = val; }

        void get_buf(char* buffer);
        void set_from_buf(char* buffer);

    protected:
    private:
        long lattitude;
        long longitude;
        unsigned long time;
        unsigned long date;
        int humidity;
        int temperature;
        int monoxyde;
        int nitrogen;
        int bounce;
        int track_id;
};

#endif // BEMAP_PACKET_H
