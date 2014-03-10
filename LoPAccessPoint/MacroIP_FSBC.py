# MacroIP_FSBC is part of MacroIP Core. Provides Access to FSBC services through simple
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

outputMacrosQueue = []
fsbc_clients = {}

def processMacro(clientid, macro):
  
  if macro.startswith("fsbc.register\\"):
    params = macro.split("\\")
    uniqueid = params[1]
    setUniqueID(clientid, uniqueid)
    outputMacrosQueue.append((clientid,  "\\fsbc.ok\\\\"))

  if macro.startswith("fsbc.send\\"):
    params = macro.split("\\")
    uniqueid = params[1]
    data_to_send = macro[ macro.find("\\\\") + 2:]
    fsbc_clients[clientid].sendMessage(uniqueid, data_to_send)
    outputMacrosQueue.append((clientid,  "\\fsbc.ok\\\\"))

def process_fsbc_message(destinationid, senderid, message, replyreuested, connectiontype):
    outputMacrosQueue.append((getClientID(destinationid),  "\\fsbc.msg\\" + senderid + "\\" + message + "\\\\"))

def getOutputMacroIPMacro():
  if len(outputMacrosQueue) > 0:
    return outputMacrosQueue.pop(0)
  else:
    return (None, None)
    
      
def getClientID(uniqueid):
  for clientid in fsbc_clients.keys():
    if fsbc_clients[clientid].uniqueid == uniqueid:
      return clientid
  return 0
  
def setUniqueID(clientid, uniqueid):
  fsbc = fsbc(uniqueid, process_fsbc_message)
  fsbc_clients[clientid] = fsbc
    