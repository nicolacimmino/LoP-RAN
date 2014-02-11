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
#include "BCH.h"


// Distance from access point
byte lop_dap = 0;

// Channel used for commincations towards the inner node.
uint8_t inbound_channel = 0;

///////////////////////////////////////////////////////////////////////////////////////////////
// Broadcast BCH
//
// Must be called at OFF=0 of SLOT=0 of each frame of a node that can act as an inner node
//  for another node (DAP!=0xFF).
//
// This call will return as soon as the broadcast is done which might be before the end of 
//  the slot, but never after.
//
void broadcastBCH()
{
  // RTX guard according to LOP_01.01§5 this is needed to maximize
  //  the chances of already syncronized nodes that are re-aligning
  //  their clocks to catch message even if they have slightly drifted.
  delay(LOP_RTXGUARD);
  
  // Setup the BCH phy layer parameters according to LOP_01.01§4
  radio.setChannel(50);
  radio.openWritingPipe(BCH_PIPE_ADDR);
     
  // To allow outer nodes to perform ranging we step down power from NRF24L01+
  //  max power down to minimum. NRF24L01+ supports only 4 power levels.
  for(byte power=RF24_PA_MAX; (int8_t)power>=RF24_PA_MIN; power--)
  { 
    // We build the the BCH SDU according to LOP_01.01§5.1 
    lop_tx_buffer[LOP_IX_SDU_ID] = LOP_SDU_BCH;
    lop_tx_buffer[LOP_IX_SDU_BCH_POW] = power;
    lop_tx_buffer[LOP_IX_SDU_BCH_BLOCK] = getNetworkTime().block;
    lop_tx_buffer[LOP_IX_SDU_BCH_FRAME] = getNetworkTime().frame;
    lop_tx_buffer[LOP_IX_SDU_BCH_DAP] = lop_dap;
    
    // And we finally send out the SDU using the current power.
    radio.setPALevel((rf24_pa_dbm_e)power);
    sendLoPRANMessage(lop_tx_buffer, LOP_LEN_SDU_BCH);
  }
   
  // We build the the BCHS (sync) SDU according to LOP_01.01§5.2
  lop_tx_buffer[LOP_IX_SDU_ID] = LOP_SDU_BCHS;
  lop_tx_buffer[LOP_IX_SDU_BCHS_BLOCK] = getNetworkTime().block;
  lop_tx_buffer[LOP_IX_SDU_BCHS_FRAME] = getNetworkTime().frame;
  lop_tx_buffer[LOP_IX_SDU_BCHS_OFF] = getNetworkTime().off;
    
  // We use max power for sync so we can reach all nodes that might have heard us
  //  as specified in "BCH Timing" LOP_01.01§5
  radio.setPALevel(RF24_PA_MAX);
  sendLoPRANMessage(lop_tx_buffer, LOP_LEN_SDU_BCHS);
         
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Attempts to find another node that would act as an inner node and provide access to
//  the network. Performs network scan and ranging (LOP_01.01§5).
//
// This function blocks until a valid inner node is found. On return from this function
//  the globals inbound_channel and inbound_tx_power are set to the right values for the
//  found inner node. Additionally the internal network time is synced to that of the
//  inner node found.
//
// The function can be called at any network time when the node is not connected to an inner node
//  (DAP=0xFF) but must be called at OFF=0 of SLOT=0 in when connected (DAP!=0xFF)
//
void innerNodeScanAndSync()
{
  // Start the scan from the last known good channel, if it is inside good values.
  inbound_channel = constrain(EEPROM.read(EEPROM_RFCH_INNER_NODE), LOP_LOW_CHANNEL, LOP_HI_CHANNEL);
  
  // We start from an invalid power to indicated ranging is not done.
  inbound_tx_power = RF24_PA_ERROR;
  
  // Listen on the BCH pipe according to LOP_01.01§4    
  radio.openReadingPipe(1, BCH_PIPE_ADDR);
  radio.startListening();
  
  while(true)
  { 
    // We attempt to receive on a given channel for maximum 180% of a slot so we maximize
    //  the chances to get a full BCH broadcast while not spending too much time on the
    //  same radio channel.
    radio.setChannel(inbound_channel);
    if(!receiveLoPRANMessage(lop_rx_buffer, LOP_MTU , LOP_FRAMEDURATION))
    {
      // We got nothing, move to the next channel. We continuously loop all available channels.
      inbound_channel = (++inbound_channel % (LOP_HI_CHANNEL - LOP_LOW_CHANNEL)) + LOP_LOW_CHANNEL; 
     
      dia_simpleFormNumericLog("SCAN", 1, inbound_channel);

      continue; 
    }
    else if(lop_rx_buffer[LOP_IX_SDU_ID] == LOP_SDU_BCH)
    {      
      // We got a BCH SDU. This is a good channel so we start ranging.
      
      // Store this as last known good channel, save EEPROM life by not
      //  writing over if there is no change.
      if(inbound_channel != EEPROM.read(EEPROM_RFCH_INNER_NODE))
        EEPROM.write(EEPROM_RFCH_INNER_NODE, inbound_channel);
                 
      // Process ranging info and store lowest usable power if we heard
      //  a message weaker than last one.
      if(inbound_tx_power > lop_rx_buffer[LOP_IX_SDU_BCH_POW])
        inbound_tx_power = lop_rx_buffer[LOP_IX_SDU_BCH_POW];
    } 
    else if(inbound_tx_power != RF24_PA_ERROR  && lop_rx_buffer[LOP_IX_SDU_ID] == LOP_SDU_BCHS)
    {
      // We got a BCHS SDU. 
      
      // Sync our nettime to the one received by the network. This is still off by the actual
      //  physical layer trasmit time, this in practice is well below 1mS since BCHS messages 
      //  have no ACK and are very short.
      setNetworkTime((NetTime){lop_rx_buffer[LOP_IX_SDU_BCHS_BLOCK], lop_rx_buffer[LOP_IX_SDU_BCHS_FRAME], 0, lop_rx_buffer[LOP_IX_SDU_BCHS_OFF]});
      
      // We have a sync.
      dia_simpleFormNumericLog("BCHS", 2, inbound_channel, inbound_tx_power);
  
      return;
    }
  }// while(true) 
}

