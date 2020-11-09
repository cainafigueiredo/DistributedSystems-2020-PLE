#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h> 
#include "./fila.c"

// Defining Macros
// ===============
#define PORT 4000
#define MESSAGE_SEPARATOR "|"
#define BUFFER_SIZE 10
#define REQUEST_ID 1
#define GRANT_ID 2
#define RELEASE_ID 3
#define MAX_PROCESSES 150
#define COORDINATOR_INFO_ON_STARTS "The coordinator is running.\n\nInterface commands:\n1 - Show process in request queue.\n2 - Show number of GRANTS per process.\n3 - Exit.\n"

// Declaring Variables
// ===================

// Coordinator Address
struct sockaddr_in my_addr;
int my_addrlen;

// Message Queue
extern struct fila fila_pedidos_id_processos;

// Request Queue
extern struct fila fila_pedidos_sockets;

// Pending sockets queue. These sockets are waiting to be associated with a thread
struct fila fila_sockets_pendentes;

// Grant Counter
extern int processos[MAX_PROCESSES];
extern int grants[MAX_PROCESSES];

// Semaphores
extern sem_t p;
extern sem_t g;
extern sem_t fp;
extern sem_t fm;
extern sem_t sockets_pendentes_semaphore;

// Interface variables
extern int comando;

// Declaring Functions
// ===================

// All the structures are initialized here, such that semaphores, message queue, request queue,
// and the hash table that will be used for GRANT counts. So, it initializes the three main 
// threads, that is, the listener thread, the interface thread and the thread running centered
// algorithm.
extern int init_coordinator();

// It creates a TCP socket and store it in the socket passed by reference. It returns
// 0 if success or -1 otherwise.
extern int init_tcp_socket(int *socket_fd); 

// It initializes the coodinator address for further connection. It receiveis a sockaddr_in 
// pointer and returns 0 if success or -1, otherwise.
extern int init_coordinator_addr_struct();

// This initializes a thread in a infinity loop. The following behaviour will be seen 
// by this thread: 
// Out of the loop, a new TCP socket will be created and the coordinator addr structure will
// be initialized. After this, into the loop, the thread receives a connection request. This 
// connection will be accepted and a new thread will be created, so that this new thread will 
// execute the dedicated_comumunication function. After this, the loop continues.
extern void * connection_listener();

// It will be used by many different threads in order to keep a dedicated communication with 
// each process that are communicating with coordinator. These threads blocks until receive
// some message from the processes, which can be a REQUEST or RELEASE message. By receive it,
// it will put the message in a shared memory (request_queue) such that the thread executing 
// mutual exclusion algorithm can get it to further process. The loop continues until 
// a RELEASE message be sent by the process. 
extern void * dedicated_comumunication();

// It gets the content of message_buffer and it parses the 'id' and 'process_id' fields in
// this buffer, separated by MESSAGE_SEPARATOR. These values are stored in two integers passed
// by reference, called 'message_id' and 'message_process_id'. It returns 0 if success and -1, 
// otherwise.
extern int parse_message(char *message_buffer, int *message_id, int *message_process_id);

// It prepares a GRANT message to the process with id 'process_id' and sends it through 
// 'process_socket'. It returns 0 if success or -1, otherwise.
extern int grant_critical_region_access(char *message_buffer, int process_id, int process_socket);

// ?????????
void * inter(void *arg);

// ?????????
int initgrant(int *g);