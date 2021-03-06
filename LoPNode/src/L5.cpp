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

#include "L5.h"

namespace L5
{

///////////////////////////////////////////////////////////////////////////////////////////////
// Invoked every time the node performs a successfull network entry.
//
void onL4LinkUp()
{
  char message[16];
  uint32_t noid;
  EEPROM.get(EEPROM_NOID_BASE, noid);

  sprintf(message, "ENTER 0x%08X", noid);

  sendMessage(0, message);
}

} // namespace L5
