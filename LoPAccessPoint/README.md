Access Point
================

The access point is based on a Raspberry PI with a LoP-R(A)N radio connected trough a USB-Serial adapter.


## Misc notes

Some random notes to elaboarate into better instructions when the code will be more mature.

* For DHCP operation IP aliases have to be defined. This is done in /etc/network/interfaces with a configuration one the lines of:

    auto eth0
    allow-hotplug eth0

    iface eth0 inet static
        address 192.168.0.200
        netmask 255.255.255.0
    iface eth0 inet static
        address 192.168.0.201
        netmask 255.255.255.0
    etc.
    
This is in addition to the usual iface where gateway etc are defined. It's also possible to have these IPs non static so they
would be leased from whatever DHCP server is running on the net. There could be a syntax also to specify a range. And it would
me more flexibly if these can be added programmatically without restating the network service, but it's not critical.

Ping reply must be disabled otherwise all reserved IPs will reply to the ping without it even going to the nodes. this is
done in  /etc/sysctl.conf adding the following line:


    net.ipv4.icmp_echo_ignore_all = 1 
    
