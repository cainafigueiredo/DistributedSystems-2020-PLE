#include "./process.h"

// Defining variables
int my_id; 
int my_socket;
struct sockaddr_in coordinator_addr;
int coordinator_addrlen;
char message_buffer[BUFFER_SIZE];
char *file_path;

// Defining functions
// ===================

int init_process_state() {
	int return_status;

	// Initializing process identification
	return_status = init_process_id(&my_id);
	// If error
	if (return_status < 0) {
		return -1;
	}
	
	// Setting up the coordinator address structure
	return_status = init_coordinator_addr_struct();
	// If error
	if (return_status < 0) {
		return -1;
	}

	// If success
	return 0;
}

int init_process_id(int *id) {
	// This schema neve throw any error.
	*id = (int) getpid();
	return 0;
}

int init_tcp_socket(int *socket_fd) {
	// Creating the TCP socket
	*socket_fd = socket(PF_INET,SOCK_STREAM,0);

	// If error
	if (*socket_fd < 0) {
		printf("Error during consumer socket creation.\n");
		return -1;
	}

	// Setting socket to be reusable
	const int optVal = 1;
	const socklen_t optLen = sizeof(optVal);
	setsockopt(*socket_fd, SOL_SOCKET, SO_REUSEADDR, (void*) &optVal, optLen);
	
	// If success
	return 0;
}

int init_coordinator_addr_struct() {
	// Error verifications are not been doing here
	coordinator_addrlen = sizeof(coordinator_addr);
	coordinator_addr.sin_family = AF_INET;
	coordinator_addr.sin_port = htons(PORT);
	coordinator_addr.sin_addr.s_addr = inet_addr(COORDINATOR_IP);
	return 0;
}

int connect_to_coordinator(int *socket_fd) {
	int return_status;

	// Creating a socket
	return_status = init_tcp_socket(socket_fd);
	// If error
	if (return_status < 0) {
		return -1;
	}

	return_status = connect(*socket_fd, (struct sockaddr *) &coordinator_addr, (socklen_t) coordinator_addrlen);
	// If error
	if (return_status < 0) {
		return -1;
	}
}

int disconnect_from_coordinator(int *socket_fd) {
	int return_status;

	// Closing socket
	close(*socket_fd);
	// If error
	if (return_status < 0) {
		return -1;
	}
	// If success
	return 0;
}

int parse_message(int *message_id, int *message_process_id) {

	char *field;

	// Getting message_id field
	field = strtok(message_buffer,MESSAGE_SEPARATOR);
	// If error
	if (field == NULL) {
		return -1;
	}
	// Otherwise
	*message_id = atoi(field);

	// Getting message_process_id field
	field = strtok(NULL,MESSAGE_SEPARATOR);
	// If error
	if (field == NULL) {
		return -1;
	}
	// Otherwise
	*message_process_id = atoi(field);

	// If success
	return 0;
}

int request_critical_region_access() {
	int return_status;
	
	// Connecting to the coordinator
	return_status = connect_to_coordinator(&my_socket);
	// If errors
	if (return_status < 0) {
		return -1;
	}

	// Preparing REQUEST message

	sprintf(message_buffer,"%d%s%d%s", REQUEST_ID, MESSAGE_SEPARATOR, my_id, MESSAGE_SEPARATOR);
	
	// Completing message_buffer with zeros	

	for (int i = strlen(message_buffer); i < BUFFER_SIZE; i++) {
		message_buffer[i] = 48;
	}

	// Sending REQUEST message
	return_status = send(my_socket, message_buffer, sizeof(message_buffer), 0);
	// If error
	if (return_status < 0) {
		return -1;
	}

	// Waiting for a GRANT message. The process is blocked here

	return_status = wait_for_grant_message();
	
	// If errors
	if (return_status < 0) {
		return -1;
	}

	// If success, the process can access the critical region
	return 0;
}

int wait_for_grant_message() {
	
	int return_status;

	int message_id, message_process_id;
	
	// Wait for a message comes to my_socket
	do {
		return_status = recv(my_socket,message_buffer,sizeof(message_buffer),0);
		// If error
		if (return_status < 0) {
			return -1;
		}
		// Parsing the message 
		return_status = parse_message(&message_id, &message_process_id);
		// If error
		if (return_status < 0) {
			return -1;
		}
	} while (message_id != GRANT_ID || message_process_id != my_id);

	// If success, this function can return and the process can access the critical region
	return 0;
}

int release_critical_region_access() {
	int return_status;

	// In our solution, we supose that the process is conected with coordinator

	// Preparing RELEASE message

	sprintf(message_buffer,"%d%s%d%s", RELEASE_ID, MESSAGE_SEPARATOR, my_id, MESSAGE_SEPARATOR);
	
	// Completing message_buffer with zeros

	for (int i = strlen(message_buffer); i < BUFFER_SIZE; i++) {
		message_buffer[i] = 48;
	}

	// Sending RELEASE message

	return_status = send(my_socket, message_buffer, sizeof(message_buffer), 0);
	// If error
	if (return_status < 0) {
		return -1;
	}
	
	// Disconnecting from coordinator
	//disconnect_from_coordinator(&my_socket);

	// If success, the process can access the critical region
	return 0;
}

int process_behaviour(int r, char *file_path, float k) {
	int return_status;
	// Invalid Parameters
	if (r < 1 || k < 0) {
		return -1;
	}

	for (int i = 0; i < r; i++) {
		// Requesting critical region access. It will be block until the coordinator 
		// granted it.
		return_status = request_critical_region_access();
		// If error
		if (return_status < 0) {
			return -1;
		}

		printf("PID : %d -  I'm into the critical region now.\n",my_id);
		// Our critical region begins here
		write_id_and_time(file_path);
		sleep((unsigned int) k);		

		// Our critical region ends here. We can release the critical region for other
		// processes.
		return_status = release_critical_region_access();
		// If error
		if (return_status < 0) {
			return -1;
		}
	}
	return 0;
}

int write_id_and_time(char *file_path) {
	FILE *f = fopen(file_path,"a");

	// Getting time and formatting it.
	struct timeval curTime;
	gettimeofday(&curTime, NULL);
	int mili = curTime.tv_usec/1000;
	char time_buffer[80];
	strftime(time_buffer, 80, "%H:%M:%S", localtime(&curTime.tv_sec));

	char currentTime[84] = "";
	sprintf(currentTime, "%s:%03d", time_buffer, mili);

	// Writing in the file
	fprintf(f,"Current time -> %s | Process ID -> %d\n",
			   currentTime, my_id);

	// Closing the file
	fclose(f);
}

int main(int argc, char *argv[]) {
	int r = atoi(argv[1]);
	int k = atoi(argv[2]);
	char *file_path = argv[3];

	init_process_state();
	process_behaviour(r,file_path,k);

	return 0;
}