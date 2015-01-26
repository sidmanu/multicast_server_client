#include <limits.h>

#include "server.h"
#include "server_db.h"
#include "server_coordinator.h" 

static int task_counter;


static int get_file_chunks(const char *mother_file,
		int num_chunks)
{
	FILE *in_fp;
	size_t sz;

	int chunk_sz;	
	in_fp = fopen(mother_file, "r");
	fseek(in_fp, 0L, SEEK_END);
	sz = ftell(in_fp);
	printf("\nTotal file size: %zu", sz);
	chunk_sz = sz/num_chunks;
	printf("\nChunk size: %d", chunk_sz);
	fseek(in_fp, sz, SEEK_SET);

	return 0;
}

int 
server_dispatch_task_to_client(struct client_info_data *cl_node,
        int subtask_id,
        int subtask_input_len, char *subtask_data)
{

    struct msg_task_assign_pld *req_pld = NULL;
    int numbytes;

    int pld_sz; 
    pld_sz = sizeof(struct msg_task_assign_pld) + subtask_input_len;
    req_pld = malloc(pld_sz);
    if (!req_pld) {
        debug_print("Server ran out of memory!");
        assert(0);
    }

    req_pld->len = subtask_input_len;
    req_pld->subtask_id = subtask_id;
    memcpy(req_pld->data, subtask_data, subtask_input_len);
    numbytes = pkt_send(cl_node->socket, MSG_TASK_ASSIGN, 
						req_pld, pld_sz);
    if (numbytes == -1) {
        printf("Error in sending MSG_TASK_ASSIGN");
    }	

    //TODO: We need to wait for the asynchronous response for this.

    free(req_pld);
}

int
server_dist_task_to_clients(const int grp_id,
			/* IN */ const struct input_task *input_task)
{
	struct group_info_node *grp;
	struct client_info_list_node *client_list_node;
    int status;


	printf("%s: group: %d, input_file: %s\n", __FUNCTION__,
			grp_id, input_task->input_file);

	grp = db_get_group_by_grp_id(grp_id);		
	if (grp) {
		for (client_list_node = grp->members;
			client_list_node != NULL; 
			client_list_node = client_list_node->next) {
		
			printf("Client-id: %d, ", client_list_node->data->socket);
		}
	} else {
		printf("Invalid group!\n");
		return -1;
	}

    //Increment global task counter
    task_counter = (task_counter + 1) % INT_MAX; 
    status = db_task_new(task_counter, grp_id, TASK_SUM, input_task->input_file); 
    if (status != 0) {
        printf("Couldn't assign new task!");
    }

	//get_file_chunks(input_task->input_file, 2);	
	return 0;
}


