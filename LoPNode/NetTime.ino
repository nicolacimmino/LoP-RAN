// NetTime is part of LoP-RAN , provides network time tracking.
//  Copyright (C) 2014 Nicola Cimmino
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see http://www.gnu.org/licenses/.
//

long off_off = 0;


NetTime getNetworkTime(void)
{
  NetTime time;
  unsigned long timemS = millis() - off_off;
  
  time.off = timemS % 100;
  time.slot = (timemS / 100) % 10;
  time.frame = (timemS / 1000 )  % 10;
  time.block = (timemS / 10000)  % 6;
  
  return time;
}

//void setNetworkTime(byte block, byte frame, byte slot)
void setNetworkTime(NetTime newTime)
{
  unsigned long wantedMillis = newTime.block * 10000l + newTime.frame * 1000l + newTime.slot * 100l; 
  off_off = (millis() % 60000) - wantedMillis;
}

void waitUntil(NetTime time)
{
  do
  {
  } while(!isTime(time));
}

bool isTime(NetTime time)
{
  NetTime currentTime = getNetworkTime();
  return   (time.block == -1 || currentTime.block == time.block) &&
           (time.frame == -1 || currentTime.frame== time.frame) &&
           (time.slot == -1 || currentTime.slot== time.slot) &&
           (time.off == -1 || currentTime.off== time.off);
}

