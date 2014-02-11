// ACH is part of LoP-RAN , provides control for the Access CHannel.
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
// LoP-RAN Specifications are available at https://github.com/nicolacimmino/LoP-RAN/wiki
//    This source code referes, where apllicable, to the chapter and 
//    sub chapter of these documents.

#include "Arduino.h"
#include <EEPROM.h>
#include "Common.h"
#include "LoPDia.h"
#include "LoPParams.h"
#include "DataLink.h"
#include "NetTime.h"
#include "OuterNeighboursList.h"
#include "ACH.h"


///////////////////////////////////////////////////////////////////////////////////////////////
// Attempts to register with an inner node in order to get radio resources allocted.
//  The call can block for up to 3 frames if a succesfull REGACK is not received.
// Must be called at OFF=0 of SLOT=1 of on any frame of a node that is not registered yet with
//  an inner node only after having found a BCH.
//
boolean registerWithInnerNode()
{
  // Get a random number to minimize chances of collisions during registration.
  // Randomizing with uS since start should be enough since the time at which we
  //  come here is highly variable (power on reset, BCH broadcast time at least).
  randomSeed(micros());
  
  for(int retry=0; retry<LOP_REG_MAX_RETRY; retry++)
  {
    byte randToken = random(0,255);
       
    // Build the the REG message according to LOP_01.01§6.1
    lop_tx_buffer[LOP_IX_SDU_ID] = LOP_SDU_REG;
    lop_tx_buffer[LOP_IX_SDU_REG_POW] = inbound_tx_power;
    lop_tx_buffer[LOP_IX_SDU_REG_TOKEN] = randToken;
    
    // Setup the ACH phy layer parameters according to LOP_01.01§4        
    radio.setPALevel((rf24_pa_dbm_e)inbound_tx_power);
    radio.openWritingPipe(ACH_PIPE_ADDR_IN);
    radio.openReadingPipe(1, ACH_PIPE_ADDR_OUT);
    
    // Introduce a guard to accomodate for sligh drift according to LOP_01.01§6
    delay(LOP_RTXGUARD);
    
    sendLoPRANMessage(lop_tx_buffer, LOP_LEN_SDU_REG);
    
    // Wait a reply, expect  a REGACK with the same token, ignore anything else
    //  according to according to LOP_01.01§6.2
    radio.startListening();
    if(receiveLoPRANMessage(lop_rx_buffer, LOP_MTU , LOP_SLOTDURATION))
    {
      if(lop_rx_buffer[LOP_IX_SDU_ID] == LOP_SDU_REGACK && lop_rx_buffer[LOP_IX_SDU_REGACK_TOKEN] == randToken)
      {
        // Store the inbound link time slot assigned to us.
        inboundTimeSlot.block = lop_rx_buffer[LOP_IX_SDU_REGACK_BLOCK];
        inboundTimeSlot.frame = lop_rx_buffer[LOP_IX_SDU_REGACK_FRAME];
        inboundTimeSlot.slot = lop_rx_buffer[LOP_IX_SDU_REGACK_SLOT];
        return true;
      }
    }

    // We failed to receive a REGACK, we just wait the next ACH to retry.
    waitUntil((NetTime){-1, (getNetworkTime().frame + 1) % LOP_FRAMES_PER_BLOCK, 1, -1});    
  }
  
  // We failed more than LOP_REG_MAX_RETRY, give up. 
  return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Serves requests on the ACH.
// Must be called at OFF=0 of SLOT=1 of on any frame of a node that broadcasts a BCH. The call
//  might return before the end of SLOT 1 but never after.
//
void serveACH()
{
  // Setup the ACH phy layer parameters according to LOP_01.01§4        
  radio.openWritingPipe(ACH_PIPE_ADDR_OUT);  
  radio.openReadingPipe(1, ACH_PIPE_ADDR_IN);
  radio.setChannel(50);
  radio.startListening();
   
  if(receiveLoPRANMessage(lop_rx_buffer, LOP_MTU , LOP_SLOTDURATION))
  {
    // Expect a REG message.
    if(lop_rx_buffer[LOP_IX_SDU_ID] == LOP_SDU_REG)
    {
      // Allocate radio resources and store them in the ONL.
      ONDescriptor neighbourDescriptor = allocateRadioResources(lop_rx_buffer[LOP_IX_SDU_REG_POW]);

      // Build the the REG message according to LOP_01.01§6.2
      lop_tx_buffer[LOP_IX_SDU_ID] = LOP_SDU_REGACK;
      lop_tx_buffer[LOP_IX_SDU_REGACK_TOKEN] = lop_rx_buffer[LOP_IX_SDU_REG_TOKEN];        // TOKEN
      lop_tx_buffer[LOP_IX_SDU_REGACK_BLOCK] = (neighbourDescriptor).resourceMask.block;  // RMBLOCK
      lop_tx_buffer[LOP_IX_SDU_REGACK_FRAME] = (neighbourDescriptor).resourceMask.frame;  // RMFRAME
      lop_tx_buffer[LOP_IX_SDU_REGACK_SLOT] = (neighbourDescriptor).resourceMask.slot;   // RMSLOT
      lop_tx_buffer[LOP_IX_SDU_REGACK_ALEN] = 1;                             // ALEN Always one for now since we have only start network support
      lop_tx_buffer[LOP_IX_SDU_REGACK_ADDRESS] = (neighbourDescriptor).resourceMask.slot;      // Address byte
      
      // Introduce a guard to accomodate for RX/TX switch time according to LOP_01.01§6
      delay(LOP_RTXGUARD);
    
      // Send the PDU using the power negotiated with the outer node.
      radio.setPALevel((rf24_pa_dbm_e)lop_rx_buffer[LOP_IX_SDU_REG_POW]);  
      sendLoPRANMessage(lop_tx_buffer, LOP_LEN_SDU_REGACK);
      
      dia_simpleFormNumericLog("REG",lop_rx_buffer[LOP_IX_SDU_REGACK_SLOT]);
    }
  }    
}