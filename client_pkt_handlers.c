#include "client_pkt_handlers.h"
#include "client.h"

#include <pthread.h>

void 
get_avail_handler(
			const void *payload, const size_t pyld_size, 
			const pkt_type type)
{

	int numbytes;

	if (busy_state) {
		numbytes = pkt_send(sockfd, 
			MSG_CHK_AVAIL_RESP_NOK, NULL, 0);
   	} else {
		numbytes = pkt_send(sockfd, 
			MSG_CHK_AVAIL_RESP_OK, NULL, 0);
	}
	if (-1 == numbytes) {
            perror("send");
    }   

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
		case MSG_CHK_AVAIL:
				get_avail_handler(rx_buffer, rx_sz, type);
				pthread_exit(NULL);
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
        time_t epoch_seconds, now;
        struct tm  ts;
        char buf_temp[80];

        time(&now);
        ts = *localtime(&now);
        strftime(buf_temp, sizeof(buf_temp), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
        debug_print_2("%s\n", buf_temp);
        epoch_seconds = mktime(&ts);
        debug_print_2("Epoch: %ld\n", (long) epoch_seconds);
        sprintf(buf, "%ld", (long)epoch_seconds);

        numbytes = pkt_send(sockfd, MSG_HEARTBEAT, buf, strlen(buf));
        if (-1 == numbytes)
        {
            perror("send");
        }
    }
    pthread_exit(NULL);
}

