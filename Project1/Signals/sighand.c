#include <string.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

void handler (int sig){
	if (sig==35){
		write(STDOUT_FILENO, "Recebi o sinal 35\n",19);}

	if (sig==36){
		write(STDOUT_FILENO, "Recebi o sinal 36\n",19);}

	if (sig==37){
		write(STDOUT_FILENO, "Recebi o sinal 37\n",19);
		raise(9);
	}

}


int main(int argc, char *argv[]) {

	signal(35, handler);
	signal(36, handler);
	signal(37, handler);

	sigset_t set;
	sigemptyset(&set);

	if (strcmp(argv[1],"0") == 0){
		while(1){
			printf("O programa está rodando\n");
			sleep(1);
		}
	}
	else {
		while(1){
			printf("O programa está rodando\n");
			sleep(1);
			sigsuspend(&set);
		}
	}

}
