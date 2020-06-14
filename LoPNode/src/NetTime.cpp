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
#include "NetTime.h"
#include "LoPParams.h"
#include "ControlInterface.h"

// Stores the offset between the internal system clock
//  and the inner link network time.
long system_clock_offset = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
// Get the inner link network time
//
// Returns the current inner link network time with slot and off.
//
NetTime getInnerLinkNetworkTime(void)
{
  NetTime time;
  unsigned long timemS = (millis() - system_clock_offset) % (int)LOP_FRAMEDURATION;

  time.off = timemS % (int)LOP_SLOTDURATION;
  time.slot = (long)floor(timemS / LOP_SLOTDURATION) % (int)(LOP_FRAMEDURATION / LOP_SLOTDURATION);

  return time;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Set the inner link network time
//
// Sets the current inner link network time with slot and off.
//
void setInnerLinkNetworkTime(NetTime newTime)
{
  long wantedMillis = newTime.slot * LOP_SLOTDURATION + newTime.off;
  long previous_offset = system_clock_offset;
  system_clock_offset = (millis() % (int)LOP_FRAMEDURATION) - wantedMillis;
  dia_simpleFormNumericLog("CLKADJ", 1, previous_offset - system_clock_offset);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Waits until the specified slot and off is reached.
//
void waitUntilInnerLink(NetTime time)
{
  do
  {
    // We will sit in this loop most of the time we need to keep the
    //  control interface alive.
    serveControlInterface();
  } while (!isInnerLinkNetworkTime(time));
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Return true if the current network time matches the specified slot and off.
//
// Any element of the supplied time mask can be set to -1 in which case it will be ignored.
//
bool isInnerLinkNetworkTime(NetTime mask)
{
  return timeMatchesMask(getInnerLinkNetworkTime(), mask);
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Return true if the supplied network time matches the specified slot and off.
//
// Any element of the supplied time mask can be set to -1 in which case it will be ignored.
//
bool timeMatchesMask(NetTime time, NetTime mask)
{
  return (mask.slot == -1 || time.slot == mask.slot) &&
         (mask.off == -1 || time.off >= mask.off);
}
