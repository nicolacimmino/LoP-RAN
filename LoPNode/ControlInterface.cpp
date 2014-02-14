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
#include <EEPROM.h>
#include "Common.h"
#include "ControlInterface.h"
#include "LoPDia.h"
#include "OuterNeighboursList.h"
#include "LoPParams.h"
#include "DataLink.h"

char control_rx_buffer[MAX_CONTROL_MSG_SIZE];
int control_rx_buffer_ix = 0;

void setupControlInterface()
{
  // Empty the message buffer.
  lop_message_buffer_o[0] = 0;
  lop_message_buffer_i[0] = 0;
}

void serveControlInterface()
{
  // If we are the AP (DAP=0) RX means outbound else it means always outbound.
  char* message_buffer_rx = (lop_dap == 0)?lop_message_buffer_i:lop_message_buffer_o;
  
  if(lop_message_buffer_has_rx_message)
  {
    Serial.print("ATRX ");
    
    // Print the address if we are the AP
    if(lop_dap == 0)
    {
      for(int ix=0; ix<LOP_ADDRESS_SIZE_NIBBLES; ix++)
      {
        Serial.print('0' + lop_message_buffer_address_i[ix]);
      }
      Serial.print(" ");
    }
    Serial.println(message_buffer_rx);
    lop_message_buffer_has_rx_message = false;
    message_buffer_rx[0]=0;
  }
  
  long start_time = millis();
  
  while(Serial.available()>0)
  {
    control_rx_buffer[control_rx_buffer_ix++] = Serial.read();

    // We have a terminator process the command.
    if(control_rx_buffer[control_rx_buffer_ix-1]=='\n' 
        || control_rx_buffer[control_rx_buffer_ix-1]=='\r')
    {
       // Make sure the terminator is \n cause we rely on that later
       //  (some terminals send \r)
       control_rx_buffer[control_rx_buffer_ix-1]='\n';
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
    // If we are the AP (DAP=0) TX means outbound else it means always inbound.
    char *message_buffer = (lop_dap == 0)?lop_message_buffer_o:lop_message_buffer_i;
    
    // Copy the address if we are the AP
    if(lop_dap == 0)
    {
      for(int ix=0; ix<LOP_ADDRESS_SIZE_NIBBLES; ix++)
      {
        message_buffer[ix] = '0' - control_rx_buffer[ix+5];
      }
    }
    
    // Copy the message body till terination char is found.
    // Start offset of message depends on presence of address. TODO: cleanup!
    for(int ix=0; ix<MAX_CONTROL_MSG_SIZE; ix++)
    {
      int ctrl_buf_ix = ix+5+((lop_dap == 0)?(LOP_ADDRESS_SIZE_NIBBLES+1):0);
      message_buffer[ix]=control_rx_buffer[ctrl_buf_ix];
      
      // We reached the end of the control message. We need
      //  to null terminate the lop message and we are done.
      if(control_rx_buffer[ctrl_buf_ix]=='\n')
      {
        message_buffer[ix]=0;
        break;
      }
    }
    Serial.println("OK");  
  }
  else if(strstr(control_rx_buffer, "ATID?") - control_rx_buffer == 0)
  {
    Serial.println("LoP-RAN RadioFW 0.1");
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
    Serial.println("OK");
  }
  else if(strstr(control_rx_buffer, "ATIPW?") - control_rx_buffer == 0)
  {  
    Serial.println(inbound_tx_power);
    Serial.println("OK");
  }
  else if(strstr(control_rx_buffer, "ATDAP?") - control_rx_buffer == 0)
  {  
    Serial.println(lop_dap);
    Serial.println("OK");
  }
  else if(strstr(control_rx_buffer, "ATADD?") - control_rx_buffer == 0)
  {  
    for(int ix=0; ix<LOP_ADDRESS_SIZE_NIBBLES; ix++)
    {
      Serial.print(node_address[ix],HEX);
      if(ix%2==1 && ix!=LOP_ADDRESS_SIZE_NIBBLES-1)
      {
         Serial.print(".");
      }
    }
    Serial.println("");
    Serial.println("OK");
  }
  else if(strstr(control_rx_buffer, "ATAP1") - control_rx_buffer == 0)
  {  
    lop_dap = 0;
    EEPROM.write(EEPROM_RFCH_ACT_AS_AP, 1);
    Serial.println("OK");
  }
  else if(strstr(control_rx_buffer, "ATAP0") - control_rx_buffer == 0)
  {  
    lop_dap = 0xFF;
    EEPROM.write(EEPROM_RFCH_ACT_AS_AP, 0);
    Serial.println("OK");
  }
  else if(strstr(control_rx_buffer, "ATONL?") - control_rx_buffer == 0)
  {  
    for(int ix=0; ix<LOP_MAX_OUT_NEIGHBOURS; ix++)
    {
      if(OuterNeighboursList[ix] != 0)
      {
        Serial.print(OuterNeighboursList[ix]->tx_power);
        Serial.print(",");
        Serial.print(OuterNeighboursList[ix]->resourceMask.slot);  
        Serial.print(",");
        Serial.print((int)constrain(LOP_ONL_ALLOCATION_TTL - (millis() - OuterNeighboursList[ix]->last_seen), 0, LOP_ONL_ALLOCATION_TTL)); 
        Serial.print(",");
        for(int ix=0; ix<LOP_ADDRESS_SIZE_NIBBLES; ix++)
        {
          Serial.print(OuterNeighboursList[ix]->address[ix],HEX);
          if(ix%2==1 && ix!=LOP_ADDRESS_SIZE_NIBBLES-1)
          {
             Serial.print(".");
          }
        }
        Serial.println("");
      }
    }
    Serial.println("OK");
  }
  else if(strstr(control_rx_buffer, "ATME?") - control_rx_buffer == 0)
  {  
    // The below chunk of code calculates amount of free memory.
    // This code is found in several forums and blogs but I never found
    //  an attribution for it, so I cannot give proper credit.
    // To uderstand what goes on here we need to remember that the stack
    //  grows from the bottom of memory up and the heap from the top down.
    // The code creates an int, which will be created on the stack as it is
    //  a local variable, so the address of v will be the current top of the
    //  stack. It will then subtract this address from the current highest 
    //  heap address that is found in the system variable __brkval unless
    //  the heap is empty in which case it will be __heap_start.
    extern int __heap_start, *__brkval; 
    int v; 
    Serial.println((int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval));
    Serial.println("OK");
  }
  else
  {
    Serial.println("ERR");
  }
  
  // Reset reveiving pointer to beginning of buffer.
  control_rx_buffer_ix=0;
}
