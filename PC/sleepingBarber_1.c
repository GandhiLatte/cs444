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

void* get_haircut(void* args){

	int temp;
	int* arg;
	int id;

	arg = (int*) args;
	id = *arg;

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

	printf("Beginning Simulation.\n\n");
	srand((unsigned int) time(NULL));
	for(iteration=0; iteration<NUM_CUSTOMERS; ++iteration){
		/* Create a customer thread */
		pthread_detach(threads[iteration]);  /* Indicate that the thread shouldn't hold resources */

		/* Wait for a random amount of time */
	}

	/* Check (and wait) for any remaining customers */

	/* Clean up */
	printf("\nBarbershop Problem Completed\n");
	return(0);
}

