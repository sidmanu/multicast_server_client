#ifndef _SERVER_DB_H
#define _SERVER_DB_H


#include <time.h>
#include "server.h" 

/* Global data structure */
extern struct global_db db; 

void init_global_db();

/*  
--------------------    --------------------
| Client 1 info ptr| -> | Client 2 info ptr |
--------------------    --------------------
*/

struct client_info_data {
	char info[MAXDESC];
	int socket;
    /*Chaitanya - to store heartbeat seconds in epoch*/
    long int heartbeat_epoch_seconds;
    char hostname[MAXHOSTNAME];
	int port; 
	int pkt_count;
	char buffer[MAXBUFFER];
	/* TODO: Add a list of pointers to group it belongs to. 
		so that we don't have to go through all groups in order to 
		delete a node */
};

struct client_info_list_node {
	struct client_info_data *data;
	struct client_info_list_node *next;
};





struct global_db {
	struct client_info_list_node *client_list;
	struct group_info_node *group_list;
    struct running_task *task_list;
    int clients_count;
};

typedef enum {
    TASK_UNASSIGNED = 1,
    TASK_ASSIGNED,
    TASK_RUNNING,
    TASK_ABORTED,
    TASK_COMPLETED
} task_status_t ;

typedef enum {
    SUBTASK_NOT_DISPATCHED = 1,
    SUBTASK_DISPATCHED,
    SUBTASK_RUNNING,
    SUBTASK_COMPLETED,
    SUBTASK_TIMED_OUT
} subtask_status_t ;



/* -------------Client Mgmt API-------------------*/
/* When a new client joins, with list of groups */
void db_client_new(struct client_info_data *data);
void db_client_del(struct client_info_data *data);
struct client_info_data *db_get_client_by_socket(int sock_id);


/* When a client dies, remove it from global list 
 * and also from other group free&busy lists */


/*  
---------------------------------------
| Group1   					          |
| Members -> [client 1] -> [client 2] |
---------------------------------------
			|
			|
			Y
		Next Group
*/


struct group_info_node {
	int grp_id;
    int num_members;
	struct client_info_list_node *members;
	struct group_info_node *next;
};

void db_group_new(int group_id);
int db_group_add_member(int group_id, 
				struct client_info_data *client_info);
struct group_info_node *db_get_group_by_grp_id(int grp_id);


struct running_subtask {
    int parent_task_id;
    int subtask_id;
    subtask_status_t status;
    struct client_info_data *client;
	time_t start_time;
	time_t end_time;
    int input_data_len;
    char *input_data;
    struct running_subtask *next;
};

/* Task management */
struct running_task {
    int grp_id;
	int task_id;
    task_status_t status;
	task_type_t task_type;
	char desc[MAXDESC];
	time_t start_time;
	time_t end_time;
	char input_file[MAXFILENAME];
    int num_clients;
    struct running_subtask *subtasks_head;
    struct running_task *next;
};

struct input_task {
	char input_file[MAXFILENAME];	
};


/* Running task API */
int db_task_new(int task_id,
        int grp_id,
        task_type_t type,
        const struct input_task *task,
        struct running_task **pp_task);

struct running_task * db_get_task_by_task_id(int task_id);

/* Sub task API */

int db_subtask_new(int subtask_id,
        int input_data_len,
        char *input_data,
        struct running_task *pp_run_task);

struct running_subtask * db_get_subtask_by_subtask_id(int subtask_id,
        struct running_subtask *subtasks_head);


void db_subtasks_print(struct running_task *p_run_task);
#endif
