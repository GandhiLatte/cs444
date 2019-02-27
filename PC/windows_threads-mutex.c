#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h> // this contains definitions for both locks and mutex

#define NUM_THREADS 2

int x = 0;

HANDLE ghMutex; // declare windows mutex

void *debug(void *args) // function pointer
{
    int i;

    // note: try commenting out the lock!
    for (i=0; i < 20; i++) {
        // aquire windows lock
        // you also use waits for aquiring locks!
        WaitForSingleObject(ghMutex, TRUE, INFINITE);

        // critical section
        x = x + 1;
        printf("x = %d\n", x);
        
        // release windows lock
        // always be sure to release the lock when done!
        ReleaseMutex(ghMutex); 
    }
}

int main(void)
{  
    int i; // iterator

    HANDLE t[NUM_THREADS]; // HANDLE instead of pthread_t

    // initialize windows lock
    // false indicates initially not owned
    /* Parameters:
       1. lpMutexAttributes
       2. bInitalOwner
       3. lpName
    */
    ghMutex = CreateMutex(NULL, FALSE, NULL);

    // CreateThread(...) instead of pthread_create(...)
    /* Parameters:
        1. lpThreadAttributes
        2. dwStackSize
        3. lpStartAddress
        4. lpParameter
        5. dwCreationFlags
        6. lpThreadId
    */

    for (i=0; i < NUM_THREADS; i++) {
        // you need to cast worker function to LPTHREAD_START_ROUTINE
        t[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) debug, NULL, 0, NULL);
    }

    // WaitForMultipleObjects instead of pthread_join
    WaitForMultipleObjects(NUM_THREADS, t, TRUE, INFINITE);

    // close all thread handles
    for (i=0; i < NUM_THREADS; i++) {
        CloseHandle(t[i]); // you don't do this in linux!
    }

    // go to https://docs.microsoft.com/en-us/windows/desktop/sync/using-semaphore-objects
    // for an example on how to use Semaphores on Windows

    system("pause"); // WaitForsingleObject will not keep the program alive!

    return 0;
}