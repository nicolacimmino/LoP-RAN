// LopNode implements a LoP-RAN , a low power radio access network.
// This is just the Arduino module, all the code is in the CPP files
//  in this way it can be easily compiled on other platforms. This file
//  provides only the entry points for the Arduino runtime (steup and loop).
//
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
#include <EEPROM.h>
#include "EEPROMMap.h"
#include "Scheduler.h"
#include "Common.h"
#include "NRF24L01Driver.h"
#include "ControlInterface.h"
#include "Cron.h"
void setup(void)
{
  // For demo purposes we have a LED between D5 and D6
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  digitalWrite(5,0);
  digitalWrite(6,0);
  
  pinMode(2, OUTPUT);
  setupControlInterface();
  setupRadio();  
  
  /*
  EEPROM.write(EEPROM_CRON_BASE + EEPROM_CRON_HOURS_OFFSET, 0xFF);   // H
  
  //For testing purpose prepare a cron entry in EEPROM while we don't have
  //  AT commands for that.
  EEPROM.write(EEPROM_CRON_BASE + EEPROM_CRON_HOURS_OFFSET, 0x80);   // H
  EEPROM.write(EEPROM_CRON_BASE + EEPROM_CRON_MINUTES_OFFSET, 0x80); // M
  EEPROM.write(EEPROM_CRON_BASE + EEPROM_CRON_SECONDS_OFFSET, 69);   // S
  for(int ix=0;ix<64;ix++)
  {
    EEPROM.write(EEPROM_CRON_BASE+EEPROM_CRON_TASK_OFFSET+ix,"\\http.get\\http://iotp2p.net:4000/api/node/aW90cDJwdG/alive\\\\\0"[ix]); 
  }
  */
  pinMode(A5, INPUT);
}


void loop(void)
{
  // We sit forever in this call, everything
  //  is timed and scheduled there.
  runScheduler();
}

bool lastDoorStatus = false;

// User code invoked at every frame.
// Must run witing one timeslot.
//
void userCode()
{
  return;
  if(lastDoorStatus && digitalRead(A5)==LOW)
  {
    char *message_buffer = (lop_dap == 0)?lop_message_buffer_o:lop_message_buffer_i; 
    for(int cix=0; cix<74; cix++)
    {
      message_buffer[cix]="\\http.get\\http://nicolacimmino.com/coeli/pushover.php?msg=Door%20Closed\\\\\0"[cix]; 
    }
    lastDoorStatus = false;
  }
  else if(!lastDoorStatus && digitalRead(A5)==HIGH)
  {
    char *message_buffer = (lop_dap == 0)?lop_message_buffer_o:lop_message_buffer_i;
    for(int cix=0; cix<72; cix++)
    {
      message_buffer[cix]="\\http.get\\http://nicolacimmino.com/coeli/pushover.php?msg=Door%20Open\\\\\0"[cix]; 
    }
    lastDoorStatus = true;
  }
}




