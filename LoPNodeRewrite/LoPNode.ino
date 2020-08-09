// LopNode implements a LoP-RAN , a low power radio access network.
// This is just the Arduino module, all the code is in the CPP files
//  in this way it can be easily compiled on other platforms. This file
//  provides only the entry points for the Arduino runtime (steup and loop).
//
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

#include "src/PHY/NRF24L01RadioDriver.h"
#include "src/EEPROMMap.h"

NRF24L01RadioDriver *radio;

void setup(void)
{
  Serial.begin(115200);
  Serial.println("Setup");
  byte hwVersion = EEPROM.read(EEPROM_HW_VERSION);
  Serial.println("creating radio");
  radio = new NRF24L01RadioDriver(hwVersion);
  Serial.println("created radio");

  radio->setRXExtendedPreamble(643234);
  radio->setTXExtendedPreamble(643234);
  radio->setTXPower(0);
  radio->setRFChannel(85);

  Serial.println("Setup done");
}

uint8_t r = 0;
char buffer[64];
uint8_t dataBufferSize = 64;

#define ACT_AS_TX

void loop()
{

#ifdef ACT_AS_TX
  memset(buffer, 0, 64);
  memset(buffer, 'A', 1 + (r++ % 62));
  //sprintf(buffer, "TEST MESSAGE OF FIXED LENGTH  AND SOME CHANGING ab DATA %i", r++);
  //               1234567890123456789012345678901212345678901234567890123456789012
  //               0        1         2         3           4         5         6
  radio->send(buffer, strlen(buffer));
  radio->receive(buffer, &dataBufferSize, 500);
  Serial.println(buffer);
  delay(200);
#else

  dataBufferSize = 64;
  if (radio->receive(buffer, &dataBufferSize, 1000))
  {
    Serial.println(dataBufferSize);
    //Serial.println(buffer);
    delay(100);
    strcpy(buffer, "OK");
    radio->send(buffer, strlen(buffer));
  }
  else
  {
    sprintf(buffer, "TOUT: %i OVFL:%i OOS:%i", radio->errTOUT, radio->errOVFL, radio->errOOS);
    Serial.println(buffer);
  }
#endif
}
