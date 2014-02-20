// Common is part of LoP-RAN , provides globals that are common to several modules.
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

#ifndef __COMMON_H__
#define __COMMON_H__

#include "NetTime.h"
#include "DataLink.h"

// Message Buffer.
extern char lop_message_buffer_i[];
extern uint32_t lop_message_buffer_address_i;
extern char lop_message_buffer_o[];
extern uint32_t lop_message_buffer_address_o;
extern uint8_t inbound_tx_power;
extern NetTime inboundTimeSlot;
extern byte tx_error_count;
extern boolean inner_link_up;
extern uint32_t node_address;
extern uint8_t lop_outbound_channel;
extern uint8_t inbound_channel;

#endif
