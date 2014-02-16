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

#ifndef __NETTIME_H__
#define __NETTIME_H__

#include <Arduino.h>

typedef struct
{
  int8_t slot;
  int8_t off; 
} NetTime, *pNetTime;


NetTime getInnerLinkNetworkTime(void);
void setInnerLinkNetworkTime(NetTime newTime);
bool timeMatchesMask(NetTime time, NetTime mask);
void waitUntilInnerLink(NetTime time);
bool isInnerLinkNetworkTime(NetTime mask);

#endif

