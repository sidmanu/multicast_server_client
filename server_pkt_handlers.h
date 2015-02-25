#ifndef _SERVER_PKT_HANDLERS_H
#define _SERVER_PKT_HANDLERS_H


#include "server_db.h"

/* Server packet handlers */
void join_handler(struct client_info_data *client_info,
			const void *payload, const size_t pyld_size, 
			const pkt_type type);

void hello_handler(struct client_info_data *client_info,
			const void *payload, const size_t pyld_size, 
			const pkt_type type);

/*chaitanya : */
void heartbeat_handler(struct client_info_data *client_info, 
            const char *payload);

void quit_handler(struct client_info_data *client_info);



#endif

