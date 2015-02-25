#ifndef _COMMON_HDR_H
#define _COMMON_HDR_H

#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>

#define NOFLAGS 0
#define MAXNUMCLIENTS 10
#define MAXDATASIZE 100 
#define MAXBUFFER 1024*1024 
#define MAXHOSTNAME 48 
#define MAXDESC 2048
#define MAGIC 0xCA0000CB
#define MAXDATEBUF 25
#define MAXFILENAME 25
#define MAXCHUNKSIZE 1024 
#define DEBUG 1
#define DEBUG_2 0
#define DEBUG_3 0

#define debug_print(...) \
    do { if (DEBUG) printf(__VA_ARGS__); } while (0)
//#define debug_print(fmt, ...) \
  //  do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

#define debug_print_2(fmt, ...) \
    do { if (DEBUG_2) fprintf(stderr, "\n%s:%d:%s(): " fmt, __FILE__, \
                              __LINE__, __func__, __VA_ARGS__); } while (0)

#define debug_print_3(fmt, ...) \
    do { if (DEBUG_3) fprintf(stderr, "\n%s:%d:%s(): " fmt, __FILE__, \
                              __LINE__, __func__, __VA_ARGS__); } while (0)
enum truth_vals{
	FALSE = 0,
	TRUE = 1,
};

typedef enum {
	MSG_HELLO = 1,
	MSG_JOIN_GRP,
	MSG_HEARTBEAT,
	MSG_GET_GRP_LIST_FOR_TASK,
	MSG_GET_GRP_LIST_FOR_TASK_RESP,
	MSG_CHK_FREE,
	MSG_FREE_OK,
	MSG_FREE_NOK,
	MSG_TASK_ASSIGN,
	MSG_TASK_RESULT,
	MSG_QUIT,
} pkt_type;

typedef enum {
	STATUS_SUCCESS = 0,
	STATUS_FAILURE,
	STATUS_ABORTED,
} status_code_t;
	
/* Payloads */

struct msg_join_grp_pld {
	int grp_id;
};


struct msg_chk_free_pld {
	int task_id;
};

struct msg_chk_free_resp_pld {
	enum truth_vals free;
};

struct msg_task_assign_pld {
    int len;
    int running_task_id;
    int subtask_id;
    char data[0];
};

struct msg_task_result_pld {
    status_code_t status; 
    int running_task_id;
    int subtask_id;
    long output;
};


typedef enum {
	TASK_SUM = 1,
} task_type_t;

struct pkt {
	int magic;
	pkt_type type;
	size_t	 len;
	char data[0];
} PACKED;

static inline long int get_epochseconds_now(void)
{
    time_t epoch_seconds, now;
    struct tm  ts;

    time(&now);
    ts = *localtime(&now);
    epoch_seconds = mktime(&ts);
    return epoch_seconds;
}

static inline size_t pkt_send(int fd, pkt_type type, void *data, size_t data_size)
{
	struct pkt *p = NULL;
	size_t sz;

	int data_sent;
	int offset = 0;

	sz = sizeof(struct pkt) + data_size;
	p = malloc(sz);

	if (!p) {
		perror("Error in malloc of packet");
		assert(0);
	}
	// fill packet
	memset(p, 0, sz);
	p->magic = MAGIC;
	p->type = type;
	p->len = data_size;
	memcpy(p->data, data, data_size);
#ifdef LOG
	printf("Sending pkt size:%zu payload_len: %zu pkt type:%d\n",
			sz, p->len, p->type);
#endif

	do {
		data_sent = send(fd, (const char *) p + offset, sz, NOFLAGS);
		if (data_sent < sz ) {
			sz = sz - data_sent;
			offset = offset +data_sent;
		} else {
			//printf("All data sent at once. breaking\n");
			break;
		}	
	} while(1);
	free(p);

	return data_size;
} 

static inline int pkt_recv(int fd, void **rx_buf, size_t *data_size, pkt_type *type)
{
	*rx_buf = NULL;

	// first receive only as much as to get type & length

	struct pkt dummy;
	size_t pkt_len;
	size_t payload_len;
	size_t rcvd_bytes;
	int offset = 0;
	
	pkt_len = recv(fd, &dummy, sizeof(struct pkt), NOFLAGS);

#ifdef LOG
	printf("Rxbytes: %zu, magic: %x pkt_type: %d, payload_len: %zu\n", 
				pkt_len, dummy.magic, dummy.type, dummy.len);
#endif

	*type = dummy.type;
	payload_len = dummy.len;
	*data_size = payload_len;

	*rx_buf = malloc(payload_len);
	if (!rx_buf) {
		perror("Error in malloc during pkt_recv");
		return -1;
	}

	do {
		rcvd_bytes = recv(fd, (*rx_buf) + offset, payload_len - offset, NOFLAGS);
	
		if (rcvd_bytes < payload_len) {
			offset = payload_len - rcvd_bytes;
		} else {
			//printf("Entire packet received!\n");
			break;
		}	
	} while(1);

	return 0;
} 


#endif
