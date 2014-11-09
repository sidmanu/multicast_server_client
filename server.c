#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>

#include "common_hdr.h"
#include "server.h"


/* Global variables */
group_info *g_head = NULL;
int num_of_clients[2] = {0,0};
thread_arguments thread_args[MAX_NUM_OF_CLIENTS];
pthread_mutex_t num_of_active_clients_lock = PTHREAD_MUTEX_INITIALIZER;



void list_add(group_info **head, group_info **new_node) 
{
	group_info *temp;
	if (*head == NULL) {
		*head = *new_node;
		return;
	} 

	/* add at head */
	temp = *head;
	(*new_node)->next = temp;
	*head = *new_node;
}



int main(int argc, char *argv[])
{

    int server_sockfd; 
    struct sockaddr_in server, client_addr;
    socklen_t sin_size;
    int num_threads = 0;
    pthread_t t2[MAX_NUM_OF_CLIENTS];
    pthread_t periodic_thread;
    int *args = NULL;
    int server_port_num = atoi(argv[1]); 
	int j;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port-num>\n", argv[0]);
        return 0;
    }

    /* Thread which will run periodically and print the output*/
    pthread_create(&periodic_thread, NULL, periodic_print_thread,(void *)args);
    
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd < 0) {
        perror("server: socket");
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr  = INADDR_ANY;
    server.sin_port = htons(server_port_num);
    memset(&(server.sin_zero),0, sizeof(server.sin_zero));

    if (-1 == bind(server_sockfd, (struct sockaddr *)&server,
				 sizeof(struct sockaddr)))
    {
        close(server_sockfd);
        perror("server: bind");
        exit(1);
    }

    /* Listen*/
    if (-1 == listen(server_sockfd, MAX_NUM_OF_CLIENTS)) {
        perror("Listen");
        exit(1);
    }

    printf("server: waiting for connections... \n");

    g_head = NULL;
    group_info *new_node = g_head;

    while(1)
    { // main accept() loop
        sin_size = sizeof(client_addr);
        thread_args[num_threads].client_sockfd = accept(server_sockfd, (struct sockaddr *) &client_addr, &sin_size);

        new_node = (group_info*) malloc(sizeof(group_info));
        new_node->client_sockfd = thread_args[num_threads].client_sockfd;
        new_node->client_message[0] = '\0';
        new_node->group_id = 0;

		list_add(&g_head, &new_node);

        thread_args[num_threads].client_info = new_node;
        if (thread_args[num_threads].client_sockfd == -1)
        {
            perror("accept");
            continue;
        }
        printf("\nGot a connection from (%s , %d)\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
		/* Create a separate thread to handle child */
        pthread_create(&t2[num_threads], NULL, client_handler_thread, (void *)&(thread_args[num_threads]));
        /*To keep a count of threads */
        num_threads++;
    }

    for (j=0; j<num_threads; j++)
        pthread_join(t2[j], NULL);

    return 0;

}
