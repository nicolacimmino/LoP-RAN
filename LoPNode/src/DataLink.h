// DataLink is part of LoP-RAN , provides implementation of L2 functionality.
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

#ifndef __DataLink_h__
#define __DataLink_h__

#include <Arduino.h>
#include <SPI.h>
#include <EEPROM.h>
#include "EEPROMMap.h"
#include "LoPDia.h"
#include "NetTime.h"
#include "DataLink.h"
#include "LoPParams.h"
#include "NRF24L01Driver.h"

// The actual transmitted preamble is 4 bytes, we null terminate it to be able to use
//  string manipulation functions when comparing etc.
const char preamble[5] = {0x55, 0xAA, 0x55, 0xAA, 0x00}; 

// TX Buffer.
extern char lopFrameBuffer[];


// Distance from access point
extern byte lop_dap;

bool receiveLoPRANMessage(char *data, uint32_t bufLen, int timeout_ms);
void sendLoPRANMessage(char *data, int len);
void setupDataLink();

#endif

