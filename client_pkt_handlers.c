#include "client_pkt_handlers.h"
#include "client.h"
#include "common_hdr.h"

#include <pthread.h>

void 
msg_chk_free_handler(
			const void *payload, const size_t pyld_size, 
			const pkt_type type)
{

	int numbytes;

	if (busy_state) {
		numbytes = pkt_send(sockfd, 
			MSG_FREE_NOK, NULL, 0);
   	} else {
		numbytes = pkt_send(sockfd, 
			MSG_FREE_OK, NULL, 0);
	}
	if (-1 == numbytes) {
            perror("send");
    }   

}

void 
msg_task_assign_handler(
			const void *payload, const size_t pyld_size, 
			const pkt_type type)
{
    struct msg_task_assign_pld *task_pld;
    task_pld = (void *)payload;

    printf("Received a subtask. subtask_id: %d\n", task_pld->subtask_id );
    printf("data: %s\n", task_pld->data); 
    
}

void *client_recv_thread_fn(void *t_args)
{
	void *rx_buffer = NULL;
	size_t rx_sz;
	pkt_type type;
	int ret;


	while (1) {
		debug_print("recv from server socket %s", "");
		ret = pkt_recv (sockfd, &rx_buffer, &rx_sz, &type);
		if (-1 == ret || !rx_buffer) {
			perror ("recv");
		}


		switch (type) {
		case MSG_CHK_FREE:
				msg_chk_free_handler(rx_buffer, rx_sz, type);
                break;
        case MSG_TASK_ASSIGN:
                msg_task_assign_handler(rx_buffer, rx_sz, type);
                break;

		default:
			printf("\nUnexpected message!!!");
		}
		free(rx_buffer);
	}
	pthread_exit(NULL);
}
/*Chaitanya : periodic heartbeat client thread */
void *client_heartbeat_thread_fn(void *t_args)
{
    while(1)
    {
        sleep(3);
        int numbytes = 0;
        char buf[MAXBUFFER];
        time_t now;
        
#if 0
        char buf_temp[80];

        strftime(buf_temp, sizeof(buf_temp), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
        debug_print_2("%s\n", buf_temp);
#endif
        now = get_epochseconds_now();
        debug_print_2("Epoch: %ld\n", (long) now);
        sprintf(buf, "%ld", (long)now);

        numbytes = pkt_send(sockfd, MSG_HEARTBEAT, buf, strlen(buf));
        if (-1 == numbytes)
        {
            perror("send");
        }
    }
    pthread_exit(NULL);
}

