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

#include "LoPDia.h"

NetTime getNetworkTime(void)
{
  NetTime time;
  unsigned long timemS = (millis() - off_off) % 60000;
  
  time.off = timemS % 100;
  time.slot = (long)floor(timemS / 100.0f) % 10;
  time.frame = (long)floor(timemS / 1000.0f)  % 10;
  time.block = (long)floor(timemS / 10000.0f)  % 6;
  
  return time;
}

//void setNetworkTime(byte block, byte frame, byte slot)
void setNetworkTime(NetTime newTime)
{
  long wantedMillis = newTime.block * 10000l + newTime.frame * 1000l + newTime.slot * 100l + newTime.off; 
  long previous_off_off = off_off;
  off_off = (millis() % 60000) - wantedMillis;
  dia_simpleFormNumericLog("CLKADJ", 1, previous_off_off - off_off);
}

void tuneNetwrokTime(int8_t off)
{
  int8_t off_tune = (getNetworkTime().off - off);
  off_off += off_tune;
  dia_simpleFormNumericLog("CLKTUN", 1, off_tune);
}

void waitUntil(NetTime time)
{
  do
  {
  } while(!isTime(time));
}

bool isTime(NetTime mask)
{
  return timeMatchesMask(getNetworkTime(), mask);
}

bool timeMatchesMask(NetTime time, NetTime mask)
{
  return   (mask.block == -1 || time.block == mask.block) &&
           (mask.frame == -1 || time.frame == mask.frame) &&
           (mask.slot == -1 || time.slot == mask.slot) &&
           (mask.off == -1 || time.off >= mask.off);
}
