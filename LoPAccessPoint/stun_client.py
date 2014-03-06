'''
Client for STUN (Session Traversal Utilities for NAT)
Author: Zhang NS (http://myweb.polyu.edu.hk/~12132351d/) <deltazhangns@163.com>
Latest revision: 30 Dec 2013
Written in Python 2.x
Based on UDP Protocol using Python Socket
Specification RFC5389 (http://tools.ietf.org/html/rfc5389) followed
Designed to obtain the corresponding reflexive transport address of a local transport address
Can be utilized in NAT Behavior Discovery and NAT Traversal, which are essential in advanced deployments of VoIP, SIP, and P2P
 
Warning:
This implementation is unreliable and unsafe.
Exception handling, retransmission mechanism, and response validation are imperfect.
Use at your own risk!
'''

# Modified by Nicola Cimmino 2014 to act as a function for the proof of concept MacroIP
# Final MacroIP will most likely not use STUN as a discovery service. If it will this
# module will be replaced by something else or a cleanup will be done on this code.
# STUN server is hardcoded for now, this is not the final solution just for the proof
# of concept won't generate extra load on the server.
 
import socket, platform, sys, random

BUFF = 4096
ID = ''
lenID = 96 / 8
timeout = 5
cookiemagic = '\x21\x12\xa4\x42'
 
def GenID(): # Generate 96 bits random string
    ret = ''
    for i in range(0, lenID):
        ret += chr(random.randint(0, 0xFF))
    return ret
 
def GenReq(): # Generate STUN Binding Request
    global ID
    ret = ''
    # STUN Message Type: 0x0001 Binding Request
    ret += '\x00\x01'
    # Message Length is 0
    ret += '\x00\x00'
    # Magic Cookie
    ret += cookiemagic
    # Transaction ID
    ID = GenID()
    ret += ID
    return ret
 
def GetTAddressByResponse(r): # Return tuple (IP, Port) or False
    try:
        # Is the Response a Success Response with Binding Method?
        # Check the first two bytes. Are they 0x0101, as promised?
        if r[0:2] != '\x01\x01': return False
        # Are the last 2 bits of Message Length zero?
        if ord(r[3]) % 4 != 0: return False
        # Does the Response contain the correct Magic Cookie?
        if r[4:8] != cookiemagic: return False
        # Does the Response contain the correct Transaction ID?
        if r[8:20] != ID: return False
        iter = 20 #iterator for traversal of r
        # Traverse the response to find the first MAPPED-ADDRESS attribute.
        while r[iter:iter + 2] != '\x00\x01':
            iter = iter + 4 + 0x100 * ord(r[iter + 2]) + ord(r[iter + 3])
        port = 0x100 * ord(r[iter + 6]) + ord(r[iter + 7])
        IP = '%s.%s.%s.%s' % (ord(r[iter + 8]), ord(r[iter + 9]), ord(r[iter + 10]), ord(r[iter + 11])) #Dot-decimal notation
        return (IP, port)
    except Exception as e:
        print('Error when interpreting response: %s' % e)
        return False
 
def perfomSTUNDiscovery(local_address, local_port):
 
   
  HOST = "stun.sipgate.net"
  PORT = 3478
  ADDR = HOST, PORT

  try:
      c = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
      c.settimeout(timeout)
      c.bind((local_address, local_port))
  except Exception as e:
      print('Error when creating socket: %s' % e)
      return None
  try:
      c.sendto(GenReq(), ADDR)
      print('STUN request sent...')
  except Exception as e:
      print('Error when sending request: %s' % e)
      return None
  try:
      resp = c.recv(BUFF)
      taddr = GetTAddressByResponse(resp)
      if taddr:
          print('STUN server returns %s:%s' % taddr)
          return taddr
      else:
          print('STUN server returns non-Success Response!')
  except Exception as e:
      print('Error when receiving response: %s' % e)
  
  return None