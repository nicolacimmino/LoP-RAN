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
#include "src/EEPROMMap.h"
#include "src/Scheduler.h"
#include "src/Common.h"
#include "src/NRF24L01Driver.h"
#include "src/ControlInterface.h"
#include "src/L4.h"

void setup(void)
{
  // EEPROM.write(EEPROM_HW_VERSION, 2);
  // EEPROM.write(EEPROM_RFCH_INNER_NODE, 50);
  // EEPROM.write(EEPROM_RFCH_ACT_AS_SEED, 0);
  // EEPROM.write(EEPROM_NTID_BASE + 0, 0x05); // NID
  // EEPROM.write(EEPROM_NTID_BASE + 1, 0x05);
  // EEPROM.write(EEPROM_NTID_BASE + 2, 0x05);
  // EEPROM.write(EEPROM_NTID_BASE + 3, 0x05);
  // EEPROM.write(EEPROM_NTID_BASE + 4, 0x05);
  // EEPROM.write(EEPROM_NTID_BASE + 5, 0x05);
  // EEPROM.write(EEPROM_NTID_BASE + 6, 0x05);
  // EEPROM.write(EEPROM_NTID_BASE + 7, 0x05);
  // uint32_t noid = 0x015A4502;
  // EEPROM.put(EEPROM_NOID_BASE, noid);

  pinMode(PIN_ACT, OUTPUT);
  setupControlInterface();
  setupRadio();
}

void loop()
{
  // We sit forever in this call, everything
  //  is timed and scheduled there.
  runScheduler();
}

// User code invoked at every frame.
// Must run witin one timeslot.
//
void userCode()
{
  static unsigned long lastPing = 0;

  if (millis() - lastPing < 1000)
  {
    return;
  }

  if (lop_dap != 0 && inner_link_up)
  {
    char message[32];
    uint32_t noid;
    EEPROM.get(EEPROM_NOID_BASE, noid);

    sprintf(message, "PING 0x%08lX %u", noid, millis()/1000);

    sendMessage(0, message);
  }

  lastPing = millis();
}
