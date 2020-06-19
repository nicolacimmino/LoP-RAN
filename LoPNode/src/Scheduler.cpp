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
        if(lop_dap < LOP_MAX_DAP) broadcastBCH();
      }
      else if(slot == LOP_ACH_SLOT)
      {
        if(lop_dap < LOP_MAX_DAP) serveACH();
      }
      else if(slot == inboundTimeSlot)
      {
        inititateCCHTransaction();       
      }
      else if(slot == LOP_SLOTS_COUNT-1)
      {
        userCode();
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
      inbound_pipe =  CCH::getCCHPipeAddressesIn() | slot;
      outbound_pipe = CCH::getCCHPipeAddressesOut() | slot;
    } 
    
    setRXExtendedPreamble(inbound_pipe);
    startReceiving();
    while(getInnerLinkNetworkTime().off < LOP_SLOTDURATION / 4)
    {
      if(receiveLoPRANMessage(lopFrameBuffer, LOP_MTU , LOP_SLOTDURATION / 4))
      {
        lop_dia_enabled = true;
        dia_logRawString("RAWSCAN ");
        dia_logRawTime();  
        dia_logBufferToHex(lopFrameBuffer,lopFrameBuffer[4]);
        dia_closeLog();
        lop_dia_enabled = false;
      }
    }
    
    setRXExtendedPreamble(outbound_pipe);
    startReceiving();
    if(receiveLoPRANMessage(lopFrameBuffer, LOP_MTU , LOP_SLOTDURATION / 4))
    {
      lop_dia_enabled = true;
      dia_logRawString("RAWSCAN "); 
      dia_logRawTime(); 
      dia_logBufferToHex(lopFrameBuffer,lopFrameBuffer[4]);
      dia_closeLog();
      lop_dia_enabled = false;
    }
  }

}    

