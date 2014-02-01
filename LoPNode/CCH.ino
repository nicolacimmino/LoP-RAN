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

uint64_t CCH_PIPE_ADDR_IN = 0;
uint64_t CCH_PIPE_ADDR_OUT = 0;

void calculateCCHPipeAddresses()
{
  NetTime currentTime = getNetworkTime();
  
  CCH_PIPE_ADDR_IN = 0x51;
  CCH_PIPE_ADDR_IN = (CCH_PIPE_ADDR_IN << 8) | currentTime.block;
  CCH_PIPE_ADDR_IN = (CCH_PIPE_ADDR_IN << 8) | currentTime.frame;
  CCH_PIPE_ADDR_IN = (CCH_PIPE_ADDR_IN << 8) | currentTime.slot;
  
  CCH_PIPE_ADDR_OUT = 0x50;
  CCH_PIPE_ADDR_OUT = (CCH_PIPE_ADDR_OUT << 8) | currentTime.block;
  CCH_PIPE_ADDR_OUT = (CCH_PIPE_ADDR_OUT << 8) | currentTime.frame;
  CCH_PIPE_ADDR_OUT = (CCH_PIPE_ADDR_OUT << 8) | currentTime.slot; 
}

void inititateCCHTransaction()
{
    // Purely for test, flash led on slot 9
    digitalWrite(2,1);
        
    calculateCCHPipeAddresses();
    radio.openWritingPipe(CCH_PIPE_ADDR_IN);  
    radio.openReadingPipe(1, CCH_PIPE_ADDR_OUT);
  
    delay(LOP_RTXGUARD);
    
    int txBufIndex = 5;
       
    // We build the the MSG message according to ...:
    // |5   |6     |7  |...|
    // |0x80|MSGLEN|MSG|...|
        
    lop_tx_buffer[txBufIndex++] = 0x80;            // MSGI
    
    txBufIndex++;                                  // MSG LEN will be filled up when known
    
    for(int ix=0; ix<LOP_MTU; ix++)                // MSG content
    {
      lop_tx_buffer[txBufIndex++] = lop_message_buffer[ix];
      // Message is null terminated. 
      if(lop_message_buffer[ix] == 0)
      {
        lop_tx_buffer[6] = ix+1;                    // MSG content length
        break;
      } 
    }
    
    radio.setPALevel((rf24_pa_dbm_e)inbound_tx_power);  
    sendLoPRANMessage(lop_tx_buffer, txBufIndex);
    Serial.print("MSGI,");
    Serial.println(lop_message_buffer);
    
    if(receiveLoPRANMessage(lop_rx_buffer, LOP_MTU , 100, rxBytes))
    {
      
      if(lop_rx_buffer[5] == (char)0x81)
      {
        for(int ix=0; ix<lop_rx_buffer[6]; ix++)
        {
          lop_message_buffer[ix] = lop_rx_buffer[7+ix];
        }
        Serial.print("MSGO,");
        Serial.println(lop_message_buffer);
      }
    }
    
    digitalWrite(2, 0);
}

void serveCCH()
{
  pONDescriptor neighbour = getNeighbourDescriptor(getNetworkTime());
  
  if(neighbour == 0)
    return;
    
  calculateCCHPipeAddresses();
  radio.openWritingPipe(CCH_PIPE_ADDR_OUT);  
  radio.openReadingPipe(1, CCH_PIPE_ADDR_IN);
  radio.setPALevel((rf24_pa_dbm_e)(*neighbour).tx_power);
  radio.startListening();
  
  if(receiveLoPRANMessage(lop_rx_buffer, LOP_MTU , 100, rxBytes))
    {
      if(lop_rx_buffer[5] == (char)0x80)
      {
        for(int ix=0; ix<lop_rx_buffer[6]; ix++)
        {
          lop_message_buffer[ix] = lop_rx_buffer[7+ix];
        }
        Serial.print("MSGI,");
        Serial.println(lop_message_buffer);
    
        int txBufIndex = 5;
        
        // We build the the MSGI message according to ...:
        // |5   |6     |7  |...|
        // |0x81|MSGLEN|MSG|...|
            
        lop_tx_buffer[txBufIndex++] = 0x81;            // MSGO
        
        txBufIndex++;                                  // MSG LEN will be filled up when known
        
        for(int ix=0; ix<LOP_MTU; ix++)                // MSG content
        {
          lop_tx_buffer[txBufIndex++] = lop_message_buffer[ix];
          // Message is null terminated. 
          if(lop_message_buffer[ix] == 0)
          {
            lop_tx_buffer[6] = ix+1;                    // MSG content length
            break;
          } 
        }
        
        delay(LOP_RTXGUARD);
    
        sendLoPRANMessage(lop_tx_buffer, txBufIndex);
        Serial.print("MSGO,");
        Serial.println(lop_message_buffer);
      }
    }
}
