#include "ids_dis.h"
#include "sys/clock.h"
#include "sys/ctimer.h"
#include "lib/list.h"
#include "lib/memb.h"
#include "net/ipv6/uip.h"
#include "sys/log.h"
#include <string.h>
#include "net/linkaddr.h"

#define LOG_MODULE "IDS-DIS"
#define LOG_LEVEL LOG_LEVEL_INFO

/* Paper parameters */
#define DIS_THRESHOLD            3
#define WINDOW_RESET_SECONDS     (5 * 60)   /* 5 minutes */
#define TEMP_BLOCK_SECONDS       60         /* 1 minute */
#define BLOCK_THRESHOLD          2          /* 2 strikes -> permanent */

/* Maximum number of neighbors to track (adjust if needed) */
#define MAX_TRACKED_NEIGHBORS    16

/* Structure to store information about each neighbor */
typedef struct dis_entry {
  struct dis_entry *next;          /* Pointer for linked list */
  uip_ipaddr_t ip;                /* IPv6 address of the neighbor */

  uint8_t dis_count;              /* Number of DIS messages received in current window */
  uint8_t block_strikes;          /* Number of times threshold has been exceeded */

  bool permanently_blocked;       /* Indicates if the node is permanently blocked */
  clock_time_t blocked_until;    /* Timestamp until temporary block expires */
} dis_entry_t;

/* Memory block allocator for neighbor entries */
MEMB(entries_memb, dis_entry_t, MAX_TRACKED_NEIGHBORS);
/* Linked list to store active entries */
LIST(entries_list);

/* Timer used to reset the counting window */
static struct ctimer reset_timer;

/* Find an existing entry for a given IP or create a new one */
static dis_entry_t *find_or_create(const uip_ipaddr_t *ip)
{
  dis_entry_t *e;

  /* Search for existing entry */
  for(e = list_head(entries_list); e != NULL; e = e->next) {
    if(uip_ipaddr_cmp(&e->ip, ip)) {
      return e;
    }
  }

  /* Allocate new entry if not found */
  e = memb_alloc(&entries_memb);
  if(e == NULL) {
    /* If table is full, simple policy: do not create new entry */
    return NULL;
  }

  /* Initialize entry */
  memset(e, 0, sizeof(*e));
  uip_ipaddr_copy(&e->ip, ip);
  list_add(entries_list, e);

  return e;
}

/* Periodic function to reset DIS counters */
static void reset_window(void *ptr)
{
  (void)ptr;
  dis_entry_t *e;

  LOG_WARN("[IDS-DIS][WINDOW_RESET] node=%u\n",
           linkaddr_node_addr.u8[LINKADDR_SIZE - 1]);

  /* Reset DIS counters for all tracked neighbors */
  for(e = list_head(entries_list); e != NULL; e = e->next) {
    e->dis_count = 0;
  }

  /* Restart timer */
  ctimer_set(&reset_timer, CLOCK_SECOND * WINDOW_RESET_SECONDS, reset_window, NULL);
}

/* Initialization of the IDS module */
void ids_dis_init(void)
{
  memb_init(&entries_memb);
  list_init(entries_list);

  /* Start periodic window reset timer */
  ctimer_set(&reset_timer, CLOCK_SECOND * WINDOW_RESET_SECONDS, reset_window, NULL);
}

/* Check if a node is currently blocked */
bool ids_dis_is_blocked(const uip_ipaddr_t *from)
{
  dis_entry_t *e = find_or_create(from);
  if(e == NULL) {
    return false;
  }

  /* If permanently blocked, always return true */
  if(e->permanently_blocked) {
    return true;
  }

  /* If temporarily blocked and block time has not expired */
  if(e->blocked_until != 0 && clock_time() < e->blocked_until) {
    return true;
  }

  return false;
}

/* Called when a DIS message is received */
void ids_dis_on_rx(const uip_ipaddr_t *from)
{
  dis_entry_t *e = find_or_create(from);
  if(e == NULL) {
    return;
  }

  /* If permanently blocked, no need to process further */
  if(e->permanently_blocked) {
    return;
  }

  /* If temporarily blocked, do not increase counter (optional behavior) */
  if(e->blocked_until != 0 && clock_time() < e->blocked_until) {
    return;
  }

  /* Increment DIS counter */
  e->dis_count++;

  /* Check if threshold is exceeded */
  if(e->dis_count > DIS_THRESHOLD) {
    e->block_strikes++;
    e->dis_count = 0; /* Reset counter for next local window */

    /* If strike threshold reached, apply permanent block */
    if(e->block_strikes >= BLOCK_THRESHOLD) {
      e->permanently_blocked = true;

      LOG_WARN("[IDS-DIS][BLOCK_PERM] node=%u attacker=",
               linkaddr_node_addr.u8[LINKADDR_SIZE - 1]);
      LOG_WARN_6ADDR(from);
      LOG_WARN_("\n");

    } else {
      /* Otherwise, apply temporary block */
      e->blocked_until = clock_time() + CLOCK_SECOND * TEMP_BLOCK_SECONDS;

      LOG_WARN("[IDS-DIS][BLOCK_TEMP] node=%u attacker=",
               linkaddr_node_addr.u8[LINKADDR_SIZE - 1]);
      LOG_WARN_6ADDR(from);
      LOG_WARN_("\n");
    }
  }
}