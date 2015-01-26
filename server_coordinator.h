#ifndef _SERVER_COORDINATOR_H
#define _SERVER_COORDINATOR_H

#include "server.h" 

int
server_dist_task_to_clients(const int grp_id,
			/* IN */ const struct input_task *task_info);


#endif
