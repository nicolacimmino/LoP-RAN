// Scheduler is part of LoP-RAN , provides scheduling for the TDMA slots.
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
#include "Scheduler.h"
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

void runScheduler(void)
{
  while(true)
  {
    if(!scanner_mode)
    {
      run_regular_schedule();
    }
    else
    {
      run_scanner_schedule();
    }
  }
}

void run_regular_schedule()
{
  if(!inner_link_up && lop_dap!=0)
  {
    // Attempt to find an inner node and, if found
    //  sync to the inner link net time. This call blocks
    //  until a node is found.
    innerNodeScanAndSync();
    
    // Access the found inner node and get the link up.
    waitUntilInnerLink((NetTime){1, 10});
    inner_link_up = registerWithInnerNode();
  }
  
  if(inner_link_up || lop_dap==0)
  {
    for(int slot=0; slot<LOP_SLOTS_COUNT; slot++)
    {
      waitUntilInnerLink((NetTime){slot, 0});
      
      if(slot == LOP_BCH_SLOT)
      {
        broadcastBCH();
      }
      else if(slot == LOP_ACH_SLOT)
      {
        serveACH();
      }
      else if(slot == inboundTimeSlot)
      {
        inititateCCHTransaction();
        if(tx_error_count > LOP_MAX_TX_ERROR)
        {
          inner_link_up = false;
          break;
        }
      }
      else
      {
        serveCCH();
      }
    }
  }
}    

int scan_round = 0;

void run_scanner_schedule()
{
  scan_round++;
    
  for(int slot=0; slot<LOP_SLOTS_COUNT; slot++)
  {
    uint64_t inbound_pipe;
    uint64_t outbound_pipe;
    
    waitUntilInnerLink((NetTime){slot, 0});
    
    if(slot == LOP_BCH_SLOT)
    {
      if((scan_round%10)==1)
      {
        innerNodeScanAndSync();
        continue;
      }
      else
      {
        inbound_pipe =  BCH_PIPE_ADDR;
        outbound_pipe = BCH_PIPE_ADDR;
      }
    }
    else if(slot == LOP_ACH_SLOT)
    {
      inbound_pipe =  ACH_PIPE_ADDR_IN;
      outbound_pipe = ACH_PIPE_ADDR_OUT;
    }
    else
    {
      inbound_pipe =  CCH_PIPE_ADDR_IN | slot;
      outbound_pipe = CCH_PIPE_ADDR_OUT | slot;
    } 
    
    radio.openReadingPipe(1, inbound_pipe);
    radio.startListening();
    while(getInnerLinkNetworkTime().off < LOP_SLOTDURATION / 4)
    {
      if(receiveLoPRANMessage(lop_rx_buffer, LOP_MTU , LOP_SLOTDURATION / 4))
      {
        lop_dia_enabled = true;
        dia_logRawString("RAWSCAN ");
        dia_logRawTime();  
        dia_logBufferToHex(lop_rx_buffer,lop_rx_buffer[4]);
        dia_closeLog();
        lop_dia_enabled = false;
      }
    }
    
    radio.openReadingPipe(1, outbound_pipe);
    radio.startListening();
    if(receiveLoPRANMessage(lop_rx_buffer, LOP_MTU , LOP_SLOTDURATION / 4))
    {
      lop_dia_enabled = true;
      dia_logRawString("RAWSCAN "); 
      dia_logRawTime(); 
      dia_logBufferToHex(lop_rx_buffer,lop_rx_buffer[4]);
      dia_closeLog();
      lop_dia_enabled = false;
    }
  }

}    
