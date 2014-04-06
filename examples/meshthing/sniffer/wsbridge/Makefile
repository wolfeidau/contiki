PROJECT = wsbridge

CC = gcc
CFLAGS = -c
SOURCES = $(PROJECT).c
OBJECTS = $(SOURCES:.c=.o)

all: $(PROJECT)

$(PROJECT): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

%.o:%.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm *.o >/dev/null 2>&1

clobber: clean
	-rm $(PROJECT) >/dev/null 2>&1
