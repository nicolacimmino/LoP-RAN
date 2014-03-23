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

#ifndef __CRON_H__
#define __CRON_H__

#define EEPROM_CRON_BASE 0x0200
#define EEPROM_CRON_END 0x03FF
#define EEPROM_CRON_SIZE 0x80

#define EEPROM_CRON_HOURS_OFFSET 0
#define EEPROM_CRON_MINUTES_OFFSET 1
#define EEPROM_CRON_SECONDS_OFFSET 2
#define EEPROM_CRON_TASK_OFFSET 3
#define EEPROM_CRON_STAR_BIT 7
#define EEPROM_CRON_SLASH_BIT 6
#define EEPROM_CRON_VAL_MASK 0b00111111

void processCronEntries();

#endif