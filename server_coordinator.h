#ifndef _SERVER_COORDINATOR_H
#define _SERVER_COORDINATOR_H

#include "server.h" 

int
server_dist_task_to_clients(const int grp_id,
			/* IN */ const struct input_task *task_info);


int server_task_exec(struct running_subtask *subtask, struct client_info_data *client);

/* split the task into chunks and deploy subtasks */
int server_task_split(struct running_task *p_rt);
#endif
