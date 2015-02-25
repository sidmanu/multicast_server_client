#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>

#include "common_hdr.h"
#include "common_utils.h"
#include "client_pkt_handlers.h"


/* Global data structures */
int sockfd;
int busy_state;
pthread_t client_recv_thread;
/*Chaitanya : */
pthread_t client_heartbeat_thread;

/* Prototypes */
static void *get_in_addr(struct sockaddr *sa);
static void sigint_handler(int s);
static void init_signal_handler();
static int init_socket(int argc, char *argv[]);
static void init_client_recv_thread();
/*chaitanya : */
static void init_client_heartbeat_thread();

static void join_grps(int *grp_list, int grp_list_len);

int 
main(int argc, char *argv[])
{
    int numbytes;
    char buf[MAXBUFFER];    

	int *grp_list;
	int grp_list_len;

    init_signal_handler();

	busy_state = FALSE;

    if (init_socket(argc, argv) != 0) {
        fprintf(stderr,"Unable to setup client (socket, bind, listen)\n");
        exit(1);
    }

	util_get_int_list_from_csv(argv[3], &grp_list,
				&grp_list_len);

	join_grps(grp_list, grp_list_len);


	init_client_recv_thread();
    /*Chaitanya : Initialize the thread which will periodically send heartbeat*/
    init_client_heartbeat_thread();

    while (1) {
        printf("Type a message. (Max 50 chars):\n");
        scanf("%50s", buf);
        numbytes = pkt_send(sockfd, MSG_HELLO, buf, strlen(buf));
        if (-1 == numbytes) {
            perror("send");
        }    
    }    

    return 0;
}


static void join_grps(int *grp_list, int grp_list_len)
{

	struct msg_join_grp_pld *pld;
	int numbytes;
	int i;
	
	pld = malloc(sizeof(*pld));
	memset(pld, 0, sizeof(*pld));

	if (!pld) {
		perror("Ran out of memory!");
		exit(1);
	}


	for (i = 0; i < grp_list_len; i++) {
		printf("Joining group %d\n", grp_list[i]);
		pld->grp_id = htons(grp_list[i]);	
    	numbytes = pkt_send(sockfd, MSG_JOIN_GRP, 
						pld, sizeof(*pld));
		if (numbytes == -1) {
			printf("Error in sending MSG_JOIN_GRP");
		}	
	}


	free(pld);
}

static int 
init_socket(int argc, char *argv[]) 
{
    struct addrinfo hints, *servinfo, *p;
    char s[INET6_ADDRSTRLEN];
    int rv;

    if (argc != 4) {
        fprintf(stderr,"usage: %s <server-port-num> <server-hostname>"
                " <list-of-groups-to-join-comma-separated>\n",
                argv[0]);
        return -1; 
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if ((rv = getaddrinfo(argv[2], argv[1], &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return -1;
    }

    /* Any one of these could work! */
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }
        break;
    }

    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return -1; 
    }
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    printf("client: connecting to %s\n", s);
    freeaddrinfo(servinfo); 

    return 0;
}

static void 
*get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


static void 
sigint_handler(int s)
{
    pkt_send(sockfd, MSG_QUIT, 0, 0);
    close(sockfd);
    printf("Thanks for running the client. Exiting...\n");
    exit(0);
}

static void 
init_signal_handler()
{
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_handler = sigint_handler;
    if (-1 == sigaction(SIGINT, &sa, NULL)) {
        perror("sigaction sigint");
        exit(1);
    }   
}

static void 
init_client_recv_thread()
{
    pthread_create (&client_recv_thread, NULL, 
		client_recv_thread_fn,
        NULL);
}

static void
init_client_heartbeat_thread()
{
    pthread_create (&client_heartbeat_thread, NULL, 
        client_heartbeat_thread_fn,
        NULL);
}


