/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
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
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         A very simple Contiki application showing how etimers and leds work.
 * \author
 *         Mark Wolfe <mark@wolfe.id.au>
 */

#include "contiki.h"
#include <avr/io.h>
#include <stdio.h>

//#define PROGLED_PORT  PORTE
//#define PROGLED_DDR   DDRE
//#define PROGLED_PIN   PINE2

#define PROGLED_PORT  PORTB
#define PROGLED_DDR   DDRB
#define PROGLED_PIN   PINB1

// PORTB
// PIN1
/*---------------------------------------------------------------------------*/
PROCESS(blink_process, "Hello world process");
AUTOSTART_PROCESSES(&blink_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(blink_process, ev, data)
{
  PROCESS_BEGIN();

  printf("Blink LED\n");
  
  // set up a etimer
  static struct etimer et;

  PROGLED_DDR |= _BV(PROGLED_PIN);

  while(1) {
  	
  	/* Delay seconds */
  	etimer_set(&et, CLOCK_SECOND / 2);
    
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

  	PROGLED_PORT ^= (1<<PROGLED_PIN); // Toggle on LED
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
