// LoPParams is part of LoP-RAN , provides constants that define network behaviour.
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

#ifndef __LOP_PARAMS_H__
#define __LOP_PARAMS_H__

// Below constants represet LoP-RAN network parameters

#define LOP_ALPHA_NET

#ifdef LOP_ALPHA_NET
  #define LOP_LOW_CHANNEL 49                 // Lowest usable radio channel
  #define LOP_HI_CHANNEL  54                 // Highest usable radio channel
  #define LOP_PAYL_SIZE 32                 // NRF24L01 MAC layer payload size
  #define LOP_MTU 256                      // MTU size
  #define LOP_REG_MAX_RETRY 3
  #define BCH_PIPE_ADDR  0x5000000001LL     // BCH Pipe Address (outbound only)
  #define ACH_PIPE_ADDR_IN 0x5100000100LL  // ACH inbound pipe
  #define  ACH_PIPE_ADDR_OUT 0x5000000100LL  // ACH outboud pipe
  #define  LOP_RTXGUARD 2                    // RX-TX Guard time
  #define  LOP_SLOTDURATION 100.0f              // Slot duration in mS
  #define LOP_FRAMEDURATION 1000.0f               // Frame duration in mS
  #define LOP_SLOTS_COUNT 10                    // Frame duration / slot duration
  #define LOP_ONL_ALLOCATION_TTL 10000          // Maximum TTL for an entry in the ONL
  #define LOP_BCH_SLOT 0
  #define LOP_ACH_SLOT 1
  #define  LOP_MAX_OUT_NEIGHBOURS 8          // Maximum outer neighbours
  #define LOP_MAX_DAP 1                        // Maximum Distance from Access Point
  #define LOP_MAX_TX_ERROR 4                  // Maximum transmission errors before the inner link is torn down
  #define LOP_IX_SDU_ID  5 // SDU ID position in the PDU
  #define LOP_SDU_BCH 0x60
  #define LOP_SDU_BCHS 0x61
  #define LOP_SDU_REG 0x70
  #define LOP_SDU_REGACK 0x71
#else
    #error Missing implementation for current network type.
#endif



#endif

