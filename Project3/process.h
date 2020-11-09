#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>

// Defining Macros
// ===============
#define COORDINATOR_IP "127.0.0.1"
#define PORT 4000
#define MESSAGE_SEPARATOR "|"
#define BUFFER_SIZE 10
#define REQUEST_ID 1
#define GRANT_ID 2
#define RELEASE_ID 3

// Declaring variables
// ===================

// Process identification
extern int my_id; 

// Socket that will be used in communication
extern int my_socket;

// Coordinator Address
extern struct sockaddr_in coordinator_addr;
extern int coordinator_addrlen;

// Message Buffer
extern char message_buffer[BUFFER_SIZE];

// Declaring functions
// ===================

// It calls init_process_id and after this it calls init_coordinator_addr_struct functions. 
// It returns 0 if success or -1 otherwise.
extern int init_process_state();

// It receives a pointer to an int and changes its value according some id attribution scheme.
// Here, this scheme will be simply the id attribution by getting PID.
extern int init_process_id(int *id); 

// It creates a TCP socket and store it in the socket passed by reference. It returns
// 0 if success or -1 otherwise.
extern int init_tcp_socket(int *socket_fd); 

// It initializes the coodinator address for further connection. It returns 0 if success 
// or -1, otherwise.
extern int init_coordinator_addr_struct();

// It establishes a TCP connection between this process and the coordinator. It should be
// preceded by init_coordinator_addr_struct function and it can be in init_process_state, for 
// example. Before connection, this function calls init_tcp_socket. It receives a pointer to a 
// socket and it returns 0 if the connection is successfully established or -1, otherwise.
extern int connect_to_coordinator(int *socket_fd);

// It disconnects this process from coordinator and closes the socket. It should receive a
// pointer to the socket that will be closed.
extern int disconnect_from_coordinator(int *socket_fd);

// It sends a REQUEST message to the coordinator. The message contains a message id and the  
// process id, specified by my_id variable. The size of the message is BUFFER_SIZE bytes.
// After sending this message, this process calls wait_for_grant_message function. It returns
// 0 if success and -1 otherwise.
extern int request_critical_region_access();

// It blocks this process and keep waiting for a grant message. When this message arrives, the
// process is unblocked and this function returns. It will return 0 if success or -1 otherwise.
extern int wait_for_grant_message();

// It sends a RELEASE message to the coordinator. The message contains a message id and the  
// process id, specified by my_id variable. The size of the message is BUFFER_SIZE bytes.
// After sending this message, this process calls disconnect_from_coordinator function and 
// continues its execution. It returns 0 if success or -1 otherwise
extern int release_critical_region_access();

// It implements the process behaviour according to the project specification. Here, r is
// the number of iterations before finishes, file_path is the path to 'resultados.txt' file
// and k is the time that the process should sleep before the next iteration. It returns 0 
// if success and -1 otherwise.
extern int process_behaviour(int r, char *file_path, float k);

// It opens the file in file_path in append mode, gets the current time with precision in
// miliseconds and writes the process id and this time in it. After this, this function 
// closes the file. It returns 0 if success or -1 otherwise.
extern int write_id_and_time(char *file_path);

// It gets the content of message_buffer and it parses the 'id' and 'process_id' fields in
// this buffer, separated by MESSAGE_SEPARATOR. These values are stored in two integers passed
// by reference, called 'message_id' and 'message_process_id'. It returns 0 if success and -1, 
// otherwise.
extern int parse_message(int *message_id, int *message_process_id);