// ControlInterface is part of LoP-RAN , provides UART control interface of the node.
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

#ifndef __CONTROLINTERFACE_COMMANDS_H__
#define __CONTROLINTERFACE_COMMANDS_H__

#include <Arduino.h>
#include <Arduino.h>
#include <EEPROM.h>
#include "../Errors.h"
#include "../Common.h"
#include "../LopParams.h"
#include "../LoPDia.h"
#include "../DataLink.h"
#include "../EEPROMMap.h"
#include "../OuterNeighboursList.h"
#include "../L4.h"
#include "../ControlInterface.h"

byte controlATAPn(bool enabled);
byte controlATCFGR();
byte controlATCFGW();
byte controlATADDq();
byte controlATDAPq();
byte controlATDIn(bool enabled);
byte controlATIDq();
byte controlATIPWq();
byte controlATNTq();
byte controlATONLq();
byte controlATRX();
byte controlATRXn(bool enabled);
byte controlATRXq();
byte controlATSCANn(bool enabled);
byte controlATTX();

#endif
