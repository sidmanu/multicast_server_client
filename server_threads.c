#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include "common_hdr.h"
#include "server.h"
#include "server_db.h"


static void print_grps_info()
{
	struct tm *tm_info;
	time_t timer;
	char time_buf[MAXDATEBUF];
	struct group_info_node *grp_node = db.group_list;
	struct client_info_list_node *cl_node;

	time(&timer);
	tm_info = localtime(&timer);
	strftime(time_buf, MAXDATEBUF, "%Y-%m-%d %H:%M:%S", tm_info);

	while (grp_node) {

		printf("\n>>%s:[Grp_id:%d]Members:", time_buf, grp_node->grp_id);
		cl_node = grp_node->members;	

		while (cl_node) {
            printf("client_fd:%d, ", cl_node->data->socket);
			cl_node = cl_node->next;
		}	
	
		grp_node = grp_node->next;
	}

	fflush(stdout);
	
}

static void print_clients_info() 
{
	struct tm *tm_info;
	time_t timer;
	char time_buf[MAXDATEBUF];
	struct client_info_list_node *node;

	node = db.client_list;
	time(&timer);
	tm_info = localtime(&timer);
	strftime(time_buf, MAXDATEBUF, "%Y-%m-%d %H:%M:%S", tm_info);

	while(node)
	{


#if 0
		if (strlen(node->data->buffer) <= 1) {
			node = node->next;
			continue;	
		}
#endif
        /* chaitanya : print heartbeat seconds */
		debug_print("[%s] Client (%s,%d): heartbeat:[%ld] buffer:[%s]\n",
				time_buf,
				node->data->hostname,
				node->data->port,
                node->data->heartbeat_epoch_seconds,
				node->data->buffer);
		node = node->next;
	}

}


void *periodic_print_thread_fn(void *t_args)
{
	while(1)
	{
		sleep(60);
		debug_print("\n%d minute elapsed.\n Printing clients status\n", 1);
		print_clients_info();
		debug_print("\nPrinting group %s", "status");
		print_grps_info();

	}
	pthread_exit(NULL);

}


void *user_interactor_thread_fn(void *t_args)
{

	task_menu();	

	pthread_exit(NULL);
}

/*chaitanya :  */
void *heartbeat_thread_fn(void *t_args)
{
    // TODO : chaitanya
       //loop through the lninkedlist and eliminate which has the heartbeat less than 1 min chaitanya
       //After having the working group and client data structures.
       //client structure is not printing anything in the periodic print thread
}
