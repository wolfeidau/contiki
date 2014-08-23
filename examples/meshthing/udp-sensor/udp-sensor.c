/**
 * \file
 *         A very simple Contiki application showing how to read a sensor over UDP.
 * \author
 *         Mark Wolfe <mark@wolfe.id.au>
 */

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"

#define DEBUG DEBUG_PRINT
#include "net/ip/uip-debug.h"

#include <avr/io.h>
#include <stdio.h>
#include "msgpack.h"
#include "dht22.h"

#include <string.h>

#define SENSOR_PORT 3000
#define MAX_PAYLOAD_LEN 120

#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_UDP_BUF  ((struct uip_udp_hdr *)&uip_buf[UIP_LLH_LEN + UIP_IPH_LEN])

static struct uip_udp_conn *server_conn;

static void
tcpip_handler(void)
{
  static int seq_id;

  // msgpack::sbuffer is a simple buffer implementation.
  msgpack_sbuffer sbuf;
  msgpack_sbuffer_init(&sbuf);

  if(uip_newdata()) {

    seq_id++;

    printf("seq_id %d\n", seq_id);
    printf("remote: ");
    uip_debug_ipaddr_print(&UIP_IP_BUF->srcipaddr);
    printf("\n");

    ((char *)uip_appdata)[uip_datalen()] = 0;

    uip_ipaddr_copy(&server_conn->ripaddr, &UIP_IP_BUF->srcipaddr);

    // set the src port to remote soure port.
    server_conn->rport = UIP_UDP_BUF->srcport;

    //printf("%02x", );
    
    // build the data packet
    msgpack_packer pk;
    msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);

    // map containing one element
    msgpack_pack_map(&pk, 2);

    // key value
    msgpack_pack_raw(&pk, 3);
    msgpack_pack_raw_body(&pk, "seq", 3);

    // value as int
    msgpack_pack_int(&pk, seq_id);
    
    // key value
    msgpack_pack_raw(&pk, 4);
    msgpack_pack_raw_body(&pk, "rssi", 4);

    // value as int
    msgpack_pack_int(&pk, sicslowpan_get_last_rssi());
    
    // send the response
    uip_udp_packet_send(server_conn, sbuf.data, sbuf.size);

    // Restore server connection to allow data from any node
    uip_create_unspecified(&server_conn->ripaddr);
    
    // clear the server rport
    server_conn->rport = 0;
  }

  msgpack_sbuffer_destroy(&sbuf);
}

/*---------------------------------------------------------------------------*/
PROCESS(udp_sensor_process, "UDP sensor process");
AUTOSTART_PROCESSES(&udp_sensor_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_sensor_process, ev, data)
{
  PROCESS_BEGIN();

  static struct etimer et_led, et_sensor;
  static dhtsample_t sample;

  DDRB |= _BV(PINB1); // set PINB1 to output

  bool flag = false;

  // allocate a dynamic port for each new request
  server_conn = udp_new(NULL, 0, NULL);

  printf("UDP server started\n");

  // bind to a port for requests.
  udp_bind(server_conn, UIP_HTONS(SENSOR_PORT));

  etimer_set(&et_led, CLOCK_SECOND * 2); // Clo
  //etimer_set(&et_sensor, CLOCK_SECOND / 4);

  static int c_seq;

  while(1) {
    PROCESS_YIELD();
    if(ev == tcpip_event) {
      tcpip_handler();
    } 
    if (ev == PROCESS_EVENT_TIMER && etimer_expired(&et_led)) {      

      PORTB ^= _BV(PINB1); // Toggle on LED

      //if () {
      //dhtAcquire(sample);
      //printf("Read dht22 sensor %f %f\n", sample.hum, sample.temp);
      //printf("read rssi %x\n", sicslowpan_get_last_rssi());
      //}
      DDRD |= _BV(PIND1); // set PIND1 to output
      printf("output DDRD %02x\n", DDRD);
     
      PORTD |= _BV(PIND1); // pull up
      printf("high PORTD %02x\n", PORTD);

      // set the timer
      //etimer_reset(&et_sensor); // waits 250ms

      clock_delay_usec(50000); // 50ms
      clock_delay_usec(50000); // 50ms
      clock_delay_usec(50000); // 50ms
      clock_delay_usec(50000); // 50ms
      clock_delay_usec(50000); // 50ms

    //if (ev == PROCESS_EVENT_TIMER && etimer_expired(&et_sensor)) {

      PORTD &= ~(_BV(PIND1)); // pull low
      printf("low PORTD %02x\n", PORTD);

      clock_delay_usec(20000); // 20ms
      DDRD &= ~(_BV(PIND1)); // change to input
      printf("input DDRD %02x\n", DDRD);

      c_seq = 0;

      printf("read..");

      cli();

      // hit the sensor
      dhtAcquire(sample);
      printf("Read dht22 sensor %f %f\n", sample.hum, sample.temp);
      // for (; c_seq < 8; c_seq++) {
      //   clock_delay_usec(20);
      //   PORTD ^= _BV(PIND1); // Toggle on LED
      // }

      sei();

      etimer_reset(&et_led);

      printf("done\n");
    }

    //}
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
