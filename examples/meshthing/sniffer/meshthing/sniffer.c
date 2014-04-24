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
 *         Sniffer process, which puts radio into promiscuous mode.
 *         Sniffer RDC driver, which dumps received packets to serial console.
 *         Serial output format intended for modified version of "wsbridge".
 *         Serial baud rate must be 115,200.
 *
 * \author
 *         Andy Gelme - <andyg@geekscape.org>
 */

#include "contiki.h"
#include "hal.h"
#include "net/packetbuf.h"
#include "net/mac/mac.h"
#include "net/mac/rdc.h"

#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
/*---------------------------------------------------------------------------*/
PROCESS(sniffer_process, "Sniffer process");
AUTOSTART_PROCESSES(&sniffer_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sniffer_process, ev, data)
{
  PROCESS_BEGIN();

  PRINTF("Sniffer started\n");

  hal_register_write(XAH_CTRL_1,  hal_register_read(XAH_CTRL_1)  | 0x02);
  hal_register_write(CSMA_SEED_1, hal_register_read(CSMA_SEED_1) | 0x10);

  PROCESS_EXIT();

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
static void
send(mac_callback_t sent, void *ptr)
{
  if(sent) {
    sent(ptr, MAC_TX_OK, 1);
  }
}
/*---------------------------------------------------------------------------*/
static void
send_list(mac_callback_t sent, void *ptr, struct rdc_buf_list *list)
{
  if(sent) {
    sent(ptr, MAC_TX_OK, 1);
  }
}
/*---------------------------------------------------------------------------*/
static void
input(void)
{
  uint16_t  datalen;
  uint8_t  *dataptr;
  uint8_t   i;

  datalen = packetbuf_datalen();
  dataptr = packetbuf_dataptr();

#if 1
  PRINTF("%c", datalen);
  for (i=0;i<datalen;i++) PRINTF("%c", dataptr[i]);
#else
  PRINTF("Length: %d\n", datalen);
  for (i=0;i<datalen;i++) PRINTF("%02x ", dataptr[i]);
  PRINTF("\n");
#endif
}
/*---------------------------------------------------------------------------*/
static int
on(void)
{
  return 1;
}
/*---------------------------------------------------------------------------*/
static int
off(int keep_radio_on)
{
  return keep_radio_on;
}
/*---------------------------------------------------------------------------*/
static unsigned short
cca(void)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
static void
init(void)
{
}
/*---------------------------------------------------------------------------*/
const struct rdc_driver sniffer_rdc_driver = {
  "sniffer-rdc",
  init,
  send,
  send_list,
  input,
  on,
  off,
  cca,
};
/*---------------------------------------------------------------------------*/
