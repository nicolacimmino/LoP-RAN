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
#include "LoPParams.h"
#include "LoPDia.h"
#include "NetTime.h"
#include "DataLink.h"
#include "ACH.h"
#include "BCH.h"
#include "CCH.h"
#include "OuterNeighboursList.h"
#include "ControlInterface.h"


// Board setup.
void setup(void)
{
  // We sometimes still use serial for debuggin.
  // Keep high speeds and use sparingly as writing to serial puts off all timings.
  Serial.begin(115200);
 
  setupDataLink();
  setupControlInterface();
}



void loop(void)
{
  
  // Currently we support only star network so we broadcast the
  //  BCH and serve ACH only if DAP=0
  if(lop_dap == 0)
  {  
      waitUntilInnerLink((NetTime){0, 0});
      broadcastBCH();
      
      waitUntilInnerLink((NetTime){1, 0});
      serveACH();
      
      for(int slot=2; slot<10; slot++)
      {
          waitUntilInnerLink((NetTime){slot, 0});
          serveCCH();
      }
  }
  else
  {
      pinMode(2, OUTPUT);
        
   
        if(!inner_link_up)
        {
          // Attempt to find an inner node and, if found
          //  sync to the inner link net time. This call blocks
          //  until a node is found.
          innerNodeScanAndSync();
          
          // Access the found inner node and get the link up.
          waitUntilInnerLink((NetTime){1, 10});
          inner_link_up = registerWithInnerNode();
        }
        else
        {
          waitUntilInnerLink(inboundTimeSlot);
          inititateCCHTransaction();
          //waitUntilInnerLink((NetTime){inboundTimeSlot.slot, LOP_SLOTDURATION-2});
          delay(100);
          
          if(tx_error_count > LOP_MAX_TX_ERROR)
          {
            inner_link_up = false;
          }
        }
           
   }  
}





