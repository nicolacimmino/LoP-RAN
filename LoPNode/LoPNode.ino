// LopNode implements a LoP-RAN , a low power radio access network
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
// Tested on a pro-mini with ATMega168 5V @16MHz and a nano.
// 
// Connections:
//
// Warning! There are at least two types of radio modules, these pins are for a specific one used in protos.
//  Please use signal names and check your module pinout.
//
//  Radio  Signal  Arduino
//  1      GND     GND
//  2      VCC     3.3V
//  3      CE      9
//  4      CSN     10
//  5      CLK     13
//  6      MOSI    11
//  7      MISO    12//

#include <SPI.h>
#include <RF24.h>        // Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>, GNU
#include <EEPROM.h>
#include "Common.h"
#include "LoPDia.h"
#include "NetTime.h"
#include "DataLink.h"
#include "ACH.h"
#include "BCH.h"
#include "CCH.h"
#include "OuterNeighboursList.h"


//
// Change the following consts if your connections are arranged in different way.


const byte EEPROM_FOCAL_NODE = 0xFF;    // 0xFF  During development used to signal the fact that this node is a focal node (if set to 1)







// Indicates network status.
// TODO: change to enum we need more than 2 statuses.
boolean netStatus = false;





// Board setup.
void setup(void)
{
  // We sometimes still use serial for debuggin.
  // Keep high speeds and use sparingly as writing to serial puts off all timings.
  Serial.begin(115200);
  
  setupDataLink();
 
  // For testing only we fix the DAP so we have always the same
  //  node acting as AP and others in a star nework around it.
  //EEPROM.write(EEPROM_FOCAL_NODE,1);
  if(EEPROM.read(EEPROM_FOCAL_NODE) != 1)
  {
    lop_dap = 1;
  }
}



void loop(void)
{
  // During development we use this register to force a node
  //  to act as inner an other to act as outer.
  // In the final code it will be a serial command seeting
  //  the node as a focal if connected to an higher level controller.
  if(EEPROM.read(EEPROM_FOCAL_NODE) == 1)
  {  
      waitUntil((NetTime){-1, -1, 0, 0});
      broadcastBCH();
      
      waitUntil((NetTime){-1, -1, 1, 0});
      serveACH();
      
      for(int slot=2; slot<10; slot++)
      {
          waitUntil((NetTime){-1, -1, slot, 0});
          serveCCH();
      }
  }
  else
  {
      
      pinMode(2, OUTPUT);
        
 
      if(netStatus)
        waitUntil((NetTime){-1, -1, 0, 0});
        
        // Resync to network at every block. This corrects
        //  drifting caused by clock innacuracies while not
        //  being too heavy on the network. Note that time
        //  sync is a receive only operation so the fact that
        //  all nodes in the net will do it at the same time
        //  is no cause for congestion.
        if(!netStatus || isTime((NetTime){0,0,0,-1}))
        {
          innerNodeScanAndSync();
          
          // Wait slot 1 (ACH) and register if we are not already.
          if(!netStatus)
          {
            waitUntil((NetTime){-1, -1, 1, 10});
            netStatus = registerWithInnerNode();
          }
        }
        
        
        waitUntil(inboundTimeSlot);
        inititateCCHTransaction();
           
   }  
}





