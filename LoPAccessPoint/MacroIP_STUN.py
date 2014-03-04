# MacroIP_STUN is part of MacroIP Core. Provides Access to STN services through simple
# textual macros.
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
#
# We don't really lease addressed from a DHCP server here. Instead our box has 
#   a pool of IP address aliases that are for us to distribute to our client.
# This has the benefit of not requiring to modify on the fly our network config
#   since in the end we always would need to have the addresses assigned to
#   this box in oder to get the traffic. This has the disavantage to require
#   a range of private IPs to be reserved for out use.

import stun_client

outputMacrosQueue = []

def processMacro(clientid, macro):
  
  if macro.startswith("stun.discover\\"):
    params = macro.split("\\")
    local_port = int(params[1])
    public_transport = stun_client.perfomSTUNDiscovery(local_port)
    if public_transport != None:
      outputMacrosQueue.append((clientid,  "\\stun.pubtrans\\" + str(public_transport[0]) + "\\" + str(public_transport[1]) + "\\\\"))
    else:
      outputMacrosQueue.append((clientid,  "\\stun.error\\\\"))
    
def getOutputMacroIPMacro():
  if len(outputMacrosQueue) > 0:
    return outputMacrosQueue.pop(0)
  else:
    return (None, None)
    
