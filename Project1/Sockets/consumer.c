#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

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

int main() {

	int consumer_socket;
	struct sockaddr_in producer_addr;
	char *producer_host = "127.0.0.1";
	int producer_port = 4000;
	int producer_addrlen = sizeof(producer_addr);

	// Create the producer socket;
	consumer_socket = socket(PF_INET,SOCK_STREAM,0);
	if (consumer_socket < 0) {
		printf("Error during consumer socket creation.\n");
		return 1;
	}

	// Setting producer address to further connection
	producer_addr.sin_family = AF_INET;
	producer_addr.sin_port = htons(producer_port);
	producer_addr.sin_addr.s_addr = inet_addr(producer_host);

	if (connect(consumer_socket, (struct sockaddr *) &producer_addr, (socklen_t) producer_addrlen) < 0) {
		printf("Error during connection.\n");
		return 2;
	}

	// Begin to receive data
	char buffer[20];
	int received_number;
	do {
		// Receive number from producer process
		recv(consumer_socket,buffer,sizeof(buffer),0);
		received_number = atoi(buffer);
		// Sending response to producer process
		if (received_number != 0) {
			sprintf(buffer, "%d", is_prime(received_number));
			send(consumer_socket, buffer, sizeof(buffer), 0);
		}
	} while (received_number != 0);

	close(consumer_socket);

	return 0;
}
