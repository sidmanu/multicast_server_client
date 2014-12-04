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

    int server_sockfd, newfd; 
    struct sockaddr_in server, client_addr;
    socklen_t sin_size;
    int *args = NULL;
    int server_port_num = atoi(argv[1]); 
	int i = 0;
    fd_set master;
    fd_set read_fds;
    int fdmax;
    int ret; 
    pthread_t periodic_thread;
    size_t rx_sz;
    char *rx_buffer = NULL;
    pkt_type type;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port-num>\n", argv[0]);
        return 0;
    }

    /* Thread which will run periodically and print the output*/
    pthread_create(&periodic_thread, NULL, periodic_print_thread,(void *)args);
    
    FD_ZERO(&master);
    FD_ZERO(&read_fds);
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

    // add the listener to the master set
    FD_SET(server_sockfd, &master);
    fdmax = server_sockfd;
    g_head = NULL;
    group_info *new_node = g_head;

    while(1)
    { 
        read_fds = master; 
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) 
        {
            perror("select");
            exit(4);
        }
        for(i = 0; i <= fdmax; i++)
        {
            if (FD_ISSET(i, &read_fds))
            {
                if (i == server_sockfd) 
                {
                    printf("Received the accept request on %d\n",i);
                    // handle new connections
                    sin_size = sizeof(client_addr);
                    newfd = accept(server_sockfd, (struct sockaddr *) &client_addr, &sin_size);
                    if(newfd == -1)
                    {
                        perror("accept");
                        continue;
                    }
                    else
                    {
                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax) 
                        { // keep track of the max
                            fdmax = newfd;
                        }
                        new_node = (group_info*) malloc(sizeof(group_info));
                        new_node->client_sockfd = newfd;
                        new_node->client_message[0] = '\0';
                        new_node->group_id = 0;
                        list_add(&g_head, &new_node);
                        printf("\nGot a connection from (%s , %d)\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
                    }
                }
                else 
                {
                    // handle data from a client
                    
                    rx_buffer = NULL;
                    printf("Got the data from the client = %d\n", i);
                    ret = pkt_recv(i, &rx_buffer, &rx_sz, &type); 
                    if(-1 == ret || !rx_buffer) 
                    {
                          perror("recv");
                          close(i);
                          FD_CLR(i, &master); // remove from master set
                    } 
                    else 
                    {
                        printf("Received from %d\n", i);
        
                        /* if recv() returns -1 then some error occured while receiving,
                         * if returns 0 client has disconnected,
                         * else it will returns the number of bytes received
                         */
                        if(-1 == ret || !rx_buffer) 
                        {
                            perror("recv");
                            close(i);
                            break;
                        }
                        //printf("One packet received!. Pkt type: %d Payload size: %zu\n", type, rx_sz);

                        switch(type) 
                        {  
                            case MSG_JOIN:
            //                    join_handler(args, rx_buffer, rx_sz, type);
                                break;
            
                            case MSG_HELLO:
              //                  hello_handler(args, rx_buffer, rx_sz, type);
                                break;
            
                            case MSG_QUIT:
                //                quit_handler(args); 
                                break;

                            default:
                                printf("Unknown packet!!!!");
                        }
                        printf("buffer = %s\n",rx_buffer);
                        free(rx_buffer); 
                    }
                } // END handle data from client
            } // END got new incoming connection
            }
        }
    return 0;

}
