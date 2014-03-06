# MacroIP_UDP is part of MacroIP Core. Provides Access to UDP data through simple
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

import MacroIP_DHCP
import socket
import select
import struct
from threading import Thread

outputMacrosQueue = []
active_sockets = []

def startActivity():
  thread = Thread( target = serveIncomingIPTraffic )
  thread.start()

def processMacro(clientid, macro):

  # Send macro  
  if macro.startswith("udp.send\\"):
    params = macro.split("\\")
    source_address = MacroIP_DHCP.getIP(clientid)
    source_port = int(params[2])
    dest_address = params[1]
    dest_port = int(params[3])
    data_to_send = macro[ macro.find("\\\\") + 2:]
    
    udpsocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    # Do not bind if the node didn't have a lease, we will just go out
    #   from the AP main IP Address (so basically we SNAT) and port mapping
    #   will take care to use a suitable source port, we in fact ignore 
    #   client supplied source port.
    if source_address != None:
      udpsocket.bind((source_address, source_port))
    udpsocket.sendto(data_to_send, (dest_address, dest_port))
    udpsocket.close()

  # Listen
  if macro.startswith("udp.listen\\"):
    print clientid
    params = macro.split("\\")
    local_port = int(params[1])
    local_address = MacroIP_DHCP.getIP(clientid)
    
    # We cannot listen unless the client has leased an IP
    if local_address != None:
      print "Listening " + local_address + ":" + str(local_port)
      udpsocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
      udpsocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
      udpsocket.bind((local_address, local_port))
      active_sockets.append(udpsocket)
    else:
      outputMacrosQueue.append((clientid,  "\\udp.listen\\failed\\\\"))

# This loop is run in a separate thread and keeps waiting for incoming
#   UDP traffic and delivers it to the client.
def serveIncomingIPTraffic():
 global outputMacrosQueue
 while True:
  readable, writable, errored = select.select(active_sockets, [], [], 0)
  for s in readable:
   (local_ip , local_port) = s.getsockname()
   clientid = MacroIP_DHCP.getClientID(local_ip)
   
   if clientid != 0:
     data, addr = s.recvfrom(1024)
     remote_ip = addr[0]
     remote_port = addr[1]
     
     if data:
       outputMacrosQueue.append((clientid,  "\\udp.data\\" + str(local_port) + "\\" + str(remote_ip) + "\\" + str(remote_port) + "\\\\" + data))
         
def getOutputMacroIPMacro():
  if len(outputMacrosQueue) > 0:
    return outputMacrosQueue.pop(0)
  else:
    return (None, None)
    