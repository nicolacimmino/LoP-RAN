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
  // Preamble must always be in the beginning of each 
  //  message. We prefill here the TX buffer so we 
  //  save some redundant code in every message composition.
  strncpy(lop_tx_buffer, preamble, 4);
  
  //EEPROM.write(EEPROM_FOCAL_NODE,1);
  
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
        
      // Now that we are synced to an inner node
      //  we just flash the LED on slot 5 purely to test sync
      while(true)
      {
        if(getNetworkTime().frame == 0 || !netStatus)
        {
          scanForNet();
        }
        
        while(getNetworkTime().slot != 1)
        {
          delay(1);
        }
        netStatus = registerWithInnerNode();
        
        if(netStatus)
        {
          while(getNetworkTime().slot != 9)
          {
            delay(1);
          }
          digitalWrite(2,1);
        }
        while(getNetworkTime().slot == 9)
        {
          delay(1);
        }
        digitalWrite(2, 0);
      }
  }     
}




void sendLoPRANMessage(char *data, int len)
{
  // Write message length.
  data[4]=len;
        
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
      Serial.println("Timeout");
      return false;
    }
	
    radio.read( data + received , LOP_PAYL_SIZE );
    DumpToSerial(data, received + LOP_PAYL_SIZE);
    
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

///////////////////////////////////////////////////////////////////////////////////////////////
// Broadcast BCH
//
// Must be called as close as possible to off=0 of slot=0
//
// We have to broadcast first BCH repeatedly with ranging information followed
//  by a BCH Sync at off=70
//
void broadcastBCH()
{
   
  radio.setChannel(50);
  radio.openWritingPipe(BCH_PIPE_ADDR);
     
  // To allow ranging we step down power from 0dBm to -18dBm
  for(byte power=3; (int8_t)power>=0; power--)
  {
    int txBufIndex = 5;
     
    // We build the the BCH message according to ...:
    // |0|1|2|3|4  |5    |6    |7    |
    // |B|C|H| |Len|Power|Block|Frame|
        
    // Header
    strncpy(lop_tx_buffer + txBufIndex, "BCH ", 4);
    txBufIndex+=4;
       
    // The power
    lop_tx_buffer[txBufIndex++] = power;
        
    // Block and frame
    lop_tx_buffer[txBufIndex++] = getNetworkTime().block;
    lop_tx_buffer[txBufIndex++] = getNetworkTime().frame;
    
    radio.setPALevel((rf24_pa_dbm_e)power);
    sendLoPRANMessage(lop_tx_buffer, txBufIndex);
  }
  
  // Wait till we near the end of the slot.   
  while(getNetworkTime().off < 80)
  {
    delay(1);
  }    
  
  // We build the the BCH sync message according to ...:
  // |0|1|2|3|
  // |B|C|H|S|
        
  int txBufIndex = 5;
        
  // Header
  strncpy(lop_tx_buffer + txBufIndex, "BCHS", 4);
  txBufIndex+=4;
       
  // End marker
  lop_tx_buffer[txBufIndex++] = 0;
        
  // We use max power for sync so we can reach all nodes that might have heard us.
  radio.setPALevel(RF24_PA_MAX);
  sendLoPRANMessage(lop_tx_buffer, txBufIndex);
         
}

///////////////////////////////////////////////////////////////////////////////////////////////
// Attempts to find a node to communicate with.
//
// Performs network scan and ranging.
// This function blocks until a valid inner node is found. On return from this function
//  the globals inbound_channel and inbound_tx_power are set to the right values for the
//  found inner node.
//
void scanForNet()
{
  // Start the scan from the last known good channel.
  inbound_channel = constrain(EEPROM.read(EEPROM_RFCH_INNER_NODE), LOP_LOW_CHANNEL, LOP_HI_CHANNEL);
  
  // We start from an invalid power so we can detect that we
  //  got at least one ranging message avoiding false power
  //  detection if we happen to get as sync as first message.
  inbound_tx_power = RF24_PA_MAX+1;
  
  // Listen on the BCH pipe        
  radio.openReadingPipe(1, BCH_PIPE_ADDR);
  radio.startListening();
  
  while(true)
  { 
    // Try to read a message, linger maximum 1.5s on a given channel and if you get
    //  nothing move on to the next.
    radio.setChannel(inbound_channel);
    if(!receiveLoPRANMessage(lop_rx_buffer, LOP_MTU , 1500, rxBytes))
    {
      inbound_channel = (++inbound_channel % (LOP_HI_CHANNEL - LOP_LOW_CHANNEL)) + LOP_LOW_CHANNEL; 
     
      Serial.print("SCAN,");
      Serial.println(inbound_channel);
      continue; 
    }
    
    if((strstr(lop_rx_buffer, "BCH ") - lop_rx_buffer) == 5)
    {        
      // Store this as last known good channel, save EEPROM life by not
      //  writing if there is no change.
      if(inbound_channel != EEPROM.read(EEPROM_RFCH_INNER_NODE))
        EEPROM.write(EEPROM_RFCH_INNER_NODE, inbound_channel);
                 
      // Process ranging info and store lowest usable power if we heard
      //  a message weaker than last one.
      if(inbound_tx_power > lop_rx_buffer[9])
        inbound_tx_power = lop_rx_buffer[9];
    
      // Sync our nettime
      setNetworkTime(lop_rx_buffer[10], lop_rx_buffer[11], 1);
      
    } 
    else if(inbound_tx_power <= RF24_PA_MAX  && (strstr(lop_rx_buffer, "BCHS") - lop_rx_buffer) == 5)
    {
      // We have a sync. 
      Serial.print(millis());
      Serial.print(",");
      Serial.print("BCHS,");
      Serial.print(getNetworkTime().block);
      Serial.print(",");
      Serial.print(getNetworkTime().frame);
      Serial.print(",");
      Serial.print(inbound_channel);
      Serial.print(",");
      Serial.println(inbound_tx_power);
      lop_rx_buffer[0]=0;
      return;
    }
  }// while(true) 
}



