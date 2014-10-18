CONTIKI_PROJECT = udp-sensor
LDFLAGS += -Wl,-u,vfprintf -lprintf_flt -lm

UIP_CONF_IPV6=1
APPS += messagepack
APPS += dht22

all: $(CONTIKI_PROJECT).hex

burn: $(CONTIKI_PROJECT).u

clobber: clean
	-rm $(CONTIKI_PROJECT).hex symbols.c symbols.h >/dev/null 2>&1

CONTIKI = ../../..
include $(CONTIKI)/Makefile.include