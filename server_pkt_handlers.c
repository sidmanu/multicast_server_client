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
		debug_print("Group %d not found! creating...", grp_id);
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

/*chaitanya : handler to handle client heartbeat messages*/
void heartbeat_handler(struct client_info_data *client_info, 
        const char *payload)
{
    //TODO : chaitanya - in the group list also update the heartbeat 
    client_info->heartbeat_epoch_seconds = (long int) atoi(payload);
   // printf("heartbeat = %d \n",client_info->heartbeat_epoch_seconds); 
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
	pthread_exit(NULL);
}
