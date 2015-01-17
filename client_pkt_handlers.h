#ifndef _CLIENT_PKT_HANDLERS_H
#define _CLIENT_PKT_HANDLERS_H

#include <stdlib.h>
#include "common_hdr.h"

/* Server packet handlers */
void get_avail_handler(
			const void *payload, const size_t pyld_size, 
			const pkt_type type);


void *client_recv_thread_fn(void *t_args);
/*chaitanya : client thread toperiodically send the heartbeat */
void *client_heartbeat_thread_fn(void *t_args);

#endif

