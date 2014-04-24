Initialize or change MeshThing EEPROM (AVR ATMega2564RFR2)
==========================================================

Configurable parameters, such as radio channel or MAC address,
are stored in EEPROM.  This project allows those values to be
easily updated, without having to re-burn the Contiki firmware.

Installation
------------
Create "eeprom.layout" file (once off operation).
In most cases, the pre-existing "eeprom.layout" file should be fine.

- make
- make burn
- stty -F /dev/ttyUSB0 38400
- cat     /dev/ttyUSB0
- Press MeshThing "reset" button
- Copy from "EEMEM_BEGIN" to "EEMEM_END" to the file "eeprom.layout"

Usage
-----
Save an initial set of configuration values to EEPROM, using "eeprom.default".
Radio channel will be 26.
MAC address will be 02:00:00:ff:fe:00:00:01

First, check that the correct serial port is being used in run.sh (see $PORT).

You may need to press the MeshThing reset button at the appropriate time
for "avrdude".

- ./run.sh -i

Update the existing EEPROM configuration values ...

- ./run.sh

Operation
---------
"run.sh" uses "avrdude" to read and write the MCU EEPROM.
The program "eeprom_update.c" is used to read, verify, change and write the
EEPROM data file (Intel HEX format).
The "eeprom.layout" file is used by "eeprom_update.c" to locate the EEPROM
fields within the EEPROM data file.

Credit
------
- Paul Stoffregen (http://pjrc.com) for the Intel HEX file format utility.
