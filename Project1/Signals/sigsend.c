#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {


	int rslt=kill(atoi(argv[1]),atoi(argv[2]));
	if (rslt == -1){
		printf("O processo não existe\n");}




}
