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

#include <Arduino.h>
#include <Serial.h>
#include "Common.h"
#include "ControlInterface.h"
#include "LoPDia.h"

char control_rx_buffer[MAX_CONTROL_MSG_SIZE];
int control_rx_buffer_ix = 0;

void setupControlInterface()
{
  // Empty the message buffer.
  lop_message_buffer[0] = 0;  
}

void serveControlInterface()
{
  if(lop_message_buffer_has_rx_message)
  {
    Serial.print("ATRX ");
    Serial.println(lop_message_buffer);
    lop_message_buffer_has_rx_message = false;
    lop_message_buffer[0]=0;
  }
  
  long start_time = millis();
  
  while(Serial.available()>0)
  {
    control_rx_buffer[control_rx_buffer_ix++] = Serial.read();

    // We have a terminator process the command.
    if(control_rx_buffer[control_rx_buffer_ix-1]=='\n')
    {
       process_control_command();
    }
    
    // We have filled the rx buffer and no termination received.
    // We have a misbehaving host, just ignore the message.
    if(control_rx_buffer_ix>=MAX_CONTROL_MSG_SIZE)
    {
        control_rx_buffer_ix = 0;
        break;
    }
    
  }  
}

void process_control_command()
{
  if(strstr(control_rx_buffer, "ATTX ") - control_rx_buffer == 0)
  {
    // Copy the message body till terination char is found.
    for(int ix=5; ix<MAX_CONTROL_MSG_SIZE; ix++)
    {
      lop_message_buffer[ix-5]=control_rx_buffer[ix];
      
      // We reached the end of the control message. We need
      //  to null terminate the lop message and we are done.
      if(control_rx_buffer[ix]=='\n')
      {
        lop_message_buffer[ix-5]=0;
        break;
      }
    }
  
    Serial.println("OK");  
  }
  else if(strstr(control_rx_buffer, "ATDI0") - control_rx_buffer == 0)
  {
    lop_dia_enabled = false;
    Serial.println("OK"); 
  }
  else if(strstr(control_rx_buffer, "ATDI1") - control_rx_buffer == 0)
  {  
      lop_dia_enabled = true;
      Serial.println("OK"); 
  }
  else if(strstr(control_rx_buffer, "ATNT?") - control_rx_buffer == 0)
  {  
    Serial.println((inner_link_up)?"1":"0");
  }
  else
  {
    Serial.println("ERR");
  }
  
  // Reset reveiving pointer to beginning of buffer.
  control_rx_buffer_ix=0;
}
