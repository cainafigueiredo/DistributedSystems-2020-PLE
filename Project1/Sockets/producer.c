#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>

void add_random_delta(int *number, int intervalBegin, int intervalEnd) {
	/* It updates 'number' parameter by adding a random delta between 'intervalBegin' 
	(inclusive) and 'intervalEnd' (exclusive). */

	*number += intervalBegin + (rand() % (intervalEnd-intervalBegin));
}

int main(int argc, char *argv[]) {

	int producer_socket;
	struct sockaddr_in producer_addr;
	int producer_port = 4000;
	int producer_addrlen = sizeof(producer_addr);
	int consumer_socket;
	int N = atoi(argv[1]); // The number of random generations.

	// Create the producer socket;
	producer_socket = socket(PF_INET,SOCK_STREAM,0);
	if (producer_socket < 0) {
		printf("Error during producer socket creation.\n");
		return 1;
	}

	// Setting socket to be reusable
	const int optVal = 1;
	const socklen_t optLen = sizeof(optVal);
	setsockopt(producer_socket, SOL_SOCKET, SO_REUSEADDR, (void*) &optVal, optLen);

	// Setting producer address to further connection
	producer_addr.sin_family = AF_INET;
	producer_addr.sin_port = htons(producer_port);
	producer_addr.sin_addr.s_addr = INADDR_ANY;

	// Binding producer socket
	if (bind(producer_socket, (struct sockaddr *) &producer_addr, (socklen_t) producer_addrlen) < 0) {
		printf("Error during bind.\n");
		return 2;
	}

	// Start to listening. We only have one consumer
	if (listen(producer_socket, 1) < 0) {
		printf("Error during listening.\n");
		return 3;
	}

	// Accepting consumer connection request
	consumer_socket = accept(producer_socket, (struct sockaddr *) &producer_addr, (socklen_t *) &producer_addrlen);
	if (consumer_socket < 0) {
		printf("Error during accept.\n");
		return 4;
	}

	// Beginning production
	srand(time(NULL));
	int number = 1;
	char buffer[20];
	int isprime;
	for (int i = 1; i <= N; i++) {
		add_random_delta(&number,1,101);
		sprintf(buffer, "%d", number);
		send(consumer_socket, buffer, sizeof(buffer), 0);
		// Receive the response from consumer
		recv(consumer_socket, buffer, sizeof(buffer), 0);
		isprime = atoi(buffer);
		if (isprime == 1) {
			printf("The number %d is prime.\n\n",number);
		} 
		else {
			printf("The number %d isn't prime.\n\n",number);
		}
	}
	// Sending zero to finish consumer process
	sprintf(buffer, "%d", 0);
	send(consumer_socket, buffer, sizeof(buffer), 0);

	// Closing sockets
	close(producer_socket);
	close(consumer_socket);
	
	return 0;
}
