// LoPDia is part of LoP-RAN , provides diagnostic logging to investigate nodes activity.
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
#include "LoPDia.h"
#include "NetTime.h"
#include <stdarg.h>


// Enables the LOP-Diagnostic interface that outputs on serial port
//  a log of the radio activity.
boolean lop_dia_enabled = true;

void dia_logTime()
{
  if(lop_dia_enabled)
  {
    NetTime currentTime = getNetworkTime();
    Serial.print(currentTime.block);
    Serial.print(":");
    Serial.print(currentTime.frame);
    Serial.print(":");
    Serial.print(currentTime.slot);
    Serial.print(".");
    Serial.print(currentTime.off);
    Serial.print(",");
    Serial.print(millis());
    Serial.print(",");
  }
}

void dia_logString(char* string)
{
  if(lop_dia_enabled)
  {
    Serial.print(string);
    Serial.print(",");
  }
}

void dia_logInt(int value)
{
  if(lop_dia_enabled)
  {
   Serial.print(value);
   Serial.print(",");
  }
}

void dia_closeLog()
{
  if(lop_dia_enabled)
  {
    Serial.println();
  } 
}

void dia_simpleFormNumericLog(const char* event, int count, ...)
{
  dia_logTime();
  Serial.print(event);
  
  int argument;

  //We use a va_list macro to go trough arguments
  va_list l_Arg;
  va_start(l_Arg, argument);

  for(int ix=0; ix<count; ix++)
  {
    argument =  va_arg(l_Arg, int);
    Serial.print(",");
    Serial.print(argument);    
  }  
  va_end(l_Arg);

  Serial.println();
}

void dia_simpleFormTextLog(const char* event, const char* text)
{
  dia_logTime();
  Serial.print(event);
  Serial.print(",");
  Serial.println(text);
}

void dia_logBufferToHex(char *data, uint8_t length)
{
  for (int i=0; i<length; i++) 
  { 
    if ((uint8_t)data[i]<0x10) {Serial.print("0");} 
    Serial.print((uint8_t)data[i],HEX); 
    Serial.print(" "); 
  }
}
