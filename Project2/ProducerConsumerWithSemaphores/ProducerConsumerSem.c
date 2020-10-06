#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <math.h>
#include <sys/time.h>

typedef struct {
	int *array;
	int firstFilledIndex;
	int firstEmptyIndex;
	int length;
} myArray;

myArray shared_memory;

sem_t empty;
sem_t full;
sem_t mutex;

int remainsToConsume;

int generate_random_int(int start, int stop) {
	/* This function returns a random int in the interval [start,stop). */
	int random_value = start + rand()%(stop - start);
	return random_value;
}

int is_prime(int number) {
	/* Given a number, it returns 1 if this number is prime and 0 otherwise. */

	// The number 1 is prime.
	if (number == 1) {
		return 1;
	}

	int floorSqrtNumber = (int) floor(sqrt((double) number));

	for (int i = 2; i <= floorSqrtNumber; i++) {
		if ((number % i) == 0){
			// The number isn't prime.
			return 0;
		}
	}

	// The number is prime.
	return 1;
}

void *producer_thread_func(void *args) {
	/* This function implements the producer behaviour, that is, it produces a random int between 1 and pow(10,7) and stores it in the shared memory. It will end when the main program ends. */
	int produced;
	while (1) {
		produced = generate_random_int(1, pow(10,7) + 1);
		sem_wait(&empty);
		sem_wait(&mutex);
		shared_memory.array[shared_memory.firstEmptyIndex] = produced;
		shared_memory.firstEmptyIndex = (shared_memory.firstEmptyIndex + 1)%shared_memory.length;
		sem_post(&mutex);
		sem_post(&full);
	}
}

void *consumer_thread_func(void *args) {
	/* This function implements the consumer behaviour, that is, it consumes an item from shared memory and print in the terminal the primality result of the consumed number until M numbers are consumed. */
	int received;
	int finished = 0;
	while (1) {
		sem_wait(&full);
		sem_wait(&mutex);
		if (remainsToConsume > 0) {
			received = shared_memory.array[shared_memory.firstFilledIndex];
			shared_memory.firstFilledIndex = (shared_memory.firstFilledIndex + 1)%shared_memory.length;
			remainsToConsume -= 1;
		} else {
			finished = 1;
		}
		sem_post(&mutex);
		sem_post(&empty);
		
		if (finished == 1) break;

		if (is_prime(received) == 1) {
			printf("The number %d is prime.\n",received);
		} else {
			printf("The number %d isn't prime.\n",received);
		}
	}
}

int main(int argc, char *argv[]) {
	/* 
	Arguments:
	---------
	All the arguments are required.
	- int np: Number of producer threads;
	- int nc: Number of consumer threads;
	- int n: Capacity of the shared memory;
	- int m: The total of numbers that the consumer should process before stop the program execution;
	*/
	
	// Variables that will be used to get the execution time
	struct timeval start, end;

	// Parsing arguments

	int Np, Nc, N;

	for (int i = 1; i < argc; i += 2) {
		if (strcmp(argv[i],"-np") == 0) {
			Np = atoi(argv[i+1]);
		} else 
		if (strcmp(argv[i],"-nc") == 0) {
			Nc = atoi(argv[i+1]);
		} else
		if (strcmp(argv[i],"-n") == 0) {
			N = atoi(argv[i+1]);
		} else
		if (strcmp(argv[i],"-m") == 0) {
			remainsToConsume = atoi(argv[i+1]);
		}
	}

	// Initializing shared memory
	int *array = calloc((size_t) N, sizeof(int));
	shared_memory.array = array;
	shared_memory.firstFilledIndex = 0;
	shared_memory.firstEmptyIndex = 0;
	shared_memory.length = N;

	// Initializing Semaphores
	if (sem_init(&empty, 0, N) || sem_init(&full, 0, 0) || sem_init(&mutex, 0, 1)) {
		printf("Error during semaphore initialization.\n");
	}

	// Setting random seed
	srand(time(NULL));

	gettimeofday(&start, NULL);

	// Creating producer threads
	pthread_t *producer_tids = calloc((size_t) Np, sizeof(pthread_t));
	for (int t = 0; t < Np; t++) {
		pthread_create(&producer_tids[t],NULL,producer_thread_func,NULL);
	}

	// Creating consumer threads
	pthread_t *consumer_tids = calloc((size_t) Nc, sizeof(pthread_t));
	for (int t = 0; t < Nc; t++) {
		pthread_create(&consumer_tids[t],NULL,consumer_thread_func,NULL);
	}

	// Waiting for all consumer threads finish before exit the program
	for (int t = 0; t < Nc; t++) {
		pthread_join(consumer_tids[t], NULL);
	}	

	gettimeofday(&end, NULL);

	// Creating a file to store the elapsed time
	FILE *out = fopen("./elapsedTime.out","w");
	fprintf(out,"%f",(double) ((end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec))/1000000);
	fclose(out);

	// Destroying semaphores
	sem_destroy(&empty);
	sem_destroy(&full);
	sem_destroy(&mutex);

	// Memory deallocation
	free(array);
	free(producer_tids);
	free(consumer_tids);

	return 0;
}
