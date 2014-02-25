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

#include "Arduino.h"
#include <SPI.h>
#include <EEPROM.h>
#include "LoPDia.h"
#include "NetTime.h"
#include "DataLink.h"
#include "LoPParams.h"
#include "NRF24L01Driver.h"

// TX Buffer.
char lop_tx_buffer[LOP_MTU];

// RX Buffer.
char lop_rx_buffer[LOP_MTU];

// Distance from Access Point.
// Default is 0xFF (that is no access).
byte lop_dap = 0xFF;

void setupDataLink()
{
  initializeRadio();
  
  // Preamble must always be in the beginning of each 
  //  message. We prefill here the TX buffer so we 
  //  save some redundant code in every message composition.
  strncpy(lop_tx_buffer, preamble, 4);
  
  // Initialize lop_dap according to our role
  lop_dap=(EEPROM.read(EEPROM_RFCH_ACT_AS_AP)==1)?0:0xFF;
}

bool receiveLoPRANMessage(char *data, uint32_t bufLen, int timeout_ms)
{
  startReceiving();
  
  long started_reading = millis();
  int messagelen = 6;
  boolean timeout = false;
  
  
  int received = 0;
  while(received < messagelen)
  {
    while (!(timeout = (millis() - started_reading) > timeout_ms))
    {
      if(isDataAvailable()) break;
    }
    
    // Fail the operation if we timeout or we receive more data
    //  than the supplied buffer can contain.
    if(timeout || (received + LOP_PAYL_SIZE > bufLen))
    {
      received = 0;
      dia_simpleFormTextLog("RAWRX", "NODATA");
      stopReceiving();
      return false;
    }
	
    readPayload(data + received);
    dia_logTime();
    dia_logString("RAWRX");  
    dia_logBufferToHex(data,data[4]);
    dia_closeLog();
    
    // If we don't have a preamble at the start of the message
    //   discard all data and keep waiting.
    if(strstr(data, preamble) - data != 0)
    {
      received = 0;
      continue; 
    }
    
    received += LOP_PAYL_SIZE;
    
    // If we got the header then the message length
    //  is the 5th byte.
    if(received >= 5)
    {
       messagelen = data[4];
    }
  }
  
  stopReceiving();
  return true;
}


void sendLoPRANMessage(char *data, int len)
{
  // Write message length.
  data[4]=len;
  transmitBuffer(data, len);
  
  dia_logTime();
  dia_logString("RAWTX");  
  dia_logBufferToHex(data,len);
  dia_closeLog();
}

