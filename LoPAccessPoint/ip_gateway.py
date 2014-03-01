# IP Gateway is part of LoPAccessPoint. Provides IP services for LoP Nodes.
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
#	to access point mode already (ATAP1). In due time autodiscovery and
#	configuration will be built.

import serial
import time
import socket
import select
from threading import Thread

ser = serial.Serial("/dev/ttyUSB0", 115200, timeout=0.2)
ser.close()
ser.open()

# We wait few seconds as the radio needs to finalize booting 
#  (serial port open resets it, at least in protos). 
time.sleep(5)
active_sockets = []
	
def serveIncomingIPTraffic():
 while True:
  readable, writable, errored = select.select(active_sockets, [], [], 0)
  for s in readable:
   local_port = s.getsockname()[1]
   print "readable"
   data = s.recv(1024)
   if data:
     ser.write("ATTX " + str((local_port-4000)) + " \\ud\\\\" + data + "\n")
     waitSerialReply()
   else:
     s.close()
     active_sockets.remove(s)

if __name__ == "__main__":
  thread = Thread( target = serveIncomingIPTraffic  )
  thread.start()

def waitSerialReply():
 raw_packet_data = ser.read(12560)
 if raw_packet_data != "":
  #time.sleep(1)
  while True: 
   pending = ser.inWaiting()
   if pending:
    raw_packet_data += ser.read(pending)
   else:
    break
 raw_packet_data=""
 
raw_packet_data=""
while(1):
 try:
  raw_packet_data += ser.read(12560)
  if raw_packet_data != "":
   time.sleep(1)
   while True: 
    pending = ser.inWaiting()
    if pending:
     raw_packet_data += ser.read(pending)
    else:
     break
	
   print raw_packet_data
  
   # Current format is ATRX LoPAddress \directive\params\otherparams\\message
   # eg: ATRX 1 \su\192.168.0.250\4000\\This is a test message.
   #
   # The fist token after the LoPAddress is the IPGateway directive. The following are supported:
   #   \su Send UDP packet
   #   \bu Bind UDP socket and listen
   #
  
   if (raw_packet_data.find("ATRX") >= 0) and (raw_packet_data.find("\n") > 0):
    # The LoP Address from which the packet came 
    source_addr = int(raw_packet_data[5:(raw_packet_data[5:].find(" ")+5)])
    
    # The directive for the IPGateway is everything from the first \ to the first \\
    ip_gateway_directive = raw_packet_data[ raw_packet_data.find("\\") + 1 : raw_packet_data.find("\\\\")].split("\\")
   
    # \su directive
    # Send data as UDP packet to specified address and port
    if ip_gateway_directive[0] == "su":
     address = ip_gateway_directive[1]
     port = int(ip_gateway_directive[2])
     data_to_send = raw_packet_data[ raw_packet_data.find("\\\\") + 2:]
     udpsocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
     udpsocket.sendto(data_to_send, (address, port))

    # \bu directive
    # Bind to a UDP socket and forward incomig traffic to the node.
    if ip_gateway_directive[0] == "bu":
     # For now we allocate the port as 4000+LoPAddress, so we get a unique port
     #  per node with no need to manage any list of ports.
     udpsocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
     udpsocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
     assigned_port = 4000 + source_addr
     udpsocket.bind(('', assigned_port))
     active_sockets.append(udpsocket)
     print "bound:" + str(assigned_port)
     # Reply to the sender informing the port number.
     ser.write("ATTX " + str(source_addr) + " \\up\\" + str(assigned_port) + "\\\\\n")
     waitSerialReply()
	 
    raw_packet_data = ""

 except:
  print "Error serving incoming request. Ignoring"
  raw_packet_data=""
  
