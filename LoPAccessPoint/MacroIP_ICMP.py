# MacroIP_ICMP is part of MacroIP Core. Provides Access to ICMP traffic through simple
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

import time
import socket
import select
import struct
import icmp_packet
import MacroIP_DHCP
from threading import Thread

outputMacrosQueue = []
pending_ping_responses = {}

icmp_raw_socket = socket.socket(socket.AF_INET,socket.SOCK_RAW,socket.IPPROTO_ICMP)


def serveIncomingICMPRequests():
  while True:
    data, addr = icmp_raw_socket.recvfrom(1600)
    
    icmp_header = data[20:28]
    icmp_type, icmp_code, checksum, rec_id, sequence = struct.unpack('bbHHH', icmp_header)
 
    if icmp_type==8 and icmp_code==0:
    
      source_address = addr[0]
      source_address_s = str(addr[0]) 
      dest_address_s = '%i.%i.%i.%i' % (ord(data[16]), ord(data[17]), ord(data[18]), ord(data[19]))   
      
      clientid = MacroIP_DHCP.getClientID(dest_address_s)
   
      if clientid != 0:
        response_packet = icmp_packet.create_response_packet(rec_id, sequence, data[28:])
        pending_ping_responses[clientid] = (response_packet, source_address)
        outputMacrosQueue.append((clientid,  "\\icmp.ping_request\\\\"))

def processMacro(clientid, macro):

  # Send macro  
  if macro.startswith("icmp.ping_response\\"):
    try: 
      icmp_raw_socket.bind((MacroIP_DHCP.getIP(clientid), 0))
      icmp_raw_socket.sendto(pending_ping_responses[clientid][0], (pending_ping_responses[clientid][1],1))
    except:
      print "Error while processing icmp_message, igoring"

def startActivity():
  thread = Thread( target = serveIncomingICMPRequests )
  thread.start()
      
def getOutputMacroIPMacro():
  if len(outputMacrosQueue) > 0:
    return outputMacrosQueue.pop(0)
  else:
    return (None, None)
