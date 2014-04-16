# Build:   make
# Install: make burn

CONTIKI_PROJECT = eeprom_layout

all: eeprom_update $(CONTIKI_PROJECT).hex

burn: $(CONTIKI_PROJECT).u

clobber: clean
	-rm eeprom_update
	-rm $(CONTIKI_PROJECT).hex symbols.c symbols.h >/dev/null 2>&1

## MeshThing
UIP_CONF_IPV6=1
UIP_CONF_RPL=1

CONTIKI = ../../..
include $(CONTIKI)/Makefile.include

eeprom_update:
	cc -o eeprom_update eeprom_update.c intel_hex.c
