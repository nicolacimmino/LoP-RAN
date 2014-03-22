# MacroIP_MSGP2P is part of MacroIP Core. Provides Access to msgp2p services through simple
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

from msgp2p import msgp2p

outputMacrosQueue = []
msgp2p_clients = {}

def processMacro(clientid, macro):
  
  if macro.startswith("msgp2p.register\\"):
    print "Reg received"
    params = macro.split("\\")
    localUID = params[1]
    setUniqueID(clientid, localUID)
    outputMacrosQueue.append((clientid,  "\\msgp2p.ok\\\\"))

  if macro.startswith("msgp2p.send\\"):
    params = macro.split("\\")
    remoteUID = params[1]
    data_to_send = macro[ macro.find("\\\\") + 2:]
    msgp2p_clients[clientid].sendMessage(remoteUID, data_to_send)
    outputMacrosQueue.append((clientid,  "\\msgp2p.ok\\\\"))

def process_msgp2p_message(localUID, remoteUID, message, logicalchannel):
    print "Received message :" 
    print remoteUID
    print localUID
    print message
    outputMacrosQueue.append((getClientID(localUID),  "\\msgp2p.msg\\" + remoteUID + "\\\\" + message))

def getOutputMacroIPMacro():
  if len(outputMacrosQueue) > 0:
    return outputMacrosQueue.pop(0)
  else:
    return (None, None)
    
      
def getClientID(uid):
  for clientid in msgp2p_clients.keys():
    if msgp2p_clients[clientid] != None and msgp2p_clients[clientid].localUID == uid:
      return clientid
  return 0
  
def setUniqueID(clientid, localUID):
  # If this localUID is bound to another clientid unbind it first
  # otherwise there will be multiple instances listening to the same UID.
  for cid in msgp2p_clients.keys():
    if msgp2p_clients[cid] != None and msgp2p_clients[cid].localUID == localUID:
      msgp2p_clients[cid].stoplistening = True
      msgp2p_clients[cid] = None
  msgp2p_clients[clientid] = msgp2p(localUID, process_msgp2p_message)
    