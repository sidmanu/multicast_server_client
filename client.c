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

#include "common_hdr.h"

static void *get_in_addr(struct sockaddr *sa);
void sigint_handler(int s);
void init_signal_handler();

int sockfd;

int main(int argc, char *argv[])

{
	int numbytes;
    struct addrinfo hints, *servinfo, *p;
    int rv;

    char s[INET6_ADDRSTRLEN];
	char buf[MAXBUFFER];	

    init_signal_handler();
    if (argc != 4) {
        fprintf(stderr,"usage: %s <server-port-num> <server-hostname>"
				" <comma-separated-groups>\n",
				argv[0]);
        exit(1);
    }
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    if ((rv = getaddrinfo(argv[2], argv[1], &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
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
        exit(2);
    }
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    printf("client: connecting to %s\n", s);
    freeaddrinfo(servinfo); // all done with this structure

    printf("Select the multicast group which you want to join as one of these %s : ", argv[3]);
	scanf("%s", buf);
    numbytes = pkt_send(sockfd, MSG_JOIN, buf,strlen(buf));

    if (-1 == numbytes)
        perror("send");

	while (1) {
		printf("Type a message. (Max 50 chars):\n");
		scanf("%50s", buf);
    	numbytes = pkt_send(sockfd, MSG_HELLO, buf, strlen(buf));
    	if(-1 == numbytes)
        	perror("send");
	}	

    return 0;
}

static void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


void sigint_handler(int s)
{
   	pkt_send(sockfd, MSG_QUIT, 0, 0);
    close(sockfd);
    printf("Thanks for running the client. Exiting...\n");
    exit(0);
}

void init_signal_handler()
{
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_handler = sigint_handler;
    if (-1 == sigaction(SIGINT, &sa, NULL)) {
        perror("sigaction sigint");
        exit(1);
    }   
    
}
