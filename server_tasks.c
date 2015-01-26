#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#include "server.h"
#include "server_db.h"
#include "server_coordinator.h"

static int file_exists (const char *filename)
{
  struct stat buffer;   
  return (stat (filename, &buffer) == 0);
}

static void
run_task_sum_csv()
{
	struct input_task task;
	struct client_info_list_node *client_list_node;
	struct group_info_node *grp;
	int chosen_grp_id;
    
    strncpy(task.input_file, "./sample.csv", MAXFILENAME);
    //TODO - uncomment this
	printf("\nInput file: %s", task.input_file);
	
	if (!file_exists(task.input_file)) {
		fprintf(stderr, "\nInput file doesn't exist!!");
		return;
	}


	printf("\nChoose a worker-group from the following:\n");	

	grp = db.group_list;
	while (grp) {
		printf("%d, ", grp->grp_id);
		grp = grp->next;
	}
	printf("\n");
	scanf("%d", &chosen_grp_id);

	server_dist_task_to_clients(chosen_grp_id, &task); 
}



static void
exec_choice(int choice)
{
	switch(choice) {
	case 1:
		run_task_sum_csv();
		break;
	case 2:
		quit_app();
		break;
	default:
		printf("\nInvalid choice!");
	}
}


void task_menu()
{
	int choice;
    printf("\n=========TASK MENU============\n");	
    printf("\n1. Run Task: Find the sum of numbers in a csv file");
    printf("\n2. Quit App");
    printf("\nPlease choose an option from the above:\n");
    scanf("%d", &choice);
    exec_choice(choice);

    printf("\n=============xxx==============\n");	

}
