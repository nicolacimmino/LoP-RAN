// L4 is part of LoP-RAN
//  Copyright (C) 2020 Nicola Cimmino
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

#include "L4.h"
#include "LopParams.h"
#include "Common.h"

extern byte lop_dap;
extern char lop_message_buffer_i[];
extern char lop_message_buffer_o[];

byte sendMessage(uint8_t address, char *message, uint8_t messageLength = 0)
{
    if (address != 0 && lop_dap != 0)
    {
        return ERROR_ADDRESS_INVALID;
    }

    if (messageLength == 0)
    {
        messageLength = strlen(message);
    }

    // If we are the AP (DAP=0) TX means outbound else it means always inbound.
    char *message_buffer = (lop_dap == 0) ? lop_message_buffer_o : lop_message_buffer_i;
    memset(message_buffer, 0, LOP_MTU);
    memcpy(message_buffer, message, min(messageLength, LOP_MTU)); // TODO: this is mtu size minus header size

    lop_message_buffer_address_o = address;

    return ERROR_NONE;
}
