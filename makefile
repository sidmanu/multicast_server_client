
CC=/usr/bin/gcc
CCFLAGS = -g -Wall -I.
LIBS = -lpthread

SERVER_OBJS = server.o server_pkt_handlers.o server_threads.o
CLIENT_OBJS = client.o

all: server client

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CCFLAGS) $(LIBS)

server: $(SERVER_OBJS) 
	$(CC) -o $@ $(SERVER_OBJS) $(CCFLAGS) $(LIBS) 


client: $(CLIENT_OBJS) 
	$(CC) -o $@ $(CLIENT_OBJS) $(CCFLAGS) $(LIBS) 

clean:
	rm -rf *.o client server *core*
