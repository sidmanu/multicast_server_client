#include "server_db.h"

#include <assert.h>

void init_global_db()
{
	memset(&db, 0, sizeof(struct global_db));
    printf("Initializing Global DB");
}

void db_client_new(struct client_info_data *data)
{
	struct client_info_list_node *node = NULL;
	struct client_info_list_node *temp;
	node = malloc(sizeof(*node));
	node->data = data;
	node->next = NULL;

	if (!db.client_list) {
		db.client_list = node;	
        debug_print_2("Added first client to client_db = %p",db.client_list);
	} else {
		temp = db.client_list;
		db.client_list = node;
		node->next = temp;
        debug_print_2("Added one more client to client_db = %p",db.client_list);
	}

    db.clients_count++;
    printf("Total number of clients: %d\n", db.clients_count);

}

struct client_info_data *
db_get_client_by_socket(int sock_id)
{

	struct client_info_list_node *node = NULL;
	if (!db.client_list) {
		return NULL;	
	}

	node = db.client_list;
	
	while (node) {
		if (node->data->socket == sock_id) {
			return node->data;
		}
		node = node->next;
	}
	return NULL;
}

void db_client_del(struct client_info_data *data)
{
	struct client_info_list_node *temp, *to_be_del;

	if (!db.client_list) {
		perror("Nothing to delete!");
		return;
	} 

	for (temp = db.client_list; temp->next != NULL;
				temp = temp->next) {
		if (temp->next->data == data) {
			to_be_del = temp->next;
			temp->next = temp->next->next;
			
			free(to_be_del->data);
			free(to_be_del);	
			return;
		}
	}
}

void db_group_new(int group_id)
{
	struct group_info_node *group_info;
	
	group_info = malloc(sizeof *group_info);
    memset(group_info, 0, sizeof(*group_info));
	group_info->grp_id = group_id;

	if (!db.group_list) {
		db.group_list = group_info;
		return;
	}

	/* add at head */
	group_info->next = db.group_list;	
	db.group_list = group_info;
}

int db_group_add_member(int group_id, 
				struct client_info_data *client_info)
{
	struct group_info_node *temp;
	struct client_info_list_node *client_info_node;

	temp = db_get_group_by_grp_id(group_id);
	if (!temp) {
		perror("Unable to find group\n");
		assert(0);
	}


	client_info_node = malloc(sizeof *client_info_node);
	memset(client_info_node, 0, sizeof(*client_info_node));
	client_info_node->data = client_info;
	client_info_node->next = NULL;
	
	if (!temp) {
		perror("Trying to add client to a non existing group!");
		return -1;
	}
	
	if (!temp->members) {
		temp->members = client_info_node; 
	} else {
		client_info_node->next = temp->members;
		temp->members = client_info_node;
	}
	
    temp->num_members++;
	return 0;
 
}

struct group_info_node *db_get_group_by_grp_id(int grp_id)
{
	struct group_info_node *temp;
	temp = db.group_list;

	while (temp) {
		if (temp->grp_id == grp_id)
			return temp;
		temp = temp->next;
	}	

	return NULL;
}


/* Runtime tasks */

int db_task_new(int task_id,
        int grp_id,
        task_type_t type,
        const struct input_task *task,
        struct running_task **pp_run_task)
{
	struct running_task *new;
    struct running_task *temp;

	new = db_get_task_by_task_id(task_id);
	if (new) {
		perror("Repeated task!\n");
		assert(0);
	}

    new = malloc(sizeof(struct running_task));
    if (!new) {
        debug_print("Malloc failed in %s", __FUNCTION__);
        assert(0);
    }
    memset(new, 0, sizeof(struct running_task));

    new->task_id = task_id;
    new->grp_id = grp_id;
    new->status = TASK_UNASSIGNED;
    new->start_time = get_epochseconds_now(); 
    strncpy(new->input_file, task->input_file, MAXFILENAME);

    if (db.task_list) {
        temp = db.task_list;
        db.task_list = new;
        new->next = temp;
    } else {
        db.task_list = new;
    }

    *pp_run_task = new;
    debug_print("\nCreated new task. input_file: %s task_id: %d grp_id: %d",
                new->input_file, task_id, grp_id);

	return 0;
}

struct running_task * db_get_task_by_task_id(int task_id)
{
	struct running_task *temp;
	temp = db.task_list;

	while (temp) {
		if (temp->task_id == task_id)
			return temp;
		temp = temp->next;
	}	

	return NULL;
}

/*Sub running tasks */


int db_subtask_new(int subtask_id,
        int input_data_len,
        char *input_data,
        struct running_task *p_run_task)
{
	struct running_subtask *new;
    struct running_subtask *temp;
    
	new = db_get_subtask_by_subtask_id(subtask_id, p_run_task->subtasks_head);
	if (new) {
		perror("Repeated task!\n");
		assert(0);
	}

    new = malloc(sizeof(struct running_subtask));
    if (!new) {
        debug_print("Malloc failed in %s", __FUNCTION__);
        assert(0);
    }
    memset(new, 0, sizeof(struct running_subtask));

    new->parent_task_id = p_run_task->task_id;
    new->subtask_id = subtask_id;
    new->status = SUBTASK_NOT_DISPATCHED;
    new->client = NULL;
    new->start_time = get_epochseconds_now(); 
    new->input_data_len = input_data_len;
    new->input_data =  calloc(sizeof(char), input_data_len);
    strncpy(new->input_data, input_data,  input_data_len);
    debug_print_2("\nnew->input_data: %s input_data %s", new->input_data,
                input_data);

    if (p_run_task->subtasks_head) {
        temp = p_run_task->subtasks_head;
        p_run_task->subtasks_head = new;
        new->next = temp;
    } else {
        p_run_task->subtasks_head = new;
    }

    debug_print_2("\nCreated new sub task. "
            "subtask_id: %d, parent_task_id = %d, input_data = %s\n",
                new->subtask_id, new->parent_task_id, new->input_data);

	return 0;
}

/* we may wanna modify this and pass parent_task pointer instead */
struct running_subtask * db_get_subtask_by_subtask_id(int subtask_id, 
        struct running_subtask *subtasks_head)
{
	struct running_subtask *temp = subtasks_head;

	while (temp) {
		if (temp->subtask_id == subtask_id)
			return temp;
		temp = temp->next;
	}	

	return NULL;
}

void db_subtasks_print(struct running_task *p_run_task)
{

	struct running_subtask *temp = p_run_task->subtasks_head;
	while (temp) {

        debug_print("\nsubtask_id: %d, parent_task_id = %d, input_data = %s",
                temp->subtask_id, temp->parent_task_id, temp->input_data);
		temp = temp->next;
	}	

}
