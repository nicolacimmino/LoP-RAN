// EEPROMMap is part of LoP-RAN , defines EEPROM memory usage.
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


#define EEPROM_RFCH_INNER_NODE  0x01            // Last known good RF channel
#define EEPROM_RFCH_ACT_AS_SEED 0x02            // Act as anetwork seed if != 0
#define EEPROM_NID_BASE         0x03            // Base of the Network Identifier (8 bytes up to 0x0A)
#define EEPROM_mp2p_UID_BASE    0x0B            // Base of the msgp2p UID (max 40 bytes up to 0x33)
#define EEPROM_mp2p_UID_LEN     40

#define EEPROM_CRON_BASE        0x0200
#define EEPROM_CRON_END         0x03FF
#define EEPROM_CRON_SIZE        0x80            // Size of a single cron entry.

#define EEPROM_CRON_HOURS_OFFSET 0
#define EEPROM_CRON_MINUTES_OFFSET 1
#define EEPROM_CRON_SECONDS_OFFSET 2
#define EEPROM_CRON_TASK_OFFSET 3
#define EEPROM_CRON_STAR_BIT 7
#define EEPROM_CRON_SLASH_BIT 6
#define EEPROM_CRON_VAL_MASK 0b00111111
