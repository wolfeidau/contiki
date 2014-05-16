#!/bin/bash
#
# Usage
# ~~~~~
#   ./run.sh [-i]
#
#   Where ...
#   -i: Use "eeprom.default" to initialize MeshThing EEPROM values
#
# Notes
# ~~~~~
# Push MeshThing reset button ...
# - Immediately before running this script
# - Immediately before pressing [Enter] during eeprom_update MAC address entry

PORT=/dev/ttyUSB0

BAUD=115200
FILE=eeprom
FILE_INPUT=$FILE.input
FILE_OUTPUT=$FILE.output
MCU=atmega2564rfr2

if [ "$1" == "-i" ]; then
  FILE_INPUT=$FILE.default
else
  avrdude -D -v -b $BAUD -P $PORT -c stk500v2 -p $MCU -U eeprom:r:$FILE_INPUT:i
fi

./eeprom_update eeprom.layout $FILE_INPUT $FILE_OUTPUT

avrdude -D -v -b $BAUD -P $PORT -c stk500v2 -p $MCU -U eeprom:w:$FILE_OUTPUT:i
