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
#include <EEPROM.h>
#include "LoPDia.h"
#include "NetTime.h"
#include "DataLink.h"



RF24 radio(radio_ce_pin,radio_csn_pin);

// Message Buffer.
char lop_message_buffer[LOP_MTU];

// TX Buffer.
char lop_tx_buffer[LOP_MTU];

// RX Buffer.
char lop_rx_buffer[LOP_MTU];

void setupDataLink()
{
  // These are fixed parameters in LoP-RAN.
  radio.begin();
  radio.setCRCLength(RF24_CRC_8);               // 8 bits CRC
  radio.setRetries(15,15);                      // Max 15 retries 4mS interval
  radio.setPayloadSize(LOP_PAYL_SIZE);                      // Payload sise, phisical layer MTU
  radio.setChannel(EEPROM.read(EEPROM_RFCH_INNER_NODE));   // We start from the last known good channel
  radio.setDataRate(RF24_250KBPS);              // 250 kbps
  radio.setPALevel(RF24_PA_MAX);                // We start from max power, ranging will take care to adjust this.
  radio.setAutoAck(0);                          // Auto ACK enabled  
  
  // Preamble must always be in the beginning of each 
  //  message. We prefill here the TX buffer so we 
  //  save some redundant code in every message composition.
  strncpy(lop_tx_buffer, preamble, 4);
  
  // Just for test purpose until we have the UART interface to send messages
  strncpy(lop_message_buffer, "ping", 4);
  lop_message_buffer[4] = 0;
    
}

bool receiveLoPRANMessage(char *data, uint32_t bufLen, int timeout_ms)
{
  long started_reading = millis();
  int messagelen = 6;
  bool timeout = false;
  
  
  int received = 0;
  while(received < messagelen)
  {
    while ( !radio.available() && !timeout)
    {
      timeout = (millis() - started_reading > timeout_ms);
    }
    
    // Fail the operation if we timeout or we receive more data
    //  than the supplied buffer can contain.
    if(timeout || (received + LOP_PAYL_SIZE > bufLen))
    {
      received = 0;
      dia_simpleFormTextLog("RAWRX", "NODATA");
      return false;
    }
	
    radio.read( data + received , LOP_PAYL_SIZE );
    /*dia_logTime();
    dia_logString("RAWRX");  
    dia_logBufferToHex(data,data[4]);
    dia_closeLog();
    */
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
  
  return true;
}


void sendLoPRANMessage(char *data, int len)
{
  // Write message length.
  data[4]=len;
  /*
  dia_logTime();
  dia_logString("RAWTX");  
  dia_logBufferToHex(data,len);
  dia_closeLog();
  */
  int offset=0;
  radio.stopListening();
  while(offset < len)
  {  
    radio.write(data+offset, LOP_PAYL_SIZE);
    //delay(1);
    offset+=LOP_PAYL_SIZE;
  }
  radio.startListening();
}

