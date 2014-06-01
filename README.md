
Introduction
=======

LoP-RAN is a low-power low-cost radio access network that allows devices with extremely low resources to communicate. The network self organizes into a tree to allow all nodes to communicate with the access point. Whenever nodes join or leave the network the others self organize in order to keep the communication up using the minimum amount of resources.

![Self Organizing](https://raw.github.com/nicolacimmino/LoP-RAN/develop/Documentation/Diagrams/SelfOrganizing.gif)


Core functions
=======

LoP-RAN is concerned only with two very basic core functions namely the delivery of a message from any node to the access point and the delivery of a message from the access point to any node. The following ancillary functions are also part of LoP-RAN and are needed to support the core functions:

- Nodes discovery
- Radio resources management
- Ranging
- Network hierarchy discovery
- Addressing and routing

**Nodes discovery**

LoP-RAN provides for the means needed by a node to discover other nodes that are within radio reach. No prior knoweledge of the network is needed, so a node doesn't need to be cofigured before having it to enter the network.

**Radio resources management**

Radio resources such as radio channels, time slots in each channel as well as radio pipes addresses need to be nogotiated among nodes so that they can communicate efficently and with low probability of interference. This functionality is completely automatic and the nodes self organize so that prior network planning is not necessary.

**Ranging**

Ranging is the act of finding the lowest needed trasmit power to reach another node. This has the benefit of reducing the power consumption as well as the interferece.

**Network hierarchy discovery**

Given the goal of the network to transfer messages from an access point to any node and from any node to an access point the nodes self organize themselves in a hierarchy that can be either a star network or a tree. Also in this case the organization is completely autonomous and doesn't require prior knoweledge.

**Addressing and routing**

Each node gets an address that is unique inside the network. This address is used to identify message originating from and destined to the node. The address is a self routing address, that is, an address that contains all the needed information to route the message to the recipient. In this way the nodes involved in the routing don't need to store large routing tables and their load is reduced.


The picture below shows a LoP-RAN node (in the circle top right), an Access Point and a shell that is pinging the node on a local network.

![Node and AP](https://raw.github.com/nicolacimmino/LoP-RAN/develop/Documentation/node_ap_ping.jpg)


