#ifndef _COMMON_HDR_H
#define _COMMON_HDR_H

#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

#define NOFLAGS 0
#define MAX_NUM_OF_CLIENTS 10
#define MAXDATASIZE 100 
#define MAXBUFFER 2048
#define MAGIC 0xCA0000CB



typedef enum {
	MSG_JOIN = 1,
	MSG_HELLO,
	MSG_QUIT,
} pkt_type;


struct pkt {
	int magic;
	pkt_type type;
	size_t	 len;
	char data[0];
} PACKED;


static inline size_t pkt_send(int fd, pkt_type type, char *data, size_t data_size)
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
	printf("Sending pkt size:%zu payload_len: %zu pkt type:%d\n",
			sz, p->len, p->type);

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

static inline int pkt_recv(int fd, char **rx_buf, size_t *data_size, pkt_type *type)
{
	*rx_buf = NULL;

	// first receive only as much as to get type & length
	struct pkt dummy;
	size_t pkt_len;
	size_t payload_len;
	size_t rcvd_bytes;
	int offset = 0;
	
	pkt_len = recv(fd, &dummy, sizeof(struct pkt), NOFLAGS);
	printf("Rxbytes: %zu, magic: %x pkt_type: %d, payload_len: %zu\n", 
				pkt_len, dummy.magic, dummy.type, dummy.len);

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
