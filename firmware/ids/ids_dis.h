#ifndef IDS_DIS_H
#define IDS_DIS_H

#include "net/ipv6/uip.h"
#include <stdbool.h>

/* Call once at startup (optional but recommended) */
void ids_dis_init(void);

/* Call when a DIS message is received from 'from'
   (increments counter and decides whether to block) */
void ids_dis_on_rx(const uip_ipaddr_t *from);

/* Check whether 'from' is currently blocked */
bool ids_dis_is_blocked(const uip_ipaddr_t *from);

#endif /* IDS_DIS_H */
