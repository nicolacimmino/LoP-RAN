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

#include "Errors.h"
#include <Arduino.h>
#include <EEPROM.h>
#include "EEPROMMap.h"
#include "Common.h"
#include "ControlInterface.h"
#include "LoPDia.h"
#include "OuterNeighboursList.h"
#include "LoPParams.h"
#include "DataLink.h"
#include "L4.h"
#include "ControlInterface/ControlInterfaceCommands.h"

char control_rx_buffer[MAX_CONTROL_MSG_SIZE];
int control_rx_buffer_ix = 0;
boolean rx_notification_enable = true;

void setupControlInterface()
{
  // Empty the message buffer.
  lop_message_buffer_o[0] = 0;
  lop_message_buffer_i[0] = 0;

  Serial.begin(115200);
}

void notifyReceivedMessage()
{
  // If we are the AP (DAP=0) RX means outbound else it means always outbound.
  char *message_buffer_rx = (lop_dap == 0) ? lop_message_buffer_i : lop_message_buffer_o;

  if (message_buffer_rx[0] != 0)
  {
    // Respond to icmp_ping requests other stuff we let up to the host.
    if ((strcasestr(message_buffer_rx, "\\icmp.ping_request\\") - message_buffer_rx) == 0)
    {
      strcpy(lop_message_buffer_i, "\\icmp.ping_response\\\\");
      lop_message_buffer_i[21] = 0;
    }
    else if ((strcasestr(message_buffer_rx, "\\msgp2p.msg\\0000\\\\LED=1") - message_buffer_rx) == 0)
    {
      digitalWrite(6, 1);
    }
    else if ((strcasestr(message_buffer_rx, "\\msgp2p.msg\\0000\\\\LED=0") - message_buffer_rx) == 0)
    {
      digitalWrite(6, 0);
    }
    else
    {
      // Prefix the message only if this is an unsolicited notification
      // For ATRX? queries we just reply with address/message.
      if (rx_notification_enable)
        Serial.print("ATRX ");

      // Print the address if we are the AP
      if (lop_dap == 0)
      {
        Serial.print(lop_message_buffer_address_i);
        Serial.print(" ");
      }
      Serial.println(message_buffer_rx);
    }
    message_buffer_rx[0] = 0;
  }
}

#define TERMINAL_KEY_ESC 0x1B
#define TERMINAL_KEY_BACKSPACE 0x7F

void serveControlInterface()
{
  unsigned long start_time = millis();

  if (rx_notification_enable)
  {
    notifyReceivedMessage();
  }

  while (Serial.available() > 0)
  {
    char c = Serial.read();

    if (c == TERMINAL_KEY_BACKSPACE)
    {
      control_rx_buffer[control_rx_buffer_ix] = 0;
      control_rx_buffer_ix--;
      continue;
    }

    control_rx_buffer[control_rx_buffer_ix++] = c;

    // We have a terminator process the command.
    if (control_rx_buffer[control_rx_buffer_ix - 1] == '\n' || control_rx_buffer[control_rx_buffer_ix - 1] == '\r')
    {
      // Make sure the terminator is \n cause we rely on that later
      //  (some terminals send \r)
      control_rx_buffer[control_rx_buffer_ix - 1] = '\n';

      process_control_command();
    }

    // We have filled the rx buffer and no termination received.
    // We have a misbehaving host, just ignore the message.
    if (control_rx_buffer_ix >= MAX_CONTROL_MSG_SIZE)
    {
      control_rx_buffer_ix = 0;
      break;
    }
  }
}

void process_control_command()
{
  Serial.println("");

  char *command = strtok(control_rx_buffer, " \n");

  byte result = 0;

  if (strcasecmp(command, "ATTX") == 0)
  {
    result = controlATTX();
  }
  else if (strcasecmp(command, "ATID?") == 0)
  {
    result = controlATIDq();
  }
  else if (strcasecmp(command, "ATDI1") == 0)
  {
    result = controlATDIn(true);
  }
  else if (strcasecmp(command, "ATDI0") == 0)
  {
    result = controlATDIn(false);
  }
  else if (strcasecmp(command, "ATSCAN1") == 0)
  {
    result = controlATSCANn(true);
  }
  else if (strcasecmp(command, "ATSCAN0") == 0)
  {
    result = controlATSCANn(false);
  }
  else if (strcasecmp(command, "ATCFGR") == 0)
  {
    result = controlATCFGR();
  }
  else if (strcasecmp(command, "ATCFGW") == 0)
  {
    result = controlATCFGW();
  }
  else if (strcasecmp(command, "ATRX") == 0)
  {
    result = controlATRX();
  }
  else if (strcasecmp(command, "ATRX1") == 0)
  {
    result = controlATRXn(true);
  }
  else if (strcasecmp(command, "ATRX0") == 0)
  {
    result = controlATRXn(false);
  }
  else if (strcasecmp(command, "ATRX?") == 0)
  {
    result = controlATRXq();
  }
  else if (strcasecmp(command, "ATNT?") == 0)
  {
    result = controlATNTq();
  }
  else if (strcasecmp(command, "ATIPW?") == 0)
  {
    result = controlATIPWq();
  }
  else if (strcasecmp(command, "ATDAP?") == 0)
  {
    result = controlATDAPq();
  }
  else if (strcasecmp(command, "ATADD?") == 0)
  {
    result = controlATADDq();
  }
  else if (strcasecmp(command, "ATAP1") == 0)
  {
    result = controlATAPn(true);
  }
  else if (strcasecmp(command, "ATAP0") == 0)
  {
    result = controlATAPn(false);
  }
  else if (strcasecmp(command, "ATONL?") == 0)
  {
    result = controlATONLq();
  }
  else if (strcasecmp(command, "AT") == 0)
  {
    result = ERROR_NONE;
  }
  else
  {
    result = ERROR_UNKNOWN_COMMAND;
  }

  if (result == ERROR_NONE)
  {
    Serial.println("OK");
  }
  else
  {
    Serial.print("ERR:");
    Serial.println(result);
  }

  // Reset receiving pointer to beginning of buffer.
  control_rx_buffer_ix = 0;
}
