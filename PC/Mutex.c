#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *inc(void *args);

int x = 0;

int is_locked = 1;

//mutex_lock declaration
pthread_mutex_t lock;

void *inc(void *args)
{
    int i; // itr

    if (is_locked) {
        pthread_mutex_lock(&lock);
    }
    /* add 1 a hundred times to x */
    for (i = 0; i < 100; i++) {
        x = x + 1; // add 1 to x
        printf("x = %d\n", x);
    }

    if (is_locked) {
        pthread_mutex_unlock(&lock);
    }
}


int main(void)
{
    int i; // itr
    
    //A mutex is initilaized in the beggining of the main function.
    pthread_mutex_init(&lock, NULL);

    // Declare two threads
    int num_threads = 10;
    pthread_t t[num_threads];

    /* Initialize two threads: 
        1. pthread reference
        2. pthread attributes
        3. a function pointer
        4. a argument pointer
    */
    
    // NULL argument pointer means empty parameters to passed function
    for (i = 0; i < num_threads; i++) {
        pthread_create(&t[i], NULL, inc, NULL);
    } 

    // wait until both threads finish
    for (i = 0; i < num_threads; i++) {
        pthread_join(t[i], NULL);
    }

    //At the end of the main when both the threads are done, the mutext is destroyed.
    pthread_mutex_destroy(&lock);

  return 0;
}
