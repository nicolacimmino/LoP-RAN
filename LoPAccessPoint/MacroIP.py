# MacroIP is part of MacroIP Core. Provides Access to IP services through simple
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

import MacroIP_DHCP
import MacroIP_UDP
import MacroIP_ICMP
import MacroIP_STUN
import MacroIP_HTTP
import MacroIP_MSGP2P

outputMacrosQueue = []

# Processes one macro
def processMacroIPMacro(clientid, macro):
 
  if macro.startswith("dhcp."):
    MacroIP_DHCP.processMacro(clientid, macro)
      
  if macro.startswith("udp."):
    MacroIP_UDP.processMacro(clientid, macro)
      
  if macro.startswith("stun."):
    MacroIP_STUN.processMacro(clientid, macro)
    
  if macro.startswith("icmp."):
    MacroIP_ICMP.processMacro(clientid, macro)
        
  if macro.startswith("http."):
    MacroIP_HTTP.processMacro(clientid, macro)

  if macro.startswith("msgp2p."):
    MacroIP_MSGP2P.processMacro(clientid, macro)
    
# Fetches a macro to be sent to a client of the 
# host application. 
def getOutputMacroIPMacro():
  (clientid, macro) = MacroIP_DHCP.getOutputMacroIPMacro() 
  if(clientid != None):
    outputMacrosQueue.append((clientid, macro))
  (clientid, macro) = MacroIP_UDP.getOutputMacroIPMacro() 
  if(clientid != None):
    outputMacrosQueue.append((clientid, macro))
  (clientid, macro) = MacroIP_ICMP.getOutputMacroIPMacro() 
  if(clientid != None):
    outputMacrosQueue.append((clientid, macro))
  (clientid, macro) = MacroIP_STUN.getOutputMacroIPMacro() 
  if(clientid != None):
    outputMacrosQueue.append((clientid, macro))
  (clientid, macro) = MacroIP_HTTP.getOutputMacroIPMacro() 
  if(clientid != None):
    outputMacrosQueue.append((clientid, macro))
  (clientid, macro) = MacroIP_MSGP2P.getOutputMacroIPMacro() 
  if(clientid != None):
    outputMacrosQueue.append((clientid, macro))
    
  if len(outputMacrosQueue) > 0:
    return outputMacrosQueue.pop(0)
  else:
    return (None, None)
    
 
def startActivity():
  MacroIP_UDP.startActivity()
  MacroIP_ICMP.startActivity()
 