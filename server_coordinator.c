#include <limits.h>

#include "server.h"
#include "server_db.h"
#include "server_coordinator.h" 


static int task_counter;

static int task_split(const struct running_task *rt)
{
	FILE *in_fp;
	size_t sz;

	int chunk_sz;	
	in_fp = fopen(rt->input_file, "r");
	fseek(in_fp, 0L, SEEK_END);
	sz = ftell(in_fp);
	printf("\nTotal file size: %zu", sz);


	return 0;
}

int 
server_dispatch_task_to_client(
        struct running_subtask *p_subtask,
        struct client_info_data *p_client)
{
    struct msg_task_assign_pld *req_pld = NULL;
    int numbytes;
    int pld_sz; 

    pld_sz = sizeof(struct msg_task_assign_pld) + p_subtask->input_data_len;
    req_pld = malloc(pld_sz);
    if (!req_pld) {
        debug_print("Server ran out of memory!");
        assert(0);
    }

    req_pld->len = p_subtask->input_data_len;
    req_pld->subtask_id = p_subtask->subtask_id;
    memcpy(req_pld->data, p_subtask->input_data, p_subtask->input_data_len);
    numbytes = pkt_send(p_client->socket, MSG_TASK_ASSIGN, 
						req_pld, pld_sz);
    if (numbytes == -1) {
        printf("Error in sending MSG_TASK_ASSIGN");
    }	

    //TODO: We need to wait for the asynchronous response for this.

    free(req_pld);
    return 0;
}

int
server_dist_task_to_clients(const int grp_id,
			/* IN */ const struct input_task *input_task)
{
	struct group_info_node *grp;
	struct client_info_list_node *client_list_node;
    struct running_task *p_run_task;
    int grp_clients_count = 0;
    int status;


	printf("%s: group: %d, input_file: %s\n", __FUNCTION__,
			grp_id, input_task->input_file);

	grp = db_get_group_by_grp_id(grp_id);		
	if (!grp) {
		printf("Invalid group!\n");
		return -1;
    }
    //Increment global task counter
    task_counter = (task_counter + 1) % INT_MAX; 
    status = db_task_new(task_counter, grp_id, TASK_SUM, input_task, &p_run_task); 
    if (status != 0 || !p_run_task) {
        printf("Couldn't assign new task!");
    }

    //breaks our task into subtasks, but doesn't deploy them yet
    server_task_split(p_run_task); 

    db_subtasks_print(p_run_task);

    struct running_subtask *p_subtask = p_run_task->subtasks_head;
	for (client_list_node = grp->members;
            client_list_node != NULL; 
            client_list_node = client_list_node->next) {
        grp_clients_count += 1;
        printf("Client-id: %d, ", client_list_node->data->socket);
        server_task_exec(p_subtask, client_list_node->data);
        p_subtask = p_subtask->next;
        p_subtask->client = client_list_node->data;
    }

    //server_task_exec(p_run_task);

	return 0;
}

int server_task_split(struct running_task *p_rt)
{
    
    char *filename = p_rt->input_file;
    char buf[MAXCHUNKSIZE+2];
    int chunk_sz;
    int offset = 0;
    int ret;
    int i = 0;
    do {
        
        ret = get_next_chunk(filename, offset, &chunk_sz, buf);
        printf("\nNext chunk: i %d offset %d \"%s\"",i, offset, buf);
        offset += chunk_sz;
        db_subtask_new(i, chunk_sz, buf, p_rt);
        i++;
    } while (ret == 0);

}

int 
server_task_exec(struct running_subtask *p_subtask, struct client_info_data *p_client_data)
{
    //TODO: log info about this task being dispatched, start a timer, etc
    server_dispatch_task_to_client(p_subtask, p_client_data);
    return 0;
}

