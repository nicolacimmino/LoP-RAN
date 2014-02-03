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

#include "LoPDia.h"

boolean registerWithInnerNode()
{
  // Get a random number to minimize chances of collisions during registration.
  // Since the instant we come here is a function of when the inner node triggers
  //  the slot start relative to our code start time correlation between two nodes
  //  should be very low even if they are started concurrently, so micros() will
  //  provide a random enough seed. Additionally in the rare event (1 over 255)
  //  that two nodes get the same random token both registration messages will
  //  be discarded by the inner node.
  randomSeed(micros());
  
  for(int retry=0; retry<3; retry++)
  {
    byte randToken = random(0,255);
    
    int txBufIndex = 5;
       
    // We build the the REG message according to ...:
    // |5   |6    |7    | 
    // |0x70|Power|Token|
          
    // REG
    lop_tx_buffer[txBufIndex++] = 0x70;
    
    // The power
    lop_tx_buffer[txBufIndex++] = inbound_tx_power;
    
    // The token
    lop_tx_buffer[txBufIndex++] = randToken;
            
    radio.setPALevel((rf24_pa_dbm_e)inbound_tx_power);
    radio.openWritingPipe(ACH_PIPE_ADDR_IN);
    radio.openReadingPipe(1, ACH_PIPE_ADDR_OUT);
    
    delay(LOP_RTXGUARD);
    sendLoPRANMessage(lop_tx_buffer, txBufIndex);
    radio.startListening();
    
    if(receiveLoPRANMessage(lop_rx_buffer, LOP_MTU , 100, rxBytes))
    {
      if(lop_rx_buffer[5] == 0x71)
      {
        inboundTimeSlot.block = lop_rx_buffer[7];
        inboundTimeSlot.frame = lop_rx_buffer[8];
        inboundTimeSlot.slot = lop_rx_buffer[9];
        return true;
      }
    }

    // Wait next ACH, this is more efficient than give up
    //  immediately are resume scan. We need to wait next frame
    //  slot 1.
    waitUntil((NetTime){-1, (getNetworkTime().frame + 1) % 10, 1, -1});    
  }
  
  return false;
}

void serveACH()
{
  // Listen on the ACH pipe
  radio.openWritingPipe(ACH_PIPE_ADDR_OUT);  
  radio.openReadingPipe(1, ACH_PIPE_ADDR_IN);
  radio.setChannel(50);
  radio.startListening();
  
  // Used to sore received bytes count.
  int rxBytes = 0;
  
  if(receiveLoPRANMessage(lop_rx_buffer, LOP_MTU , 100, rxBytes))
  {
    if(lop_rx_buffer[5] == 0x70)
    {
      byte tx_power = lop_rx_buffer[6];
      byte token = lop_rx_buffer[7];
      
      ONDescriptor* neighbourDescriptor = allocateRadioResources(tx_power);
      
      int txBufIndex = 5;
       
      // We build the the REGA message according to ...:
      // |5   |6    |7      |8      |9     |10  |11 ...    |
      // |0x71|Token|RMBLOCK|RMFRAME|RMSLOT|ALEN|Address...|
          
      lop_tx_buffer[txBufIndex++] = 0x71;                          // REGACK
      lop_tx_buffer[txBufIndex++] = token;                         // TOKEN
      lop_tx_buffer[txBufIndex++] = (*neighbourDescriptor).resourceMask.block;  // RMBLOCK
      lop_tx_buffer[txBufIndex++] = (*neighbourDescriptor).resourceMask.frame;  // RMFRAME
      lop_tx_buffer[txBufIndex++] = (*neighbourDescriptor).resourceMask.slot;   // RMSLOT
      lop_tx_buffer[txBufIndex++] = 1;                             // ALEN
      lop_tx_buffer[txBufIndex++] = (*neighbourDescriptor).resourceMask.slot;      // Address byte
      
      
      delay(LOP_RTXGUARD);
      radio.setPALevel((rf24_pa_dbm_e)tx_power);  
      sendLoPRANMessage(lop_tx_buffer, txBufIndex);
      dia_simpleFormNumericLog("REG",lop_rx_buffer[10]);
    }
  }    
}
