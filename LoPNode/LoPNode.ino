// LopNode implements a LoP-RAN , scalable messaging for the Internet of Things
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
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include "NetTime.h"

//
// Change the following consts if your connections are arranged in different way.

#define radio_ce_pin 9        // CE pin for the NRF24L01+
#define radio_csn_pin 10      // CSN pin for the NRF24L01+

// EEPROM Memory map
const byte EEPROM_RFCH_INNER_NODE = 0x01;        // 0x01  Last known good RF channel
const byte EEPROM_URI  = 0x10;        // 0x10  Start of our URI, null terminated max 64 bytes including null
const byte EEPROM_URI_TO = 0x50;        // 0x50  Start of the URI we report to, null terminated max 64 bytes including null

// An instance of the NRF24L01+ chip controller.
RF24 radio(radio_ce_pin,radio_csn_pin);

// BCH Address is common to all nodes.
const uint64_t BCH_PIPE_ADDR = 0x5000000001LL;

// MTU size in Lop-RAN
const uint32_t LopRAN_MTU = 256;

// Single radio packet size.
const uint32_t LoPRAN_payload_size = 4;

// TX Buffer.
char lop_tx_buffer[LopRAN_MTU];

// RX Buffer.
char lop_rx_buffer[LopRAN_MTU];

// TX Power parameter in BCH.
const byte BCH_TXPOW = 0x01;


// Lowest usable power to talk to the inner node.
int lowest_tx_power_inner = RF24_PA_MAX;


// URI of this node, will be loaded from EEPROM_uri address.
String uri = "";

// URI to which we report. This is a single one in this test node, of course
//   different applications might need to talk to different URIs
String uri_to = "";

// Flag, indicates that we have entered a network and we can send data.
bool network_ok = false;

// Count of consecutive TX operation failures
int tx_fail = 0;

// The Interrupt Service Routine, this is called everytime the watchdog interrupt fires.
ISR(WDT_vect)
{
   //We don't do anything here at the moment.
   // Code will resume the execution after the sleep.
   //Serial.println("ISR");
   return;
}

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
  radio.setPayloadSize(LoPRAN_payload_size);                      // Payload sise, phisical layer MTU
  radio.setChannel(EEPROM.read(EEPROM_RFCH_INNER_NODE));   // We start from the last known good channel
  radio.setDataRate(RF24_250KBPS);              // 250 kbps
  radio.setPALevel(RF24_PA_MAX);                // We start from max power, ranging will take care to adjust this.
  radio.setAutoAck(1);                          // Auto ACK enabled
  //radio.openWritingPipe(pipes[0]);              // Just default addresses, each operating mode will set its own.
  //radio.openReadingPipe(1,pipes[1]);
  //radio.startListening();
  
  // Read URI from EEPROM
  int ix=0;
  while(ix<64)
  {
    //EEPROM.write(EEPROM_URI+ix,"node1@nicolacimmino.com\0"[ix]);
    if(EEPROM.read(EEPROM_URI+ix)==0) break;
    uri = uri + (char)EEPROM.read(EEPROM_URI+ix);
    ix++; 
  }
  
  // Read URI_TO from EEPROM
  ix=0;
  while(ix<64)
  {
    //EEPROM.write(EEPROM_URI_TO+ix,"logger@nicolacimmino.com\0"[ix]);
    if(EEPROM.read(EEPROM_URI_TO+ix)==0) break;
    uri_to = uri_to + (char)EEPROM.read(EEPROM_URI_TO+ix);
    ix++; 
  }
  
  // We setup here the Watchdog timer. Sleep duration will be set before sleep anyway.
  MCUSR &= ~(1<<WDRF);  // Clear reset flag 
  WDTCSR |= (1<<WDCE) | (1<<WDE); // Setting WDCE gives us 4 clock cycles time to change WDE
  WDTCSR =  1<<WDP2 | 1<<WDP1; // Set default sleep to 1 second.
  WDTCSR |= _BV(WDIE);  // Enable watchdog interrupt.
  
}


void DumpToSerial(char *data, uint8_t length)
{
  for (int i=0; i<length; i++) 
  { 
    if (data[i]<0x10) {Serial.print("0");} 
    Serial.print(data[i],HEX); 
    Serial.print(" "); 
  }
  Serial.println();
}

void sleep_0_5_s()
{
  MCUSR &= ~(1<<WDRF);
  WDTCSR |= (1<<WDCE) | (1<<WDE);
  WDTCSR =  1<<WDP2 | 1<<WDP0;
  WDTCSR |= _BV(WDIE);
  
  // Enter sleep mode
    //logMessage("Sleeping");
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);  
    sleep_enable();
    sleep_mode();
    
    // We come here after the WDT wakes us up
    // Disable sleep and power up all peripherals
    sleep_disable(); 
    power_all_enable();
}

void loop(void)
{
      //sleep_0_5_s();
      
      /*unsigned long start = millis();
      int toff_correction = tx_slot();
      int tspent = (millis() - start) % 1000;
      if(tspent < 365 && (toff_correction + 365-tspent) > 0)
        delay(toff_correction + 365-tspent);
      else if(toff_correction + 1365-tspent > 0)
        delay(toff_correction + 1365-tspent);
      */
      
      radio.setChannel(50);
      
      /*while(getNetworkTime().slot != 0)
      { 
        delay(1);
      }
      broadcastBCH();
      */
      //scanForNet();
      
      
}



void sendLoPRANMessage(char *data, int len)
{
  radio.stopListening();
  int offset=0;
  while(offset < len)
  {  
    radio.write(data+offset, LoPRAN_payload_size);
    delay(1);
    offset+=LoPRAN_payload_size;
  }
  radio.startListening();
}

bool receiveLoPRANMessage(char *data, uint32_t bufLen, int timeout_ms, int &received)
{
  long started_reading = millis();
  int messagelen = 4;
  bool timeout = false;
  
  radio.startListening();
  
  received = 0;
  while(received < messagelen)
  {
    while ( !radio.available() && !timeout)
    {
      timeout = (millis() - started_reading > timeout_ms);
    }
    
    // Fail the operation if we timeout or we receive more data
    //  than the supplied buffer can contain.
    if(timeout || (received + LoPRAN_payload_size > bufLen))
    {
      received = -1;
      return false;
    }
	
    radio.read( data + received , LoPRAN_payload_size );
    DumpToSerial(data, LoPRAN_payload_size);
    
    received += LoPRAN_payload_size;
    
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
  NetTime currentNetTime = getNetworkTime();
  
  radio.stopListening();

  // Keep braodcasting for the all slot 0 duration.
  while(currentNetTime.slot == 0)
  {
    radio.setAutoAck(1);         
    radio.openWritingPipe(BCH_PIPE_ADDR);
    
    int txBufIndex = 0;
   
    // To allow ranging we step down power from 0dBm to -18dBm
    for(byte power=3;; power--)
    {
      txBufIndex = 0;
 
      currentNetTime = getNetworkTime();
      
      if(currentNetTime.off < 70)
      {
        // We are before off=70, we broadcast the BCH and ranging info.
        
        // We build the the BCH message according to ...:
        // |0|1|2|3|4  |5        |6   |7  |
        // |B|C|H| |Len|BCH_TXPOW|Power|END|
        
        // Header
        strncpy(lop_tx_buffer, "BCH ", 4);
        txBufIndex+=4;
        
        // Place holder for length.
        lop_tx_buffer[txBufIndex++] = 0;
        
        // The power
        lop_tx_buffer[txBufIndex++] = BCH_TXPOW;
        lop_tx_buffer[txBufIndex++] = power;
        
        // End marker
        lop_tx_buffer[txBufIndex++] = 0;
        
        // Length
        lop_tx_buffer[4]=txBufIndex;
        
        radio.setPALevel((rf24_pa_dbm_e)power);
        sendLoPRANMessage(lop_tx_buffer, txBufIndex);
      }
      else
      {
        // We are past off=70 we broadcast sync
        // We build the the BCH sync message according to ...:
        // |0|1|2|3|4   |
        // |B|C|H|S||END|
        
        // Header
        strncpy(lop_tx_buffer, "BCHS", 4);
        txBufIndex+=4;
        
        // Place holder for length.
        lop_tx_buffer[txBufIndex++] = 0;
        
        // End marker
        lop_tx_buffer[txBufIndex++] = 0;
        
        // Length
        lop_tx_buffer[4]=txBufIndex;
        
        // We use max power for sync so we can reach all nodes
        //  that might have heard us.
        radio.setPALevel(RF24_PA_MAX);
        sendLoPRANMessage(lop_tx_buffer, txBufIndex);
        return;
      }
      
      if(power == 0) break;
    }
    
  }
  
  radio.startListening();
}


// Attempts to find an AP where to connect.
void scanForNet()
{
  // Start the scan from the last known good channel.
  int scan_start = EEPROM.read(EEPROM_RFCH_INNER_NODE);
  if(scan_start>15)
   scan_start = 0;
  int rxBytes = 0;
  lowest_tx_power_inner = RF24_PA_MAX;
  
  radio.stopListening();
  radio.setAutoAck(1);         
  radio.openReadingPipe(1, BCH_PIPE_ADDR);
  
  while(true)
  {   
    for(int ch=scan_start; ch<15; ch++)
    {
       radio.setChannel(ch);
       
       bool res = receiveLoPRANMessage(lop_rx_buffer, LopRAN_MTU , 1500, rxBytes);
       if(strstr(lop_rx_buffer, "BCH ") - lop_rx_buffer == 0)
       {        
         // Store this as last known good channel
         if(ch != EEPROM.read(EEPROM_RFCH_INNER_NODE))
           EEPROM.write(EEPROM_RFCH_INNER_NODE, ch);
                 
         // Process ranging info to store lowest received
         //  power.
         if(lowest_tx_power_inner > lop_rx_buffer[6])
         {
           lowest_tx_power_inner = lop_rx_buffer[6];
         }
       } 
       else if(strstr(lop_rx_buffer, "BCHS") - lop_rx_buffer == 0)
      {
        Serial.print("BCH SYNC. POW=");
        Serial.println(lowest_tx_power_inner);
        return;
      } 
    }
    
    // Not found, start from the first channel.
    scan_start = 0;
  }
}


