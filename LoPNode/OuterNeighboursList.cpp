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

#include "Arduino.h"
#include "Common.h"
#include "LoPDia.h"
#include "LoPParams.h"
#include "DataLink.h"
#include "OuterNeighboursList.h"
#include "NetTime.h"



pONDescriptor OuterNeighboursList[LOP_MAX_OUT_NEIGHBOURS];

void clearOuterNeighboursList()
{
  for(int ix=0; ix<LOP_MAX_OUT_NEIGHBOURS; ix++)
    OuterNeighboursList[ix] = 0;
}

// Allocates radio resources to serve an outer neighbour.
pONDescriptor allocateRadioResources(byte tx_power)
{
  // Find the first free time slot
  for(int ix=0; ix<LOP_MAX_OUT_NEIGHBOURS; ix++)
  {
    if(OuterNeighboursList[ix] == 0 || (millis() - OuterNeighboursList[ix]->last_seen) > LOP_ONL_ALLOCATION_TTL)
    {
      if(OuterNeighboursList[ix] == 0)
        OuterNeighboursList[ix] = new ONDescriptor();
      OuterNeighboursList[ix]->tx_power = tx_power;
      OuterNeighboursList[ix]->resourceMask.off = -1;
      OuterNeighboursList[ix]->resourceMask.slot = ix + 2; 
      OuterNeighboursList[ix]->resourceMask.frame = -1;
      OuterNeighboursList[ix]->resourceMask.block = -1;
      OuterNeighboursList[ix]->last_seen = millis();
      OuterNeighboursList[ix]->address[0] = ix+1;
      OuterNeighboursList[ix]->address[1] = 0;
      OuterNeighboursList[ix]->address[2] = 0;
      OuterNeighboursList[ix]->address[3] = 0;
      OuterNeighboursList[ix]->address[4] = 0;
      OuterNeighboursList[ix]->address[5] = 0;
      OuterNeighboursList[ix]->address[6] = 0;
      OuterNeighboursList[ix]->address[7] = 0;
      
      return OuterNeighboursList[ix];
    }
  }
}

pONDescriptor getNeighbourDescriptor(NetTime time)
{
  // Find a neighbour allocated for this time slot.
  for(int ix=0; ix<LOP_MAX_OUT_NEIGHBOURS; ix++)
  {
    if(timeMatchesMask(time, (*OuterNeighboursList[ix]).resourceMask))
    {
      return OuterNeighboursList[ix];
    }
  }
  
  // No neighbour alloted here.
  return 0;
}


