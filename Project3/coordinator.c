#include "./coordinator.h"

// Defining variables
// ==================

// Request Queue with the sockets file descriptors
struct fila fila_pedidos_sockets;

// Request Queue with the processes ids
struct fila fila_pedidos_id_processos;

// Pending sockets queue. These sockets are waiting to be associated with a thread
struct fila fila_sockets_pendentes;

// Grant Counter
int processos[MAX_PROCESSES]= {0};
int grants[MAX_PROCESSES]={0};

// Semaphores
sem_t p;
sem_t g;
sem_t fp;
sem_t fm;
sem_t sockets_pendentes_semaphore;

// Interface variables
int comando;

// Defining functions
// ==================

int index_finder(int *a, int value, int size){
  int i;
  for(i=0; i<size; i++) {
        if (a[i]==value) return i;
        if (a[i]==0) return -2-i;
  }
   return -1;
}

int init_coordinator() {
	int return_status;

	pthread_t listener_thread;
	pthread_t interf;

	// Initializing structures
	// =======================
	
	// Init queues
	init_fila(&fila_pedidos_sockets);
    init_fila(&fila_pedidos_id_processos);
	init_fila(&fila_sockets_pendentes);
	
	// Init semaphores
	sem_init(&p, 0, 1);
    sem_init(&g, 0, 1);
    sem_init(&fm, 0, 1);
    sem_init(&fp, 0, 1);
	sem_init(&sockets_pendentes_semaphore, 0, 1);

	// Creating the threads
	// ====================

	printf("%s\n",COORDINATOR_INFO_ON_STARTS);

	// For the interface thread
	return_status = pthread_create(&interf, NULL, inter, NULL);

	// For the listener thread
	return_status = pthread_create(&listener_thread,NULL,connection_listener,(void *) NULL);
	// If error 
	if (return_status != 0) {
		return -1;
	}

	// Waiting for threads finish
	pthread_join(interf, NULL);
	pthread_join(listener_thread, NULL);

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
	my_addrlen = sizeof(my_addr);
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(PORT);
	my_addr.sin_addr.s_addr = INADDR_ANY;;
	return 0;
}

void * connection_listener() {
	int return_status;

	int listener_socket, process_socket;
	pthread_t dedicated_thread;

	// Creating a socket
	return_status = init_tcp_socket(&listener_socket);
	// If error
	/*if (return_status < 0) {
		return -1;
	}*/

	// Setting up the coordinator address structure
	return_status = init_coordinator_addr_struct();
	// If error
	/*if (return_status < 0) {
		return -1;
	}*/

	// Binding listener_socket
	return_status = bind(listener_socket, (struct sockaddr *) &my_addr, (socklen_t) my_addrlen);
	// If error
	/*if (return_status < 0) {
		return -1;
	}*/

	// Start to listening
	return_status = listen(listener_socket, 1);
	// If error
	/*if (return_status < 0) {
		return -1;
	}*/

	// Beginning to accept connection requests into a loop
	while (1) {
		
		process_socket = accept(listener_socket, (struct sockaddr *) &my_addr, (socklen_t *) &my_addrlen);
		// If error
		/*if (process_socket < 0) {
			return -1;
		}*/
		
		sem_wait(&sockets_pendentes_semaphore);
		adiciona_elemento(&fila_sockets_pendentes,process_socket);
		sem_post(&sockets_pendentes_semaphore);

		// Creating a new thread to dedicated communication with the accept process
		pthread_create(&dedicated_thread,NULL,dedicated_comumunication,NULL);
	}

	//return 0;
}

void *dedicated_comumunication() {
	sem_wait(&sockets_pendentes_semaphore);
	int process_socket = remove_elemento(&fila_sockets_pendentes);
	sem_post(&sockets_pendentes_semaphore);

	// Creating its own message_buffer
	char message_buffer[BUFFER_SIZE];

	int return_status;

	int message_id, message_process_id;

	// REQUEST Message
	// ===============

	// Waiting for a REQUEST message
	do {
		return_status = recv(process_socket,message_buffer,sizeof(message_buffer),0);
		// If error
		if (return_status < 0) {
			pthread_exit((void *) NULL);
		}
		
		// Parsing message fields
		return_status = parse_message(message_buffer, &message_id, &message_process_id);
		// If error
		if (return_status < 0) {
			pthread_exit((void *) NULL);
		}

	} while (message_id != 1);

	//Trying to add process PID to processos array
	sem_wait(&p);
    int pos = index_finder(processos, message_process_id, MAX_PROCESSES);
    if (pos<-1){ 
		pos =-(pos+2);
        processos[pos]=message_process_id;
	}
    sem_post(&p);

	// Checking the request queue. If empty, send a GRANT message. In both case, add the 
	// process id in request queue
	sem_wait(&fp);
	if (filavazia(&fila_pedidos_id_processos) == 1) {
		grant_critical_region_access(message_buffer, message_process_id,process_socket);
        sem_wait(&g);
        grants[pos]+=1;
        sem_post(&g);		
	}
	adiciona_elemento(&fila_pedidos_id_processos, message_process_id);
	adiciona_elemento(&fila_pedidos_sockets, process_socket);
	sem_post(&fp);

	// Wait for a RELEASE message
	do {
		return_status = recv(process_socket,message_buffer,sizeof(message_buffer),0);
		if (return_status < 0) {
			pthread_exit((void *) NULL);
		}
		// Parsing message fields
		return_status = parse_message(message_buffer, &message_id, &message_process_id);
		if (return_status < 0) {
			pthread_exit((void *) NULL);
		}
	} while (message_id != RELEASE_ID);

	// Removing head of the queue and sending GRANT for the next process, if exists
	sem_wait(&fp);
	remove_elemento(&fila_pedidos_id_processos);
	remove_elemento(&fila_pedidos_sockets);
	int head_process_id = olhar_primeiro_fila(&fila_pedidos_id_processos);
	int head_socket = olhar_primeiro_fila(&fila_pedidos_sockets);
	if (filavazia(&fila_pedidos_id_processos) != 1) {
		grant_critical_region_access(message_buffer, head_process_id,head_socket);
        sem_wait(&g);
		sem_wait(&p);
		pos = index_finder(processos, head_process_id, MAX_PROCESSES);
		sem_post(&p);
        grants[pos]+=1;
        sem_post(&g);
	}
	sem_post(&fp);

	// Now, we can disconnect the socket and finish this thread
	close(process_socket);

	pthread_exit((void *) NULL);
}

int parse_message(char *message_buffer, int *message_id, int *message_process_id) {

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

int grant_critical_region_access(char *message_buffer, int process_id, int process_socket) {
	int return_status;

	// In our solution, we supose that the process is connected with coorditor already

	// Preparing GRANT message

	sprintf(message_buffer,"%d%s%d%s",GRANT_ID,MESSAGE_SEPARATOR,process_id, MESSAGE_SEPARATOR);
	
	// Completing message_buffer -with zeros	

	for (int i = strlen(message_buffer); i < BUFFER_SIZE; i++) {
		message_buffer[i] = 48;
	}

	// Sending GRANT message

	return_status = send(process_socket, message_buffer, sizeof(message_buffer), 0);
	// If error
	if (return_status < 0) {
		return -1;
	}

	// If success, the process can access the critical region
	return 0;
}

void * inter(void *arg) {
	int i;
	while(1){
		printf("Digite um comando: ");
		scanf("%d", &comando);
		printf("\n");

        if (comando == 1){
			printf("Process in request queue:\n");
			printf("========================:\n");
        	sem_wait(&fp);
        	printfila(&fila_pedidos_id_processos);
        	sem_post(&fp);
        }
        else if (comando == 2){
			printf("Process ID : GRANTS Number\n");
			printf("==========   =============\n");
        	for(i=0;i<MAX_PROCESSES;i++) {
        		sem_wait(&g);
            	sem_wait(&p);
				if (processos[i]==0) {
					sem_post(&p);
					sem_post(&g);
					break;
				}
				printf("%10d : %d\n", processos[i],grants[i]);
				sem_post(&p);
				sem_post(&g);
        	}
			printf("\n");
        }
        else if (comando == 3){
			printf("Finishing the execution\n");
    		sem_wait(&p);
        	for(i=0;i<MAX_PROCESSES;i++){
		if (processos[i]==0) break;
            	kill(processos[i],9);
            }
          	raise(9);
        	sem_post(&p);
        }
        comando = 0;
	}
	return NULL;
}

int main(int argc, char *argv[]) {
	init_coordinator();
	return 0;
}
