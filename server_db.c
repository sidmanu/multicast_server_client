#include "server_db.h"

#include <assert.h>

void init_global_db()
{
	memset(&db, 0, sizeof(struct global_db));
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
	} else {
		temp = db.client_list;
		db.client_list = node;
		node->next = temp;
	}
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
	group_info->grp_id = group_id;
	group_info->next = NULL;

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



