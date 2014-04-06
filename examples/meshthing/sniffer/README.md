802.15.4 mesh-network sniffer
=============================

WireShark serial port bridge

Installation
------------
Note: MeshThing serial console must be running at 115,200 baud

- cd meshthing
- make
- make burn

Usage
-----

- cd wsbridge
- make
- rm /tmp/wireshark
- ./wsbridge /dev/ttyUSB0
- wireshark -k -i /tmp/wireshark
