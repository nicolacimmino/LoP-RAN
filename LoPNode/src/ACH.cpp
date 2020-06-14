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
#include "NRF24L01Driver.h"
#include "L5.h"

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
    setTransmitPower(inbound_tx_power);
    setRFChannel(inbound_channel);
    setTXExtendedPreamble(ACH_PIPE_ADDR_IN);
    setRXExtendedPreamble(ACH_PIPE_ADDR_OUT);
    
    // Introduce a guard to accomodate for sligh drift according to LOP_01.01§6
    delay(LOP_RTXGUARD);
    
    sendLoPRANMessage(lop_tx_buffer, LOP_LEN_SDU_REG);
    
    // Wait a reply, expect  a REGACK with the same token, ignore anything else
    //  according to according to LOP_01.01§6.2
    if(receiveLoPRANMessage(lop_rx_buffer, LOP_MTU , LOP_SLOTDURATION / 2))
    {
      if(lop_rx_buffer[LOP_IX_SDU_ID] == LOP_SDU_REGACK && (byte)lop_rx_buffer[LOP_IX_SDU_REGACK_TOKEN] == randToken)
      {
        // Store the inbound link time slot assigned to us.
        inboundTimeSlot = lop_rx_buffer[LOP_IX_SDU_REGACK_SLOT];
        
        node_address = 0;
        for(int ix=0; ix<sizeof(node_address); ix++)
        {
          node_address += lop_rx_buffer[LOP_IX_SDU_REGACK_ADDRESS+ix] << (ix*4); 
        }
        
        L5::onL4LinkUp();
        
        return true;
      }
    }

    // We failed to receive a REGACK, we just wait the next ACH to retry. We are still in slot 1
    //  so we cannot call directly wait until slot 1 ends as the call would return immediately.
    waitUntilInnerLink((NetTime){ 2, -1});  
    waitUntilInnerLink((NetTime){ 1, -1});    
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
  setTXExtendedPreamble(ACH_PIPE_ADDR_OUT);  
  setRXExtendedPreamble(ACH_PIPE_ADDR_IN);
  setRFChannel(lop_outbound_channel);
   
  if(receiveLoPRANMessage(lop_rx_buffer, LOP_MTU , LOP_SLOTDURATION / 2))
  {
    // Expect a REG message.
    if(lop_rx_buffer[LOP_IX_SDU_ID] == LOP_SDU_REG)
    {
      // Allocate radio resources and store them in the ONL.
      pONDescriptor neighbourDescriptor = allocateRadioResources(lop_rx_buffer[LOP_IX_SDU_REG_POW]);

      // Build the the REG message according to LOP_01.01§6.2
      lop_tx_buffer[LOP_IX_SDU_ID] = LOP_SDU_REGACK;
      lop_tx_buffer[LOP_IX_SDU_REGACK_TOKEN] = lop_rx_buffer[LOP_IX_SDU_REG_TOKEN];        // TOKEN
      lop_tx_buffer[LOP_IX_SDU_REGACK_SLOT] = neighbourDescriptor->resourceMask.slot;   // RMSLOT
      
      lop_tx_buffer[LOP_IX_SDU_REGACK_ADDRESS] = neighbourDescriptor->address & 0xFF;      // Address.
      lop_tx_buffer[LOP_IX_SDU_REGACK_ADDRESS+1] = neighbourDescriptor->address >>8 & 0xFF;      
      lop_tx_buffer[LOP_IX_SDU_REGACK_ADDRESS+2] = neighbourDescriptor->address >>16 & 0xFF;      
      lop_tx_buffer[LOP_IX_SDU_REGACK_ADDRESS+3] = neighbourDescriptor->address >>24 & 0xFF;      
      
      // Introduce a guard to accomodate for RX/TX switch time according to LOP_01.01§6
      delay(LOP_RTXGUARD);
    
      // Send the PDU using the power negotiated with the outer node.
      setTransmitPower(lop_rx_buffer[LOP_IX_SDU_REG_POW]);  
      sendLoPRANMessage(lop_tx_buffer, LOP_LEN_SDU_REGACK);
      
      dia_simpleFormNumericLog("REG",lop_rx_buffer[LOP_IX_SDU_REGACK_SLOT]);
    }
  }    
}
