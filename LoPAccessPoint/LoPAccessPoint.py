# LoPAccessPoint. Provides Access Point functionality in LoP-R(A)N.
#   Copyright (C) 2014 Nicola Cimmino
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see http://www.gnu.org/licenses/.
#
# This service expects a LoPNode connected on serial port ttyUSB0 and set
#   to access point mode already (ATAP1). In due time autodiscovery and
#   configuration will be built.

import serial
import MacroIP

# This is stuff that will come from a config file.
serial_device_name = "/dev/ttyUSB0"
serial_speed = 115200
serial_open = False

raw_packet_data = ""

MacroIP.startActivity()

# We mainly sit here waiting serial traffic ad delivering the
#  relevant messages to the relevant sub system.
while True:
  try:
    if not serial_open:
      # Open the serial port we keep a low timeout so the monitoring
      #  loop is not stuck too long in a blocking call.
      ser = serial.Serial(serial_device_name, serial_speed, timeout=0.2)
      ser.close()
      ser.open()
      serial_open = True
      
    # We keep queuing incoming data until at least one
    #  complete line is found ("\n")
    raw_packet_data += ser.read(10000)
    
    if raw_packet_data.find("\n") > 0:
    
      # Some terminals might send also \r we just ignore those.
      raw_packet_data = raw_packet_data.translate(None, "\r")
    
      # If there is no incomplete line this will trim awaty the last \n
      #   and if there is an incomplete line this will trim that one away
      raw_lines = raw_packet_data[:raw_packet_data.rfind("\n")].split("\n")
      
      # If there is a \n in the string but it's not the last char it means
      #  there is a partial line received e.g. OK\nATR
      # We lease the last incomplete line in the incoming data buffer and
      #  process the rest.
      if raw_packet_data.rfind("\n") != len(raw_packet_data):
        raw_packet_data = raw_packet_data[raw_packet_data.rfind("\n")+1:]
      else:
        raw_packet_data = ""
      
      for raw_line in raw_lines:
        
        # Ignore "OK" they are replies to sent commands.
        if raw_line == "OK":
          continue;
        
        if raw_line.startswith("ATRX "):
          # The LoP Address from which the packet came 
          lop_address = int(raw_line[5:(raw_line[5:].find(" ")+5)])

          # The directive for the IPGateway is everything from the first \ to the first \\
          macro = raw_line[raw_line.find("\\") + 1 :]

          MacroIP.processMacroIPMacro(lop_address, macro)       
      
    # See if there are macros to send out
    (lop_address, macro) = MacroIP.getOutputMacroIPMacro()
    if lop_address != None:
      ser.write("ATTX " + str(lop_address) + " " + macro + "\n")
      
  except OSError:
    print "Error in serial communication. Attempting serial restart."
    serial_open = False
  except:
    print "Error processing serial data. Ignoring command."
