#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "webserver.h"
#include <unistd.h>
#include <sys/time.h>
#include <semaphore.h>
#define MAX_REQUEST 100

int n_items_in_buffer=0;

pthread_mutex_t lock;
sem_t sem_empty;
sem_t sem_full;
int s;
int numThread, port;
// int k=0;
int socketBuffer[MAX_REQUEST+1];
// int n=0;
// int processed=0;
int sock;


void *listener()
{
	int r;
	struct sockaddr_in sin;
	struct sockaddr_in peer;
	int peer_len = sizeof(peer);
	sock = socket(AF_INET, SOCK_STREAM, 0);
    // printf("%d =======\n", sock);
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(port);
	r = bind(sock, (struct sockaddr *) &sin, sizeof(sin));
	if(r < 0) {
		perror("Error binding socket:");
		return NULL;
	}
	r = listen(sock, 5);
	if(r < 0) {
		perror("Error listening socket:");
		return NULL;
	}
	printf("HTTP server listening on port %d\n", port);
}

void *producer() 
{
    printf("To [Producer] produce an item\n");
	while (1)
	{
            int s;
            s = accept(sock, NULL, NULL);
            if (s < 0) break;
        sem_wait(&sem_empty);
        pthread_mutex_lock(&lock);
            socketBuffer[n_items_in_buffer]=s;
            n_items_in_buffer++;
        printf("\n[Producer] added the item to the buffer. # items in buffer = %d",n_items_in_buffer+1);
        pthread_mutex_unlock(&lock);
        sem_post(&sem_full);
	}
    close(sock);
}

void *consumer() 
{
    int fd;
    int n=0;
    while(n_items_in_buffer+1>0)
    {
        sem_wait(&sem_full); 
        pthread_mutex_lock(&lock);
        fd = socketBuffer[0];
                n_items_in_buffer--;
                int m=0;
                for(m=0;m<n_items_in_buffer;m++)
                {
                    socketBuffer[m]=socketBuffer[m+1];
                }
                // socketBuffer[]
                printf("\n[Consumer %d] removed an item from the buffer. # items in buffer = %d\n", syscall(186)-getpid(), n_items_in_buffer);
        pthread_mutex_unlock(&lock);
        sem_post(&sem_empty);
        process(fd);
    }
        // consumer();
}


int main(int argc, char *argv[])
{
    int i;
    // c[MAX_REQUEST];
    if(argc != 3 || atoi(argv[1]) < 2000 || atoi(argv[1]) > 50000)
	{
		fprintf(stderr, "./webserver_multi PORT(2001 ~ 49999) #_of_threads\n");
		return 0;
	}
    port = atoi(argv[1]);
	numThread = atoi(argv[2]);
    if(numThread>MAX_REQUEST)
    {
        printf("Maximum thread limit is 100 Limit Exceeded");
        exit(-1);
    }
    pthread_t threads[numThread+1]; 
    pthread_mutex_init(&lock, NULL);
    sem_init(&sem_empty, 0, MAX_REQUEST);
    sem_init(&sem_full, 0, 0);
    listener();
    for(i= 0;i<numThread+1;i++)
    {
        if(i==0)
        {
            pthread_create(&threads[i], NULL, producer , NULL);
        }
        else
        {
            // printf("\n Created = %d",&(c[i]));
            pthread_create(&threads[i], NULL, consumer, NULL);
        }
    }
    // join waits for the threads to finish
    for(i= 0;i< numThread+1;i++)
    {
        pthread_join(threads[i], NULL);
    }
    //sem_destroy(&sem_empty);
    //sem_destroy(&sem_full);
    return 0;
}