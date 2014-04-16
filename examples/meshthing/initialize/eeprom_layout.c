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
 *         Used to create the "eeprom.layout" file.
 * \author
 *         Andy Gelme <andyg@geekscape.org>
 */

/*
 * Description
 * ~~~~~~~~~~~
 * Determine EEPROM field offsets and lengths to create "eeprom.layout" file.
 * "eeprom.layout" file is used by "eeprom_update" to alter EEPROM values.
 *
 * Usage
 * ~~~~
 *   # Acquire MeshThing EEPROM field layout
 *   make
 *   make burn
 *   stty -F /dev/ttyUSB0 38400
 *   cat     /dev/ttyUSB0
 *
 *   # Press MeshThing "reset" button
 *   # Copy from "EEMEM_BEGIN" to "EEMEM_END" to file "eeprom.layout"
 *
 * Notes
 * ~~~~~
 *   0x00 00 0x00                 tx_power
 *   0x01 01 0x0000               pan_address
 *   0x03 03 0xCDAB               pan_id (ABCD)
 *   0x05 05 0x10                 channel_id (16)
 *   0x06 06 0xEF                 channel_id xor
 *   0x07 07 0x0000               node_id
 *   0x09 09 6C6F63616C686F737400 domain name "localhost"
 *   0x13 19 6D6573687468696E6700 server name "meshthing"
 *   0x1d 29 020000FFFE000001     mac address
 *
 * To do
 * ~~~~~
 * - Correctly handle variable length strings for "domain" and "server" names.
 *   The length of these fields affects the location of the "mac address".
 */

#define PRINTF(FORMAT,args...) printf_P(PSTR(FORMAT),##args)

#define SIZEOF_MAC_ADDRESS  8
#define SIZEOF_STRING       0

#include "contiki.h"
#include <stdio.h>
#include <avr/pgmspace.h>                                        // Define PSTR

extern uint8_t  eemem_txpower;
extern uint16_t eemem_panaddr;
extern uint16_t eemem_panid;
extern uint8_t  eemem_channel[2];
extern uint16_t eemem_nodeid;
extern uint8_t  eemem_domain_name[];
extern uint8_t  eemem_server_name[];
extern uint8_t  eemem_mac_address[];

PROCESS(eeprom_addresses_process, "EEPROM addresses process");
AUTOSTART_PROCESSES(&eeprom_addresses_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(eeprom_addresses_process, ev, data)
{
  PROCESS_BEGIN();
  PRINTF("EEMEM_BEGIN  field_name offset length\n");

  PRINTF("eemem_txpower      %d %d\n",
    & eemem_txpower, sizeof(eemem_txpower));

  PRINTF("eemem_panaddr      %d %d\n",
    & eemem_panaddr, sizeof(eemem_panaddr));

  PRINTF("eemem_panid        %d %d\n",
    & eemem_panid,   sizeof(eemem_panid));

  PRINTF("eemem_channelid    %d %d\n",
    & eemem_channel, sizeof(eemem_channel));

  PRINTF("eemem_nodeid       %d %d\n",
    & eemem_nodeid,  sizeof(eemem_nodeid));

  PRINTF("eemem_domain_name  %d %d\n",
      eemem_domain_name, SIZEOF_STRING);

  PRINTF("eemem_server_name  %d %d\n",
      eemem_server_name, SIZEOF_STRING);

  PRINTF("eemem_mac_address  %d %d\n",
      eemem_mac_address, SIZEOF_MAC_ADDRESS);

  PRINTF("EEMEM_END\n");

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
