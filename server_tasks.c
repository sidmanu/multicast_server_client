#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>

#include "server.h"
#include "server_db.h"

static int file_exists (const char *filename)
{
  struct stat buffer;   
  return (stat (filename, &buffer) == 0);
}


static int get_file_chunks(char *mother_file,
		int num_chunks)
{
	FILE *in_fp;
	size_t sz;

	int chunk_sz;	
	in_fp = fopen(mother_file, "r");
	fseek(in_fp, 0L, SEEK_END);
	sz = ftell(in_fp);
	printf("\nTotal file size: %zu", sz);
	chunk_sz = sz/num_chunks;
	printf("\nChunk size: %d", chunk_sz);
	fseek(in_fp, sz, SEEK_SET);

	return 0;
} 

static void
run_task_sum_csv()
{
	char file_name[MAXFILENAME];
	struct client_info_list_node *client_list_node;
	struct group_info_node *grp;
	int chosen_grp_id;
    
    strncpy(file_name, "./sample.csv", MAXFILENAME);
    //TODO - uncomment this
#if 0
	printf("\nEnter the input file location:");	
	scanf("%s", file_name);
#endif
	printf("\nInput file: %s", file_name);
	
	if (!file_exists(file_name)) {
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

	get_file_chunks(file_name, 2);	
	grp = db_get_group_by_grp_id(chosen_grp_id);		

	if (grp) {
		for (client_list_node = grp->members;
			client_list_node != NULL; 
			client_list_node = client_list_node->next) {
		
			printf("Client-id: %d, ", client_list_node->data->socket);

		}

	
	}
	
		
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
