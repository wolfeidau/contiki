/*
 * Copyright (c) 2014, Geekscape Pty. Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the organization nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE ORGANIZATION AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE ORGANIZATION OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the MeshThing toolkit / examples.
 */

/**
 * \file
 *         Used to create an updated "eeprom.output" file.
 * \author
 *         Andy Gelme <andyg@geekscape.org>
 */

/*
 * Description
 * ~~~~~~~~~~~
 * Reads an EEPROM input file dumped by "avrdude" in Intel HEX format.
 * Uses "eeprom.layout" file (created by eeprom_layout.c)
 * to define EEPROM field names, offsets and lengths.
 * Allows EEPROM field values to be changed interactively.
 * Writes an EEPROM output file that "avrdude" can write to the MCU EEPROM.
 *
 * Usage
 * ~~~~~
 *   eeprom_update eeprom.layout eeprom.input eeprom.output
 *
 *   Where ...
 *     eeprom.layout: Defines EEPROM field names, offsets and sizes
 *     eeprom.input:  Current EEPROM values. Read by avrdude
 *     eeprom.output: Updated EEPROM values. To be written by avrdude
 *
 *   Example ...
 *     eeprom_update eeprom.layout eeprom.default /dev/stdout
 *
 * To Do
 * ~~~~~
 * - Change field values via command line arguments.
 * - Command line parameter: EEPROM memory range to save.
 * - Add EEPROM layout "field data type".
 * - Handle custom application EEPROM fields (improve "eeprom.layout").
 * - Use STK500 protocol directly to read/write EEPROM (instead of avrdude).
 *   - http://www.atmel.com/Images/doc2591.pdf
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FALSE  0
#define TRUE   1

// intel_hex.c definitions

#define SAVE_COMMAND_FORMAT "S 0 3f %s"      // memory begin/end in hexadecimal

void load_file(char *filename);              // Load Intel HEX file
int  parse_hex_line(char *theline, int bytes[], int *addr, int *num, int *code);
void save_file(char *command);               // Save Intel HEX file

// local definitions

int memory[64 * 1024];                                   // used by load_file()
#define MEMORY_SIZE  (sizeof(memory) / sizeof(int))

typedef struct {
  char name[32];
  int  offset;
  int  length;
}
  eepromField_t;

#define EEPROM_FIELDS_MAXIMUM  16
#define EEPROM_CHANNEL_ID      "eemem_channelid"

int           eepromFieldCount = 0;
eepromField_t eepromFields[EEPROM_FIELDS_MAXIMUM];

void changeEepromField(char *name, int offset, int length);
void changeEepromFieldChannelId(char *name, int offset, int length);
void changeEepromFieldMacAddress(char *name, int offset, int length);
void changeEepromFields(void);
void displayUsage(void);
void dumpEepromField(char *name, int offset, int length);
void dumpEepromFields(void);
void dumpEepromLayout(void);
int  getChannelId(int offset);
void initialize(void);
void loadEepromLayout(char *layoutFileName);
void validateChannelId(void);

int main(
  int   argc,
  char *argv[]) {

  if (argc != 4) displayUsage();

  char *eepromLayoutFilename = argv[1];
  char *eepromInputFilename  = argv[2];
  char *eepromOutputFilename = argv[3];

  initialize();

  load_file(eepromInputFilename);
  printf("\n");

  loadEepromLayout(eepromLayoutFilename);

//dumpEepromLayout();

  validateChannelId();

  dumpEepromFields();

  changeEepromFields();

  char saveCommand[256];
  sprintf(saveCommand, SAVE_COMMAND_FORMAT, eepromOutputFilename);
  save_file(saveCommand);

  exit(0);
}

void changeEepromField(
  char *name,
  int offset,
  int length) {

  if (strcmp(name, EEPROM_CHANNEL_ID) == 0) {
    changeEepromFieldChannelId(name, offset, length);
  }

  if (strcmp(name, "eemem_mac_address") == 0) {
    changeEepromFieldMacAddress(name, offset, length);
  }
}

void changeEepromFieldChannelId(
  char *name,
  int offset,
  int length) {

  char buffer[80];
  int  channelId = -1;

  while (channelId == -1) {
    printf("Change %s: [%d] (11 - 26) ? ", name, getChannelId(offset)); 

    if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
      channelId = atoi(buffer);

      if (channelId == 0) {
        channelId = getChannelId(offset);
      }
      else {
        if (channelId >= 11  &&  channelId <= 26) {
          memory[offset]     = channelId;
          memory[offset + 1] = (~channelId & 0xff);
        }
        else {
          fprintf(stderr, "EEPROM channel id out of range 11 to 26\n");
          channelId = -1;
        }
      }
    }
  }
}

void changeEepromFieldMacAddress(
  char *name,
  int offset,
  int length) {

  char buffer[80];
  int  macAddress = -1;

  printf(
    "Change %s: [%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x] ? ", name, 
    memory[offset],   memory[offset+1], memory[offset+2], memory[offset+3],
    memory[offset+4], memory[offset+5], memory[offset+6], memory[offset+7]
  );

  if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
    sscanf(buffer, "%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
      & memory[offset],   & memory[offset+1],
      & memory[offset+2], & memory[offset+3],
      & memory[offset+4], & memory[offset+5],
      & memory[offset+6], & memory[offset+7]
   );
  }
}

void changeEepromFields(void) {
  int index;

  for (index = 0;  index < eepromFieldCount;  index ++) {
    changeEepromField(
      eepromFields[index].name,
      eepromFields[index].offset,
      eepromFields[index].length
    );
  }

  printf("\n");
}

void displayUsage(void) {
  printf("Usage:\n");
  printf("  eeprom_update eeprom.layout eeprom.input eeprom.output\n");
  printf("\n");
  printf("Where ...\n");
  printf("  eeprom.layout: Defines EEPROM field names, offsets and sizes\n");
  printf("  eeprom.input:  Current EEPROM values. Read by avrdude\n");
  printf("  eeprom.output: Updated EEPROM values. To be written by avrdude\n");
  exit(-1);
}

void dumpEepromField(
  char *name,
  int offset,
  int length) {

  char *format = "%02x";

// Only display the channel id (not the XORed value) in decimal

  if (strcmp(name, EEPROM_CHANNEL_ID) == 0) {                     // channel id
    format = "%d";
    length = 1;
  }

  if (length == 0) {                                              // string
    char buffer[32];
    int  i;

    for (i = 0; i < (sizeof(buffer) - 1) && memory[offset + i] != 0; i ++) {
      buffer[i] = memory[offset + i];
    }
    buffer[i] = 0;

    printf("Field: %s, value: %s\n", name, buffer);
  }
  else {
    unsigned int value = memory[offset];                          // byte

    if (length == 2) {                                            // word
      value += (memory[offset + 1] << 8);
    }

    printf("Field: %s, value: ", name);

    if (length <= 2) {
      printf(format, value);                                      // scalar
    }
    else {                                                        // array
      int i;
      for (i = 0;  i < length;  i++) printf(format, memory[offset + i]);
    }

    printf("\n");
  }
}

void dumpEepromFields(void) {
  int index;

  for (index = 0;  index < eepromFieldCount;  index ++) {
    dumpEepromField(
      eepromFields[index].name,
      eepromFields[index].offset,
      eepromFields[index].length
    );
  }

  printf("\n");
}

void dumpEepromLayout(void) {
  int index;

  for (index = 0;  index < eepromFieldCount;  index ++) {
    printf("Field: %s, offset: %d, length: %d\n",
      eepromFields[index].name,
      eepromFields[index].offset,
      eepromFields[index].length
    );
  }

  printf("\n");
}

int getChannelId(
  int offset) {

  return(memory[offset]);
}

void initialize(void) {
  int address;
  for (address = 0; address < MEMORY_SIZE; address ++) memory[address] = 0xff;
}

void loadEepromLayout(
  char *filename) {

  char buffer[80];
  int  parseEnabled = FALSE;

  FILE *eepromLayoutFile = fopen(filename, "r");

  if (eepromLayoutFile == NULL) {
    fprintf(stderr, "Couldn't read EEPROM layout file: %s\n", filename);
    exit(-1);
  }

  while (fgets(buffer, sizeof(buffer), eepromLayoutFile) != NULL) {
    char name[32];
    int  offset, length;

    int items = sscanf(buffer, "%s %d %d", name, & offset, & length);

    if (items == 1) {
      if (memcmp(buffer, "EEMEM_BEGIN", 11) == 0) parseEnabled = TRUE;
      if (memcmp(buffer, "EEMEM_END",    9) == 0) parseEnabled = FALSE;
    }

    if (items == 3  &&  parseEnabled) {
      if (eepromFieldCount >= EEPROM_FIELDS_MAXIMUM) {
        fprintf(stderr, "Too many EEPROM fields in layout: %s\n", filename);
        exit(-1);
      }

      strcpy(eepromFields[eepromFieldCount].name, name);
      eepromFields[eepromFieldCount].offset = offset;
      eepromFields[eepromFieldCount].length = length;
      eepromFieldCount ++;
    }
  }

  fclose(eepromLayoutFile);

  return;
}

void validateChannelId(void) {
  int found = FALSE;
  int index;

  for (index = 0;  index < eepromFieldCount;  index ++) {
    if (strcmp(eepromFields[index].name, EEPROM_CHANNEL_ID) == 0) {
      found = TRUE;

      int offset = eepromFields[index].offset;
      int length = eepromFields[index].length;

      int channelId      = getChannelId(offset);
      int channelIdCheck = memory[offset + 1];

      if (channelId < 11  ||  channelId > 26) {
        fprintf(stderr, "EEPROM channel id out of range: %d\n", channelId);
        exit(-1);
      }

      if (channelId != (~channelIdCheck & 0xff)) {
        fprintf(stderr, "EEPROM channel id check failed\n");
        exit(-1);
      }

      break;
    }
  }

  if (found == FALSE) {
    fprintf(stderr, "EEPROM layout doesn't define '%s'\n", EEPROM_CHANNEL_ID);
    exit(-1);
  }
}
