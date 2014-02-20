// CCH is part of LoP-RAN , provides managing of the CommunicationCHannel.
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
#include "OuterNeighboursList.h"
#include "NetTime.h"
#include "CCH.h"

uint64_t CCH_PIPE_ADDR_IN = 0;
uint64_t CCH_PIPE_ADDR_OUT = 0;

void calculateCCHPipeAddresses()
{
  NetTime currentTime = getInnerLinkNetworkTime();
  
  CCH_PIPE_ADDR_IN =  0x51000200 | currentTime.slot;
  CCH_PIPE_ADDR_OUT = 0x50000200 | currentTime.slot;
}

void inititateCCHTransaction()
{
    // Purely for test
    digitalWrite(2,1);
 
    int txBufIndex = 5;
       
    // We build the the MSG message according to ...:
    // |5   |6     |7      |...|
    // |0x80|MSGLEN|MSGDATA|...|
        
    lop_tx_buffer[txBufIndex++] = 0x80;            // MSGI
    
    txBufIndex++;                                  // MSG LEN will be filled up when known
    
    for(int ix=0; ix<LOP_MTU; ix++)                // MSG content
    {
      // Message is null terminated but we don't send the null
      //  as there is already the MSG length
      if(lop_message_buffer_i[ix] == 0)
      {
        lop_tx_buffer[6] = ix;                    // MSG content length
        break;
      }
      lop_tx_buffer[txBufIndex++] = lop_message_buffer_i[ix]; 
    }
    
    // Set the radio to the power we negotiated during ranging.
    radio.setPALevel((rf24_pa_dbm_e)inbound_tx_power); 
 
    // Set up the right pipe address for the current network time.
    calculateCCHPipeAddresses();
    radio.openWritingPipe(CCH_PIPE_ADDR_IN);  
    radio.openReadingPipe(1, CCH_PIPE_ADDR_OUT);
    radio.setChannel(inbound_channel);
  
    // Wait what is left of the RTX guard period.
    delay(constrain(LOP_RTXGUARD-getInnerLinkNetworkTime().off,0,LOP_RTXGUARD));
    
    // Send the datagram. 
    sendLoPRANMessage(lop_tx_buffer, txBufIndex);
    
    if(lop_tx_buffer[6]>0)
    {
      dia_simpleFormTextLog("MSGI", lop_message_buffer_i);
    }
    
    // Empty the message buffer.
    lop_message_buffer_i[0] = 0;
    
    if(receiveLoPRANMessage(lop_rx_buffer, LOP_MTU , LOP_SLOTDURATION / 2))
    {
      if(lop_rx_buffer[5] == (char)0x81)
      {
        setInnerLinkNetworkTime((NetTime){getInnerLinkNetworkTime().slot, lop_rx_buffer[6]});
        
        for(int ix=0; ix<lop_rx_buffer[7]; ix++)
        {
          lop_message_buffer_o[ix] = lop_rx_buffer[8+ix];
        }
        
        // We add null terminator as it's not sent.
        if(lop_rx_buffer[7]>0)
        {
          lop_message_buffer_o[lop_rx_buffer[7]]=0;
          dia_simpleFormTextLog("MSGO", lop_message_buffer_o);
        }
      
        // Reset the TX error count.
        tx_error_count = 0;
      }
      else
      {
        tx_error_count++;     
      }

    }
    else
    {
      tx_error_count++;     
    }
  
    digitalWrite(2, 0);
}

void serveCCH()
{
  pONDescriptor neighbourDescriptor = getNeighbourDescriptor(getInnerLinkNetworkTime());
  
  if(neighbourDescriptor == 0)
    return;
    
  calculateCCHPipeAddresses();
  radio.openWritingPipe(CCH_PIPE_ADDR_OUT);  
  radio.openReadingPipe(1, CCH_PIPE_ADDR_IN);
  radio.setPALevel((rf24_pa_dbm_e)neighbourDescriptor->tx_power);
  radio.setChannel(lop_outbound_channel);
  radio.startListening();
  
  if(receiveLoPRANMessage(lop_rx_buffer, LOP_MTU , LOP_SLOTDURATION / 2))
  {
      if(lop_rx_buffer[5] == (char)0x80)
      {
        for(int ix=0; ix<lop_rx_buffer[6]; ix++)
        {
          lop_message_buffer_i[ix] = lop_rx_buffer[7+ix];
        }
        
        // We add null terminator as it's not sent.
        if(lop_rx_buffer[6]>0)
        {
          lop_message_buffer_i[lop_rx_buffer[6]]=0;        
          lop_message_buffer_address_i = neighbourDescriptor->address;
          dia_simpleFormTextLog("MSGI", lop_message_buffer_i);
        }
        
        int txBufIndex = 5;
        
        // We build the the MSGI message according to ...:
        // |5   |6  |7     |8  |...|
        // |0x81|OFF|MSGLEN|MSG|...|
            
        lop_tx_buffer[txBufIndex++] = 0x81;            // MSGO
        
        txBufIndex++;                                  // Current OFF will be filled just before sending
        txBufIndex++;                                  // MSG LEN will be filled up when known
        
        // See if the outbound message waiting is for the address of the 
        //  node assigned to this slot, if not we just send an empty message
        //  in order to complete the CCH trasaction.
        lop_tx_buffer[7] = 0;
        if(lop_message_buffer_address_o == neighbourDescriptor->address)
        {
          for(int ix=0; ix<LOP_MTU; ix++)                // MSG content
          {
            // Message is null terminated but we don't send the null
            //  as there is already the MSG length
            if(lop_message_buffer_o[ix] == 0)
            {
              lop_tx_buffer[7] = ix;                    // MSG content length
              break;
            }
            lop_tx_buffer[txBufIndex++] = lop_message_buffer_o[ix]; 
          }
          // Empty the message buffer.
          lop_message_buffer_o[0]=0;
        }
       
        delay(LOP_RTXGUARD);
    
        lop_tx_buffer[6] = getInnerLinkNetworkTime().off;  // OFF
        
        sendLoPRANMessage(lop_tx_buffer, txBufIndex);
        if(lop_tx_buffer[7]>0)
        {
          dia_simpleFormTextLog("MSGO", lop_message_buffer_o);
        }
        // We got some data from the node, refresh the last seen
        neighbourDescriptor->last_seen = millis();
      }
  }
}
