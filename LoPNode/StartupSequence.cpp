// StartupSequence is part of LoP-RAN , provides basic configuration at
//  SmartNode startup.
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
#include "Arduino.h"
#include "Common.h"

uint8_t last_startup_step = 0;

void processStartupSequence()
{
  
}

void startupOnNetRegistration()
{
  strcpy(lop_message_buffer_i, "\\msgp2p.register\\A2BXJV2ZMIDCL2IPRINBE44SQFNXU3WHO\\\0"); 
}
