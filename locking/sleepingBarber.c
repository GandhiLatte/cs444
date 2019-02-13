#include<pthread.h>

struct customer_data {

};

struct barber_data {


};

void customer_thread(void* args) {
	customer_data* data = (customer_data*)args;
	// Check the availability of barbers
	if(trylock(empty_chair)) {
	// If some barber is available, ring the bell and goto that barber
		for(int i = 0 ; i < barber_count ; i++) {
			lock(barber[i].access);
			if(barber[i].is_available) {
				barber[i].next_customer = data;
				release(barbers[i].bell);
			}	
			unlock(barber[i].access);
			if(allocated)
				break;
		}
		
		unlock(empty_chair);
	} else {
	// If no barber is available, wait in the queue
		lock(queue_access);
		queue.add(data);
		unlock(queue_access);
		wait(data->lock);
	}
}

void barber_thread(void* args) {
  	barber_data* data = (barber_data*)args;
  	// Barber will keep working on incoming customers 
	while(true) {
		notify(empty_chair);
		// Wait on his bell
		wait(bell[index]);
	
	 	// Process self.next_customer	
		while(true) {	
			lock(queue_access);
			if(!queue.isEmpty())
				data = queue.pop();
			unlock(queue_access);
			process_data(data);
		}
	} 
}

int main(int argc, char** argv) {

	// Initialize locks
	

	// Create barbers
	

	// Randomly create some customers
	for(int i = 0 ; i < 100; i++) {

	}

	// Wait for all the customers to exit
	
}
