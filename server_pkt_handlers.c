#include "server.h"
#include "server_db.h"
#include "server_pkt_handlers.h"


#include <pthread.h>
#include <netinet/in.h>

void join_handler(struct client_info_data *client_info,
		const void *payload, const size_t pyld_size, const pkt_type type)
{
	struct group_info_node *grp_info;
	const struct msg_join_grp_pld *join_pld = payload;
	int ret;

	int grp_id = ntohs(join_pld->grp_id);
	debug_print("Client (sock_fd:%d) requests to join group:\"%d\"\n", client_info->socket,
			grp_id);

	grp_info = db_get_group_by_grp_id(grp_id);
	if (!grp_info) {
		debug_print("Creating group %d...\n", grp_id);
		db_group_new(grp_id); 
	}

	ret = db_group_add_member(grp_id, client_info);

	if (ret != 0) {
		perror("Error in adding member to group!");
	}
		
}
void hello_handler(struct client_info_data *client_info,
		const void *payload, const size_t pyld_size, const pkt_type type)
{
	strncat(client_info->buffer, payload, pyld_size);
	strcat(client_info->buffer, ", ");
}


void task_result_handler(struct client_info_data *client_info,
		const void *payload, const size_t pyld_size, const pkt_type type)
{
    
    struct msg_task_result_pld *resp_pld;
    resp_pld = (void*) payload;

    printf("Received response for task_id: %d, subtask_id: %d"
            " from client %d, status: %d, sum: %ld\n", 
            resp_pld->running_task_id,
            resp_pld->subtask_id,
            client_info->socket,
            resp_pld->status,
            resp_pld->output);

    client_info->is_busy = FALSE;

}


/*chaitanya : handler to handle client heartbeat messages*/
void heartbeat_handler(struct client_info_data *client_info, 
        const char *payload)
{
    //TODO : chaitanya - in the group list also update the heartbeat 
    client_info->heartbeat_epoch_seconds = (long int) atoi(payload);
    debug_print_2("client_socket = %d, heartbeat = %ld \n",client_info->socket, client_info->heartbeat_epoch_seconds); 
    /*
    client_heartbeat_info *node;
    node = search_client_heartbeat_info(head, client_sockfd);
    if(node == NULL)
    {
        printf("error : node not found\n");
        exit(1);
    }
    node->heartbeat_epoch_seconds = (long int) atoi(payload) ;
    */
}

void quit_handler(struct client_info_data *client_info)
{
}
