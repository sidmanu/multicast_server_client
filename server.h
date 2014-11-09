#ifndef _CAPS_SERVER_H
#define _CAPS_SERVER_H

#include "common_hdr.h"

/* Server Client Group Mgmt */
typedef struct group_info
{
    int client_sockfd;
    char client_message[MAXBUFFER];
    int group_id;
    struct group_info *next;
}group_info;

typedef struct
{
    int client_sockfd;
    group_info *client_info;
} thread_arguments;

extern int num_of_clients[2];
extern thread_arguments thread_args[MAX_NUM_OF_CLIENTS];
extern pthread_mutex_t num_of_active_clients_lock;
extern group_info *g_head;

/* Thread functions */
void *client_handler(void *t_args);
void *periodic_print_t(void *t_args);


/* Server packet handlers */
void join_handler(thread_arguments *args,
			const char *payload, const size_t pyld_size, 
			const pkt_type type);

void hello_handler(thread_arguments *args,
			const char *payload, const size_t pyld_size, 
			const pkt_type type);

void quit_handler();


/* Thread functions */
void *periodic_print_thread(void *t_args);
void *client_handler_thread(void *t_args);

#endif
