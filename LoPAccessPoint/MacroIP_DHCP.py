# MacroIP_DHCP is part of MacroIP Core. Provides Access to DHCP services through simple
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

# This will come from config and it will be speficied as a range
addresses_pool = [ "192.168.0.200", "192.168.0.201", "192.168.0.202", "192.168.0.203", "192.168.0.204", 
                    "192.168.0.205", "192.168.0.206", "192.168.0.207", "192.168.0.208", "192.168.0.209" ]

leased_ips = {}
outputMacrosQueue = []

def processMacro(clientid, macro):
  
  if macro.startswith("dhcp.lease\\"):
    # For now we have a very simple "DHCP" sevice, just always assing IP based on client ID.
    # This works only for proof of concept as we won't have ids above 9 anywat.
    leased_ips[clientid] = addresses_pool[clientid]
    print "Leased: " + leased_ips[clientid] + " to " + str(clientid)
    outputMacrosQueue.append((clientid,  "\\dhcp.ip\\" + str(leased_ips[clientid]) + "\\\\"))
    
def getOutputMacroIPMacro():
  if len(outputMacrosQueue) > 0:
    return outputMacrosQueue.pop(0)
  else:
    return (None, None)
    
def getIP(clientid):
  if clientid in leased_ips.keys():
    return leased_ips[clientid]
  else:
    return None
    
def getClientID(local_ip):
  for clientid in leased_ips:
    if leased_ips[clientid] == local_ip:
      return clientid
  
  return 0
  