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
#include <RF24.h>        // Copyright (C) 2011 J. Coliz <maniacbug@ymail.com>, GNU
#include <EEPROM.h>
#include "Scheduler.h"
#include "Common.h"

void setup(void)
{
  pinMode(2, OUTPUT);
  setupRadio();  
}


void loop(void)
{
  // We sit forever in this call, everything
  //  is timed and scheduled there.
  runScheduler();
}





