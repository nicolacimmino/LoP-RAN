// RadioDriver Driver is part of LoP-RAN.
//
//  Copyright (C) 2020 Nicola Cimmino
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

#ifndef __RADIO_DRIVER_H__
#define __RADIO_DRIVER_H__

#include <Arduino.h>

class RadioDriver
{
private:
public:
    virtual void setRXExtendedPreamble(uint64_t extended_preamble) = 0;
    virtual void setTXExtendedPreamble(uint64_t extended_preamble) = 0;
    virtual void setTXPower(uint8_t power) = 0;
    virtual void setRFChannel(uint8_t channel) = 0;
    virtual void send(char *buffer, int length) = 0;
    virtual bool receive(char *buffer, uint8_t wantedBytes, uint16_t timeoutMilliseconds) = 0;
};

#endif
