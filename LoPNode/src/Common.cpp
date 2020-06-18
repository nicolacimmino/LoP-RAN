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

#include "Common.h"

// Inbound and outbound Message Buffers.
char lop_message_buffer_i[LOP_MTU];
char lop_message_buffer_o[LOP_MTU];

// Address of the source/destination of the current message.
uint32_t lop_message_buffer_address_i;
uint32_t lop_message_buffer_address_o;

// Address assigned to this node.
uint32_t node_address = 0xFFFFFFFF;

// Lowest usable power to talk to the inner node.
uint8_t inbound_tx_power = NRF24L01_TX_POW_0dBm;

// The mask representing the timeslot assigned to this node
//  for traffic towards/from the inner node.
byte inboundTimeSlot;

// Counter of TX errors.
// Reset every time a sucessful communication on the CCH is performed.
byte tx_error_count = 0;

// Inidcates wether the inner link has been eastablished
boolean inner_link_up = false;

uint8_t lop_outbound_channel = constrain(EEPROM.read(EEPROM_AP_CHANNEL), LOP_LOW_CHANNEL, LOP_HI_CHANNEL);

// Channel used for commincations towards the inner node.
uint8_t inbound_channel = 0;

// If set the radio will act as a scanner and only monitor
//  all traffic on a channel.
boolean scanner_mode = false;

void setupRadio(void)
{
  setupDataLink();
  setupControlInterface(); 
}
