LoP-RAN
=======

A Low Power - Radio Access Network designed to be deployed on ISM bands.



Introduction
=======

LoP-RAN is a low-power low-cost radio access network that allows devices with extremely low resources to communicate. The network supports a mesh topology in which all traffic tends towards a focal point, the access point which can act as a gateway to the Internet.


Core functions
=======

LoP-RAN is concerned only with two very basic core functions namely the delivery of a message from any node to the access point and the delivery of a message from the access point to any node. The following ancillary functions are also part of LoP-RAN:

- Network discovery
- Redio resources allocation
- Ranging
- Peers hierarchy discovery

**Network discovery**

LoP-RAN provides a broadcast channel on which each node of the network advertises itself so that peers can be discovered without prior knoweledge.

**Radio resources allocation**

Radio resources such as radio channels, time slots in each channel as well as radio addresses need to be nogotiated among peers so that they can communicate efficently and with low probability of interference.

**Ranging**

Ranging is the act of finding the lowest needed trasmit power to reach another peer. This has the benefit of reducing the power consumption as well as the interferece.

**Peers hierarchy discovery**

In a mesh network that aims for the delivery of messages to specific nodes it's necessary to know how peers relate to each other in order to route a message efficently. This prevents the need to simply flood the network which is a less power efficient strategy.
