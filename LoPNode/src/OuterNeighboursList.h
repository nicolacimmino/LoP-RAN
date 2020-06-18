// OuterNeighboursList is part of LoP-RAN , provides managing of the ONL.
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

#ifndef __OUTERNEIGHBOURSLIST_H
#define __OUTERNEIGHBOURSLIST_H

#include <Arduino.h>
#include "Common.h"
#include "LoPDia.h"
#include "LoPParams.h"
#include "DataLink.h"
#include "NetTime.h"

typedef struct
{
  byte tx_power;
  NetTime resourceMask;
  unsigned long last_seen;
  uint32_t address;
} ONDescriptor, *pONDescriptor;

extern pONDescriptor OuterNeighboursList[];

void clearOuterNeighboursList();
pONDescriptor allocateRadioResources(byte tx_power);
pONDescriptor getNeighbourDescriptor(NetTime time);
byte getActiveOuterNeighboursCount();

#endif
