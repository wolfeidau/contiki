# Build:   make
# Install: make burn

CONTIKI_PROJECT = eeprom_layout

UIP_CONF_IPV6=1
# CFLAGS+= -DWITH_UIP6 -DUIP_CONF_IPV6 -DUIP_CONF_IPV6_RPL

all: eeprom_update $(CONTIKI_PROJECT).hex

burn: $(CONTIKI_PROJECT).u

clobber: clean
	-rm eeprom_update
	-rm $(CONTIKI_PROJECT).hex symbols.c symbols.h >/dev/null 2>&1

CONTIKI = ../../..
include $(CONTIKI)/Makefile.include

eeprom_update:
	cc -o eeprom_update eeprom_update.c intel_hex.c
