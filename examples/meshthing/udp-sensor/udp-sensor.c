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

#include <string.h>

#define SENSOR_PORT 3000
#define REMOTE_PORT 3001
#define MAX_PAYLOAD_LEN 120

#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_UDP_BUF  ((struct uip_udp_hdr *)&uip_buf[UIP_LLH_LEN + UIP_IPH_LEN])

static struct uip_udp_conn *server_conn;

static void
tcpip_handler(void)
{
  static int seq_id;

  /* msgpack::sbuffer is a simple buffer implementation. */
  msgpack_sbuffer sbuf;
  msgpack_sbuffer_init(&sbuf);

  if(uip_newdata()) {
    printf("seq_id %d\n", seq_id ++);
    printf("remote: ");
    uip_debug_ipaddr_print(&UIP_IP_BUF->srcipaddr);
    printf("\n");

    ((char *)uip_appdata)[uip_datalen()] = 0;

    uip_ipaddr_copy(&server_conn->ripaddr, &UIP_IP_BUF->srcipaddr);
//  server_conn->rport = UIP_UDP_BUF->srcport;

    msgpack_packer pk;
    msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);

    msgpack_pack_array(&pk, 2);
    msgpack_pack_raw_body(&pk, "example", 7);
    msgpack_pack_int(&pk, seq_id);

    uip_udp_packet_send(server_conn, sbuf.data, sbuf.size);

    /* Restore server connection to allow data from any node */
    uip_create_unspecified(&server_conn->ripaddr);
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

  server_conn = udp_new(NULL, UIP_HTONS(REMOTE_PORT), NULL);
  udp_bind(server_conn, UIP_HTONS(SENSOR_PORT));

  while(1) {
    PROCESS_YIELD();
    if(ev == tcpip_event) {
      tcpip_handler();
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
