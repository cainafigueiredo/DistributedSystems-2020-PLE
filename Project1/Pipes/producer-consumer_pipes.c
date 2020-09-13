#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

void add_random_delta(int *number, int intervalBegin, int intervalEnd) {
	/* It updates 'number' parameter by adding a random delta between 'intervalBegin' 
	(inclusive) and 'intervalEnd' (exclusive). */

	*number += intervalBegin + (rand() % (intervalEnd-intervalBegin));
}

int is_prime(int number) {
	/* Given a number, it returns 1 if this number is prime and 0 otherwise. */

	// The number 1 is prime.
	if (number == 1) {
		return 1;
	}

	int half = number/2;

	for (int i = 2; i <= half; i++) {
		if ((number % i) == 0){
			// The number isn't prime.
			return 0;
		}
	}

	// The number is prime.
	return 1;
}

int main(int argc, char *argv[]) {

	int N = atoi(argv[1]);

	int pid;
	int fd[2];

	/* Create a pipe */
	if (pipe(fd) == -1) {
		printf("Error during pipe creation.\n\n");
		return 1;
	}

	/* Creating a new process */
	pid = fork();
	if (pid == -1) {
		printf("Error during fork.\n\n");
		return 2;
	}
	else if (pid == 0) {
		/* Code of the child process */
		
		// Child process doesn't write. Closing write end of the pipe.
		close(fd[1]);

		// Receiving numbers and telling prime status until receive a zero.
		int received;
		do {
			read(fd[0],&received,sizeof(received));
			//printf("Receiving a number...\nThe received number is %d.\n\n",received);
			if (received != 0) {
				if (is_prime(received) == 1) {
					printf("The number %d is prime.\n\n",received);
				} 
				else {
					printf("The number %d isn't prime.\n\n",received);
				}
			}
		}
		while (received != 0);

		//printf("I have received the number 0. Closing child process.\n\n");
		close(fd[0]);
		return 3;
	}
	else {
		/* Code of the parent process */

		// Parent process doesn't read. Closing read end of the pipe.
		close(fd[0]);

		// Generating N increasing random numbers.
		srand(time(NULL));
		int number = 1;
		for (int i = 1; i <= N; i++) {
			add_random_delta(&number,1,101);
			//printf("Generating a number for %dth iteration...\nThe generated number is: %d.\n\n",i,number);
			write(fd[1],&number,sizeof(number));
		}
		
		// Sending a zero to finish child process and waiting for child.
		//printf("All numbers were generated. Telling to the child to finish.\n\n");
		number = 0;
		write(fd[1],&number,sizeof(number));

		close(fd[1]);
		wait(NULL);
		return 4;
	}

	return 0;
}
