CC = gcc
CFLAGS = -g -lpthread 

all: net webserver webserver_multi client 

net: net.c
	$(CC) $(CFLAGS) -c net.c

webserver: webserver.c net.c webserver.h
	$(CC) $(CFLAGS) -o $@ webserver.c net.c

webserver_multi: webserver_multi.c net.o webserver.h
	$(CC) $(CFLAGS) -o $@ webserver_multi.c net.c -lpthread

client: client.c
	$(CC) $(CFLAGS) -o $@ client.c -lpthread

clean:
	rm -f webserver webserver_multi client
