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

#include "BCH.h"

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
  setRFChannel(lop_outbound_channel);
  setTXExtendedPreamble(BCH_PIPE_ADDR);
     
  // To allow outer nodes to perform ranging we step down power from NRF24L01+
  //  max power down to minimum. NRF24L01+ supports only 4 power levels.
  for(byte power=NRF24L01_TX_POW_0dBm; (int8_t)power>=NRF24L01_TX_POW_m18dBm; power--)
  { 
    // We build the the BCH SDU according to LOP_01.01§5.1 
    lopFrameBuffer[LOP_IX_SDU_ID] = LOP_SDU_BCH;
    lopFrameBuffer[LOP_IX_SDU_BCH_POW] = power;
    lopFrameBuffer[LOP_IX_SDU_BCH_DAP] = lop_dap;
    
    #ifdef LOP_ALPHA_NET
    lopFrameBuffer[LOP_IX_SDU_BCH_INFO] = 0x0 << LOP_IX_SDU_BCH_INFO_NET_TYPE_msb;
    lopFrameBuffer[LOP_IX_SDU_BCH_INFO] = ((getActiveOuterNeighboursCount() >= LOP_MAX_OUT_NEIGHBOURS)?1:0) << LOP_IX_SDU_BCH_INFO_NODE_FULL_msb;
    #else
    #error Missing implementation for current network type.
    #endif

    for(int ix=0; ix<LOP_IX_SDU_BCH_NID_LEN; ix++)
    {
      lopFrameBuffer[LOP_IX_SDU_BCH_NID_BASE+ix] = EEPROM.read(EEPROM_NTID_BASE+ix);
    }
    
    // And we finally send out the SDU using the current power.
    setTransmitPower(power);
    sendLoPRANMessage(lopFrameBuffer, LOP_LEN_SDU_BCH);
  }
   
  // We build the the BCHS (sync) SDU according to LOP_01.01§5.2
  lopFrameBuffer[LOP_IX_SDU_ID] = LOP_SDU_BCHS;
  lopFrameBuffer[LOP_IX_SDU_BCHS_OFF] = getInnerLinkNetworkTime().off;
    
  // We use max power for sync so we can reach all nodes that might have heard us
  //  as specified in "BCH Timing" LOP_01.01§5
  setTransmitPower(NRF24L01_TX_POW_0dBm);
  sendLoPRANMessage(lopFrameBuffer, LOP_LEN_SDU_BCHS);
         
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
  inbound_tx_power = NRF24L01_TX_POW_INVALID;
  
  // Listen on the BCH pipe according to LOP_01.01§4    
  setRXExtendedPreamble(BCH_PIPE_ADDR);
  
  // Keep scanning unless we are set as an AP from the control interface.
  while(lop_dap != 0)
  { 
    // Keep the control interface active during scan.
    serveControlInterface();
    
    // We attempt to receive on a given channel for maximum 180% of a slot so we maximize
    //  the chances to get a full BCH broadcast while not spending too much time on the
    //  same radio channel.
    setRFChannel(inbound_channel);
    if((!receiveLoPRANMessage(lopFrameBuffer, LOP_MTU , LOP_FRAMEDURATION))
          || (lopFrameBuffer[LOP_IX_SDU_BCH_INFO] & LOP_IX_SDU_BCH_INFO_NODE_FULL_MASK) != 0)
    {
      // We got nothing, or the node was full move to the next channel. We continuously loop all available channels.
      inbound_channel++;
      if(inbound_channel > LOP_HI_CHANNEL || inbound_channel < LOP_LOW_CHANNEL)
      {
        inbound_channel =  LOP_LOW_CHANNEL; 
      }
      
      dia_simpleFormNumericLog("SCAN", 1, inbound_channel);

      // Reset ranging register.
      inbound_tx_power = NRF24L01_TX_POW_INVALID;
  
      continue; 
    }
    else if(lopFrameBuffer[LOP_IX_SDU_ID] == LOP_SDU_BCH)
    {    
        
      // We got a BCH SDU. This is a good channel so we start ranging if the NID matches
      bool nidMatch = true;
      for(int ix=0; ix<LOP_IX_SDU_BCH_NID_LEN; ix++)
      {
        if(lopFrameBuffer[LOP_IX_SDU_BCH_NID_BASE+ix] != EEPROM.read(EEPROM_NTID_BASE+ix))
        {
           nidMatch = false;
        }
      }
      
      // Not the right NID, ignore and continue SCAN
      if(!nidMatch) continue;
      
      // Store this as last known good channel, save EEPROM life by not
      //  writing over if there is no change.
      if(inbound_channel != EEPROM.read(EEPROM_RFCH_INNER_NODE))
        EEPROM.write(EEPROM_RFCH_INNER_NODE, inbound_channel);
                 
      // Process ranging info and store lowest usable power if we heard
      //  a message weaker than last one.
      if(inbound_tx_power > lopFrameBuffer[LOP_IX_SDU_BCH_POW])
        inbound_tx_power = lopFrameBuffer[LOP_IX_SDU_BCH_POW];
        
      // Set our DAP as one more of the detected node. This is our current DAP
      //  even though the link is not currently yet up (that will happen in the
      //  ACH abd will be signalled by inner_link_up.
      lop_dap = lopFrameBuffer[LOP_IX_SDU_BCH_DAP]+1;
    } 
    else if(inbound_tx_power != NRF24L01_TX_POW_INVALID  && lopFrameBuffer[LOP_IX_SDU_ID] == LOP_SDU_BCHS)
    {
      // We got a BCHS SDU. 
      
      // Sync our nettime to the one received by the network. This is still off by the actual
      //  physical layer trasmit time, this in practice is well below 1mS since BCHS messages 
      //  have no ACK and are very short.
      setInnerLinkNetworkTime((NetTime){0, lopFrameBuffer[LOP_IX_SDU_BCHS_OFF]});
      
      // We have a sync.
      dia_simpleFormNumericLog("BCHS", 2, inbound_channel, inbound_tx_power);
  
      // Just for thesting a very rough frequency reuse, that is really not good.
      lop_outbound_channel = 50 + lop_dap;
      
      return;
    }
  }// while(true) 
}

