// BCH is part of LoP-RAN , provides control for the Broadcast CHannel.
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

///////////////////////////////////////////////////////////////////////////////////////////////
// Broadcast BCH
//
// Must be called as close as possible to off=0 of slot=0
//
// We have to broadcast first BCH repeatedly with ranging information followed
//  by a BCH Sync at off=90
//
void broadcastBCH()
{
   
  radio.setChannel(50);
  radio.openWritingPipe(BCH_PIPE_ADDR);
     
  // To allow ranging we step down power from 0dBm to -18dBm
  for(byte power=3; (int8_t)power>=0; power--)
  {
    int txBufIndex = 5;
     
    // We build the the BCH message according to ...:
    // |5   |6    |7    |8    |
    // |0x60|Power|Block|Frame|
        
    // BCH
    lop_tx_buffer[txBufIndex++] = 0x60;
    
    // The power
    lop_tx_buffer[txBufIndex++] = power;
        
    // Block and frame
    lop_tx_buffer[txBufIndex++] = getNetworkTime().block;
    lop_tx_buffer[txBufIndex++] = getNetworkTime().frame;
    
    radio.setPALevel((rf24_pa_dbm_e)power);
    sendLoPRANMessage(lop_tx_buffer, txBufIndex);
  }
  
  // Wait till we near the end of the slot. 
  waitUntil((NetTime){-1, -1, -1, 90});  
  
  // We build the the BCH sync message according to ...:
  // |5   |6    |7    |
  // |0x61|Block|Frame|
        
  int txBufIndex = 5;
        
  // BCHS
  lop_tx_buffer[txBufIndex++] = 0x61;
             
  // Block and frame
  lop_tx_buffer[txBufIndex++] = getNetworkTime().block;
  lop_tx_buffer[txBufIndex++] = getNetworkTime().frame;
  lop_tx_buffer[txBufIndex++] = getNetworkTime().off;
    
  // We use max power for sync so we can reach all nodes that might have heard us.
  radio.setPALevel(RF24_PA_MAX);
  sendLoPRANMessage(lop_tx_buffer, txBufIndex);
         
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Attempts to find a node to communicate with.
//
// Performs network scan and ranging.
// This function blocks until a valid inner node is found. On return from this function
//  the globals inbound_channel and inbound_tx_power are set to the right values for the
//  found inner node. Additionally the internal network time is synced to that of the
//  inner node found.
//
void scanForNet()
{
  // Start the scan from the last known good channel.
  inbound_channel = constrain(EEPROM.read(EEPROM_RFCH_INNER_NODE), LOP_LOW_CHANNEL, LOP_HI_CHANNEL);
  
  // We start from an invalid power so we can detect that we
  //  got at least one ranging message avoiding false power
  //  detection if we happen to get as sync as first message.
  inbound_tx_power = RF24_PA_MAX+1;
  
  // Listen on the BCH pipe        
  radio.openReadingPipe(1, BCH_PIPE_ADDR);
  radio.startListening();
  
  while(true)
  { 
    // Try to read a message, linger maximum 1.5s on a given channel and if you get
    //  nothing move on to the next.
    radio.setChannel(inbound_channel);
    if(!receiveLoPRANMessage(lop_rx_buffer, LOP_MTU , 1500, rxBytes))
    {
      inbound_channel = (++inbound_channel % (LOP_HI_CHANNEL - LOP_LOW_CHANNEL)) + LOP_LOW_CHANNEL; 
     
      Serial.print("SCAN,");
      Serial.println(inbound_channel);
      continue; 
    }
    
    if(lop_rx_buffer[5] == 0x60)
    {        
      // Store this as last known good channel, save EEPROM life by not
      //  writing if there is no change.
      if(inbound_channel != EEPROM.read(EEPROM_RFCH_INNER_NODE))
        EEPROM.write(EEPROM_RFCH_INNER_NODE, inbound_channel);
                 
      // Process ranging info and store lowest usable power if we heard
      //  a message weaker than last one.
      if(inbound_tx_power > lop_rx_buffer[6])
        inbound_tx_power = lop_rx_buffer[6];
    } 
    else if(inbound_tx_power <= RF24_PA_MAX  && lop_rx_buffer[5] == 0x61)
    {
      // Sync our nettime
      setNetworkTime((NetTime){lop_rx_buffer[6], lop_rx_buffer[7], 0, 90});
      
      // We have a sync. 
      Serial.print("BCHS,");
      Serial.print(getNetworkTime().block);
      Serial.print(",");
      Serial.print(getNetworkTime().frame);
      Serial.print(",");
      Serial.print(inbound_channel);
      Serial.print(",");
      Serial.println(inbound_tx_power);
      lop_rx_buffer[0]=0;
      return;
    }
  }// while(true) 
}



