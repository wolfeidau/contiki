# Build:   make
# Install: make burn

CONTIKI_PROJECT = sniffer
CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

all: $(CONTIKI_PROJECT).hex

burn: $(CONTIKI_PROJECT).u

clobber: clean
	-rm $(CONTIKI_PROJECT).hex symbols.c symbols.h >/dev/null 2>&1

CONTIKI = ../../../..
include $(CONTIKI)/Makefile.include
