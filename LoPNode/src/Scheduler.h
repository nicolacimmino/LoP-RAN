// Scheduler is part of LoP-RAN , provides scheduling for the TDMA slots.
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

#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__

#include <Arduino.h>
#include "Scheduler.h"
#include "Common.h"
#include "LoPParams.h"
#include "LoPDia.h"
#include "NetTime.h"
#include "DataLink.h"
#include "ACH.h"
#include "BCH.h"
#include "CCH.h"
#include "OuterNeighboursList.h"
#include "ControlInterface.h"
#include "NRF24L01Driver.h"

void runScheduler(void);
void run_regular_schedule(void);
void run_scanner_schedule(void);
extern void userCode(void);
#endif

