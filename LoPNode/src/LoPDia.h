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

#ifndef __LopDia_h__
#define __LopDia_h__

#include <Arduino.h>
#include <stdarg.h>
#include "Arduino.h"
#include "LoPDia.h"
#include "NetTime.h"

extern boolean lop_dia_enabled;

void dia_logTime();
void dia_logString(char *string);
void dia_logInt(int value);
void dia_closeLog();
void dia_simpleFormNumericLog(const char *event, int count, ...);
void dia_simpleFormTextLog(const char *event, const char *text);
void dia_logBufferToHex(char *data, uint8_t length);
void dia_logRawTime();
void dia_logRawString(char *string);

#endif
