// LopNode implements a LoP-RAN , a low power radio access network
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
// Tested on a pro-mini with ATMega168 5V @16MHz and a nano.
// 
// Connections:
//
// Warning! There are at least two types of radio modules, these pins are for a specific one used in protos.
//  Please use signal names and check your module pinout.
//
//  Radio  Signal  Arduino
//  1      GND     GND
//  2      VCC     3.3V
//  3      CE      9
//  4      CSN     10
//  5      CLK     13
//  6      MOSI    11
//  7      MISO    12//

#include <SPI.h>
#include <RF24.h>        // Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>, GNU
#include <EEPROM.h>
#include "NetTime.h"

//
// Change the following consts if your connections are arranged in different way.

#define radio_ce_pin 9        // CE pin for the NRF24L01+
#define radio_csn_pin 10      // CSN pin for the NRF24L01+

// EEPROM Memory map
const byte EEPROM_RFCH_INNER_NODE = 0x01;        // 0x01  Last known good RF channel
const byte EEPROM_URI  = 0x10;        // 0x10  Start of our URI, null terminated max 64 bytes including null
const byte EEPROM_URI_TO = 0x50;        // 0x50  Start of the URI we report to, null terminated max 64 bytes including null

const byte EEPROM_FOCAL_NODE = 0xFF;    // 0xFF  During development used to signal the fact that this node is a focal node (if set to 1)

// An instance of the NRF24L01+ chip controller.
RF24 radio(radio_ce_pin,radio_csn_pin);

// Below constants represet LoP-RAN network parameters

const uint32_t LOP_PAYL_SIZE = 16;                 // Single NRF24L01 packet payload size
const uint32_t LOP_MTU = 256;                      // MTU size
const uint8_t LOP_LOW_CHANNEL = 48;                 // Lowest usable radio channel
const uint8_t LOP_HI_CHANNEL = 51;                 // Highest usable radio channel
const uint64_t BCH_PIPE_ADDR = 0x5000000001LL;     // BCH Pipe Address (outbound only)
const uint64_t ACH_PIPE_ADDR_IN = 0x5100000100LL;  // ACH inbound pipe
const uint64_t ACH_PIPE_ADDR_OUT = 0x5000000100LL;  // ACH outboud pipe
// The actual transmitted preamble is 4 bytes, we null terminate it to be able to use
//  string manipulation functions when comparing etc.
const char preamble[5] = {0x55, 0xAA, 0x55, 0xAA, 0x00}; 

// TX Buffer.
char lop_tx_buffer[LOP_MTU];

// RX Buffer.
char lop_rx_buffer[LOP_MTU];

// Lowest usable power to talk to the inner node.
uint8_t inbound_tx_power = RF24_PA_MAX;

// Channel used for commincations towards the inner node.
uint8_t inbound_channel = 0;

// Used to sore received bytes count.
int rxBytes = 0;

// Indicates network status.
// TODO: change to enum we need more than 2 statuses.
boolean netStatus = false;

long off_off = 0;

// Board setup.
void setup(void)
{
  // We sometimes still use serial for debuggin.
  // Keep high speeds and use sparingly as writing to serial puts off all timings.
  Serial.begin(115200);
  
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
  
  //EEPROM.write(EEPROM_FOCAL_NODE,1);
  
}

void DumpToSerial(char *data, uint8_t length)
{
  for (int i=0; i<length; i++) 
  { 
    if ((uint8_t)data[i]<0x10) {Serial.print("0");} 
    Serial.print((uint8_t)data[i],HEX); 
    Serial.print(" "); 
  }
  Serial.println();
}

void loop(void)
{
  // During development we use this register to force a node
  //  to act as inner an other to act as outer.
  // In the final code it will be a serial command seeting
  //  the node as a focal if connected to an higher level controller.
  if(EEPROM.read(EEPROM_FOCAL_NODE) == 1)
  {  
      while(getNetworkTime().slot != 0)
      { 
        delay(1);
      }
      broadcastBCH();
      
      while(getNetworkTime().slot != 1)
      { 
        delay(1);
      }
      serverACH();
  }
  else
  {
      
      pinMode(2, OUTPUT);
        
 
      if(netStatus)
        waitUntil((NetTime){-1, -1, 0, 0});
        
        // Resync to network at every block. This corrects
        //  drifting caused by clock innacuracies while not
        //  being too heavy on the network. Note that time
        //  sync is a receive only operation so the fact that
        //  all nodes in the net will do it at the same time
        //  is no cause for congestion.
        if(!netStatus || isTime((NetTime){0,0,0,-1}))
        {
          scanForNet();
          
          // Wait slot 1 (ACH) and register if we are not already.
          if(!netStatus)
          {
            waitUntil((NetTime){-1, -1, 1, 10});
            netStatus = registerWithInnerNode();
          }
        }
        
        // For test purposes only wait slot 9
        //  to light the led if we are registered
        //  and put if off at the end of the slot
        if(netStatus)
        {
          waitUntil((NetTime){-1, -1, 9, 0});
          Serial.println(millis());
          digitalWrite(2,1);
          waitUntil((NetTime){-1, -1, 9, 50});
          Serial.println(millis());
          Serial.println(off_off);
          Serial.println("-");
          digitalWrite(2, 0);
        }
       
   }  
}



void sendLoPRANMessage(char *data, int len)
{
  // Write message length.
  data[4]=len;
  
  Serial.print("RAWTX,");  
  DumpToSerial(data,len);
  
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

bool receiveLoPRANMessage(char *data, uint32_t bufLen, int timeout_ms, int &received)
{
  long started_reading = millis();
  int messagelen = 6;
  bool timeout = false;
  
  
  received = 0;
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
      Serial.println("RAWRX,NODATA");
      return false;
    }
	
    radio.read( data + received , LOP_PAYL_SIZE );
    Serial.print("RAWRX,");
    DumpToSerial(data, data[4]);
    
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


