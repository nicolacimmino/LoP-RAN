// Cron is part of LoP-RAN , provides managing of the cron functions in a 
//  SmartNode.
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

#include <EEPROM.h>
#include "EEPROMMap.h"
#include "Cron.h"
#include "Common.h"

long lastCronCheck = 0;

void processCronEntries()
{
    // Cron has 1s resolution, if we run this function more
    //  often than once a second we will trigger the same tasks
    //  multiple times.
    if(millis()-lastCronCheck < 1000)
    {
      return;
    }
    lastCronCheck = millis();
    
    // An entry in CRON table has this format (bytes):
    // |HOURS|MINUTES|SECONDS|COMMAND0|....|
    //
    // Each of HOURS, MINUTES, SECONDS has the following format (bits):
    // |/|*|bit5|bit4|bit3|bit2|bit1|bit0|
    //  / bit indicates a /notation. e.g. /5 -> Every 5
    //  * bit indicates don't care, any (value is ignored then)
    // If both / and * are clear it indicates an exact mactch. e.g. 5 at 5 minutes
    //
    // If all bits of the HOURS byte are set (0xFF) the entry is not in use.
    if(EEPROM.read(EEPROM_CRON_BASE) != 0xFF)
    {
      // No RTC at the moment, we just process seconds according to current 
      //   internal clock.
      uint8_t seconds = (millis()/1000) % 60;
      
      bool match = true;
      if(EEPROM.read(EEPROM_CRON_BASE+EEPROM_CRON_SECONDS_OFFSET) & _BV(EEPROM_CRON_SLASH_BIT) != 0)
      {
          match = match & (seconds % ((EEPROM.read(EEPROM_CRON_BASE+EEPROM_CRON_SECONDS_OFFSET) & EEPROM_CRON_VAL_MASK)) == 0);
      }
      
      if(match)
      {
        // If we are the AP (DAP=0) TX means outbound else it means always inbound.
        char *message_buffer = (lop_dap == 0)?lop_message_buffer_o:lop_message_buffer_i;
        for(int cix=0; cix<EEPROM_CRON_SIZE; cix++)
        {
          message_buffer[cix]=EEPROM.read(EEPROM_CRON_BASE+cix+EEPROM_CRON_TASK_OFFSET);
          
          if(message_buffer[cix]=='\0')
          {
            break;
          }
        }
      }
    }
}

