#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_THREADS 10

// Declare the semaphore globally
sem_t sem;

void *inc(void *args);

int x = 0;

void *inc(void *args)
{
    int i; //itr

    sem_wait(&sem);
 
    for(i =0; i<100; i++){
        x = x+1;
        printf("x = %d\n", x);
    }

    sem_post(&sem);

}

int main()
{
    int i; //itr

    //Initialize the semaphore in the main function
    sem_init(&sem, 0, 1);

    // declare two threads
    //int num_threads = 10;
    pthread_t t[NUM_THREADS];

    //create the threads
    for(i = 0; i < NUM_THREADS; i++){
        pthread_create(&t[i], NULL, inc, NULL);
    }

    //wait until both threads finish
    for(i=0; i<NUM_THREADS; i++){
        pthread_join(t[i], NULL);
    }
    
    return 0;
}
