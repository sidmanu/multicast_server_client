#include "server.h"
#include <pthread.h>
#include <unistd.h>

void *periodic_print_thread(void *t_args)
{
    group_info *head = g_head;
    while(1)
    {
        head = g_head;
        sleep(10);
        while(head)
        {         
            printf("Client id:%d, message buff:[%s], group:%d \n",
                    head->client_sockfd,
                    head->client_message,
                    head->group_id);
            head = head->next;
        }
      
    }
    pthread_exit(NULL);
}


void *client_handler_thread(void *t_args)
{
    thread_arguments *args=(thread_arguments *)t_args;
	int ret;
	size_t rx_sz;
	pkt_type type;
	char *rx_buffer = NULL;
	
	while(1)
	{
     	ret = pkt_recv(args->client_sockfd, &rx_buffer, &rx_sz, &type);
        
        /* if recv() returns -1 then some error occured while receiving,
         * if returns 0 client has disconnected,
         * else it will returns the number of bytes received
         */
        if(-1 == ret || !rx_buffer) {
	    	perror("recv");
	   		close(args->client_sockfd);
    	   	break;
     	}
		//printf("One packet received!. Pkt type: %d Payload size: %zu\n", type, rx_sz);

		switch(type) {
			case MSG_JOIN:
				join_handler(args, rx_buffer, rx_sz, type);
				break;
			
			case MSG_HELLO:
				hello_handler(args, rx_buffer, rx_sz, type);
				break;
			
			case MSG_QUIT:
				quit_handler(args);	
				break;

			default:
				printf("Unknown packet!!!!");
		}
		free(rx_buffer);
	}
	assert(0);
}
