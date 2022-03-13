# multi_threaded_webserver
Multi Threaded Webserver Using Producer Consumer Model

Steps to run:-

1. Make

2. ./webserver_multi <port> <number_of_threads_you_want_to_create> (Starts the Server)
  
3. ./client <localhost eg.127.0.0.1> <port> <number_of_requests>
  
  
Shared Data Structure:-
Shared Data Structure used is the buffer which contains the request sent from the client. It is accessed by the variable n_items_in_buffer which is also shared among multiple consumer threads and the producer thread. The producer pushes an item into the buffer and increments the n_items_in_buffer variable. The consumer accesses a value from the buffer and decrements the n_items_in_buffer.

Synchronization:-
Synchronization is done by using 2 Semaphores and one Mutex Lock. The Semaphore variables used are sem_full which is initialized to 0 and sem_empty which is initialized to  number of requests. The Mutex Lock is used by the producer and consumer threads for mutual exclusion and protecting the shared variables and race conditions.
Roughly the Synchronization is done in this way..

    Producer ()
  
```{{
    while(1)
   {
       -
       -
        sem_wait(&sem_empty); 
        pthread_mutex_lock(&lock);
        -
         n_items_in_buffer + +
        -
        -
        pthread_mutex_unlock(&lock);
        sem_post(&sem_full);
    
 }
```

  Consumer ()
  
`
    
        sem_wait(&sem_full); 
        pthread_mutex_lock(&lock);
        
        n_items_in_buffer - - 
        -
        -
        pthread_mutex_unlock(&lock);
        sem_post(&sem_empty);
  
`


The Solution will never have and concurrency problem because it satisfies mutual exclusion by Mutex Locks which will allow only one thread to execute in the critical section which in this case is the buffer and n_items_in_buffer variable. The Consumer cannot read if the sem_full is 0 that means there are no items to read in the buffer. The Producer will not write to the buffer if sem_emty is max that means it is already full. The Solution will not lead to deadlock as there is no blocking execution taking place within the lock.
