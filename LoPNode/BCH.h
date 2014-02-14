// BCH is part of LoP-RAN , provides control for the Broadcast CHannel.
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

#ifndef __BCH_H__
#define __BCH_H__

///////////////////////////////////////////////////////////////////////////////////////////////
// Offsets inside the RX/TX buffer of the various SDUs elements.
//   These are offsets realtive to the PDU start, not relative to the SDU start. According to
//    the below ASCII art graph the SDU always starts at an offset 5.
//
// +---------+---+---+---+---+---------+---+---+----
// |PDU Start|x  |x  |x  |x  |SDU start|x  |x  |...
// |0        |1  |2  |3  |4  |5        |6  |7  |8
// +---------+---+---+---+---+---------+---+---+----

#define LOP_IX_SDU_BCH_POW  6
#define LOP_IX_SDU_BCH_BLOCK  7
#define LOP_IX_SDU_BCH_FRAME  8
#define LOP_IX_SDU_BCH_DAP  9
#define LOP_LEN_SDU_BCH  10
#define LOP_IX_SDU_BCHS_BLOCK  6
#define LOP_IX_SDU_BCHS_FRAME  7
#define LOP_IX_SDU_BCHS_OFF  8
#define LOP_LEN_SDU_BCHS  9

// Channel used for commincations towards the inner node.
extern uint8_t inbound_channel;

void broadcastBCH();
void innerNodeScanAndSync();

#endif

