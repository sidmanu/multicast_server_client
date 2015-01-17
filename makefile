
CC=/usr/bin/gcc
CCFLAGS = -g -I.
LIBS = -lpthread

SERVER_OBJS = server_threads.o server.o server_pkt_handlers.o server_tasks.o server_db.o common_utils.o
CLIENT_OBJS = client.o common_utils.o client_pkt_handlers.o

all: server client

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CCFLAGS) $(LIBS)

server: $(SERVER_OBJS) 
	$(CC) -o $@ $(SERVER_OBJS) $(CCFLAGS) $(LIBS) 


client: $(CLIENT_OBJS) 
	$(CC) -o $@ $(CLIENT_OBJS) $(CCFLAGS) $(LIBS) 

clean:
	rm -rf *.o client server *core*
