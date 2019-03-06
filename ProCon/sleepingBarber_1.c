#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#include <pthread.h>
#include <semaphore.h>

int NUM_BARBERS	= 3;
int NUM_CHAIRS = 10;
int NUM_CUSTOMERS = 25;
int CUSTOMER_MAX_INTERVAL = 2;
int HAIRCUT_TIME = 4;

extern int errno;

int chair_availability;

pthread_mutex_t count_lock;
sem_t barber_sem;
sem_t barber_chair;

void* get_haircut(void *args){

	int *arg;
	int id;

	arg = (int*) args;
	id = *arg;

	sem_wait(&barber_chair);
	printf("Seating: %i \n", id);
	sem_wait(&barber_sem);

	printf("\t\t\tCutting Hair for Customer %i \n", id);
	usleep ( HAIRCUT_TIME * 1000000); 
	printf("\t\t\t\t\t\t Finished Cutting %i hair \n", id);

	sem_post(&barber_sem);
	sem_post(&barber_chair);

	/* Actual logic:
	 * - Check (and wait) for an available barber
	 * - Once a chair is occupied, get a haircut
	 * - Make sure to get out of the chair when we're done!
	 *
	 * Remember to be careful and protect your concurrent accesses--but
	 * don't hold any needless mutexes, or you might risk deadlock!
	 *
	 * Bonus: do statistics to see e.g. average wait time, average
	 * queued customers at any point, etc.
	 */

	return 0;
}


int main(int argc, char **argv)
{
	int iteration;
	int sleep_time;
	int result;
	pthread_t threads[NUM_CUSTOMERS];

	if (argc > 1){
		NUM_BARBERS = atoi(argv[1]);
		if (NUM_BARBERS < 1){
			printf("There must be at least one barber\n");
			return(1);
		}
	}
	if (argc > 2){
		NUM_CHAIRS = atoi(argv[2]);
		if (NUM_CHAIRS < 1){
			printf("There must be at least one chair\n");
			return(1);
		}
	}
	if (argc > 3){
		NUM_CUSTOMERS = atoi(argv[3]);
		if (NUM_CUSTOMERS < 1){
			printf("There must be at least one customer\n");
			return(1);
		}
	}
	if (argc > 4){
		CUSTOMER_MAX_INTERVAL = atoi(argv[4]);
		if (CUSTOMER_MAX_INTERVAL < 0){
			printf("The customers cannot come in negative time\n");
			return(1);
		}
	}
	if (argc > 5){
		HAIRCUT_TIME = atoi(argv[5]);
		if (HAIRCUT_TIME < 1){
			printf("Haircuts must take at least one second\n");
			return(1);
		}
	}

	printf("Welcome to the barbershop problem!\n");
	printf("For this run, we have:\n");
	printf("%i Barbers\n", NUM_BARBERS);
	printf("%i Chairs\n", NUM_CHAIRS);
	printf("%i customers will come in with delay from 0 to %i between them.\n", NUM_CUSTOMERS, CUSTOMER_MAX_INTERVAL);
	printf("A haircut takes %i seconds.\n\n", HAIRCUT_TIME);

	/* Initialize variables--counts, locks, and semaphors */
	pthread_mutex_init(&count_lock, NULL);
	sem_init(&barber_sem, 0, NUM_BARBERS);
	sem_init(&barber_chair, 0, NUM_CHAIRS);

	printf("Beginning Simulation.\n\n");
	//srand((unsigned int) time(NULL));
	for(iteration=0; iteration<NUM_CUSTOMERS; iteration++){
		/* Create a customer thread */
		pthread_create(&threads[iteration], NULL, get_haircut, &iteration);
		printf("Customer %i walked in the door\n", iteration);
		//pthread_detach(threads[iteration]);  /* Indicate that the thread shouldn't hold resources */
		/* Wait for a random amount of time */
		usleep((rand()%CUSTOMER_MAX_INTERVAL) * 1000000);
	}

	/* Check (and wait) for any remaining customers */
    for(iteration=0; iteration<NUM_CUSTOMERS; iteration++){
        pthread_join(threads[iteration], NULL);
    }

	/* Clean up */
	sem_destroy(&barber_chair);
	sem_destroy(&barber_sem);
	pthread_mutex_destroy(&count_lock);
	printf("\nBarbershop Problem Completed\n");
	return(0);
}

