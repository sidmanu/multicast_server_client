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
#include "server_db.h"
#include "server_pkt_handlers.h"


/* global variables */

struct global_db db; 

pthread_t periodic_thread;
pthread_t user_interactor_thread;
/*chaitanya : hearbeat thread on server*/
pthread_t heartbeat_thread;
int server_sockfd;

static void init_periodic_print();
static void init_user_interactor();
static void init_heartbeat_thread();
static int init_server_socket(int argc, char *argv[]);
static void new_connection(fd_set *master_fds, int *fdmax);
static void handle_client_recv(fd_set *master, int sock_id);

int
main (int argc, char *argv[])
{

    int i = 0;
    fd_set master;
    fd_set read_fds;
    int fdmax;
    int ret;

	init_global_db();
    ret = init_server_socket(argc, argv);
    if (0 != ret) {
        fprintf(stderr, "unable to create socket!\n");
        exit(1);    
    }

	/* Run two other threads */
    init_periodic_print();
    init_user_interactor();
    /*chaitanya : */
    init_heartbeat_thread();

    FD_ZERO (&master);
    FD_SET (server_sockfd, &master);
    fdmax = server_sockfd;

    while (1) {
        read_fds = master;
        if (select (fdmax + 1, &read_fds, 
				NULL, NULL, NULL) == -1) {
            perror ("select");
            exit (4);
        }
        for (i = 0; i <= fdmax; i++) {
            if (FD_ISSET (i, &read_fds)) {
                if (i == server_sockfd) {
                    new_connection(&master, &fdmax);
                } else {
                    handle_client_recv(&master, i);
                }
            }            // end got new incoming connection
        }
    }
    return 0;
}

static void 
init_periodic_print()
{
    pthread_create (&periodic_thread, NULL, periodic_print_thread_fn,
            NULL);
}

static void 
init_user_interactor()
{
    pthread_create (&user_interactor_thread, NULL, user_interactor_thread_fn,
            NULL);
}
/*chaitanya : heartbeat thread in server*/
void
init_heartbeat_thread()
{
    pthread_create (&heartbeat_thread, NULL, heartbeat_thread_fn,
            NULL);
}

static void 
handle_client_recv(fd_set *master, int sock_id)
{
    void *rx_buffer = NULL;
    size_t rx_sz;
    pkt_type type;
    int ret;
	struct client_info_data *client_info = NULL;

    debug_print_2 ("recv from the client socket: %d\n", sock_id);
    ret = pkt_recv (sock_id, &rx_buffer, &rx_sz, &type);
    if (-1 == ret || !rx_buffer) {
        perror ("recv");
        close (sock_id);
        FD_CLR (sock_id, master);    // remove from master set
    } else {

        /* if recv() returns -1 then some error occured while receiving,
         * if returns 0 client has disconnected,
         * else it will returns the number of bytes received
         */
        if (-1 == ret || !rx_buffer) {
            perror ("recv");
            close (sock_id);
            return;
        }
        //printf("one packet received!. pkt type: %d payload size: %zu\n", type, rx_sz);

		client_info = db_get_client_by_socket(sock_id);	

		//TODO: Identify the client based on socket
		

        switch (type)
        {
            case MSG_JOIN_GRP:
                join_handler(client_info, rx_buffer, rx_sz, type);
                break;

            case MSG_HELLO:
                hello_handler(client_info, rx_buffer, rx_sz, type);
                break;
            /*chaitanya : to handle heartbeat messages fromt he client*/
            case MSG_HEARTBEAT:
                heartbeat_handler(client_info, rx_buffer);
                break;

            case MSG_QUIT:
                quit_handler(client_info); 
                break;

            default:
                printf ("unknown packet!!!!");
        }
        free (rx_buffer);
    }
}


static int 
init_server_socket(int argc, char *argv[])
{
    struct sockaddr_in server;
    if (argc != 2) {
        fprintf (stderr, "usage: %s <port-num>\n", argv[0]);
        return -1;
    }

    int server_port_num = atoi (argv[1]);
    server_sockfd = socket (AF_INET, SOCK_STREAM, 0);
    if (server_sockfd < 0)
    {
        perror ("server: socket");
        return -1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons (server_port_num);
    memset (&(server.sin_zero), 0, sizeof (server.sin_zero));

    if (-1 == bind (server_sockfd, (struct sockaddr *) &server,
                sizeof (struct sockaddr)))
    {
        close (server_sockfd);
        perror ("server: bind");
        return -1;
    }

    /* listen */
    if (-1 == listen (server_sockfd, MAXNUMCLIENTS))
    {
        perror ("listen");
        return -1;    
    }

    printf ("server: waiting for connections... \n");
    return 0;
}


static void 
new_connection(fd_set *master_fds, int *fdmax)
{
    int newfd;
    struct sockaddr_in client_addr;
    socklen_t sin_size;

	struct client_info_data *data;
	data = malloc(sizeof(*data));
	if (!data) {
		perror("new_connection: Out of memory!\n");
		exit(1);
	}

	memset(data, 0, sizeof(*data));

    sin_size = sizeof (client_addr);
    newfd = accept (server_sockfd, (struct sockaddr *) &client_addr,
            &sin_size);
    if (newfd == -1) {
        perror ("accept");
        return;
    } else {
        debug_print_3("accept the new connection from newfd=%d\n",newfd);
        FD_SET (newfd, master_fds); // now we'll also select() on this fd 
        if (newfd > *fdmax) {
            /* max fd might have changed */
            *fdmax = newfd;
        }

		data->socket = newfd;
		strncpy(data->hostname, inet_ntoa(client_addr.sin_addr), 
				MAXHOSTNAME);
		data->pkt_count = 0;
		data->port = ntohs(client_addr.sin_port);
		db_client_new(data);
        printf ("\nNew client joined (%s , %d)\n",
                data->hostname,
                data->port);
    }
}


void quit_app() 
{
	//TODO: close all connections
	printf("\nGracefully exiting!");
	exit(0);
}
