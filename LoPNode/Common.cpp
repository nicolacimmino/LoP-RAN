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

#include <Arduino.h>
#include <RF24.h>        // Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>, GNU
#include "NetTime.h"
#include "DataLink.h"

// Inbound and outbound Message Buffers.
char lop_message_buffer_i[LOP_MTU];
char lop_message_buffer_o[LOP_MTU];

// Address of the source/destination of the current message.
uint64_t lop_message_buffer_address_i;
uint64_t lop_message_buffer_address_o;

// Address assigned to this node.
uint64_t node_address = 0xFFFFFFFF;

// Lowest usable power to talk to the inner node.
uint8_t inbound_tx_power = RF24_PA_MAX;

// The mask representing the timeslot assigned to this node
//  for traffic towards/from the inner node.
NetTime inboundTimeSlot;

// Counter of TX errors.
// Reset every time a sucessful communication on the CCH is performed.
byte tx_error_count = 0;

// Inidcates wether the inner link has been eastablished
boolean inner_link_up = false;
