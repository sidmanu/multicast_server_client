#include "server.h"
#include <pthread.h>

void join_handler(thread_arguments *args,
			const char *payload, const size_t pyld_size, const pkt_type type)
{

		printf("Client (fd: %d) requests to join group:\"%s\"\n", args->client_sockfd,
						payload);
        /* Apply the lock for num_of_clients*/
		if (payload[0] == '1')
		{
           	num_of_clients[0]++;
            args->client_info->group_id = 1;
	   		printf("No. of clients in group %c : %d\n",payload[0],num_of_clients[0]);
	  	}
		if (payload[0] == '2')
	  	{
           	num_of_clients[1]++;
            args->client_info->group_id = 2;
	   		printf("No. of clients in group %c : %d\n",payload[0],num_of_clients[1]);
	  	}

}

void hello_handler(thread_arguments *args,
			const char *payload, const size_t pyld_size, const pkt_type type)
{

		printf("Client (fd: %d) sent a message...\n", args->client_sockfd);
       	strncat(args->client_info->client_message,payload, pyld_size);
		strcat(args->client_info->client_message, ", ");
		printf("exiting \n");
}


void quit_handler() 
{
	pthread_exit(NULL);
}
