#include <stdio.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

atomic_flag lock = ATOMIC_FLAG_INIT;
int tam ;
int soma = 0;
int threadnums;
char *nums;
struct timeval t1,t2;

void acquire(){
  while(atomic_flag_test_and_set(&lock));
}

void release(){
  atomic_flag_clear(&lock);
}

int fim(int a, int b,int c,int index){
  if (index==b-1){return c;}
  return a+(c/b);
}	


void* somador(void *arg){
  int i=(int)arg; 
  int j;
  int parc = 0;
  int start = i*(tam/threadnums);
  for(j=start;j<fim(start,threadnums , tam,i);j++){
    parc=parc+(int)nums[j] ;
  }
  acquire();
  soma = soma+parc;
  release();
  return NULL;
}




int main(int argc, char *argv[]) {
  tam=atoi(argv[1]);
  threadnums=atoi(argv[2]);
  nums= malloc(tam);
  srand(time(NULL));
  pthread_t * threads = malloc(sizeof(pthread_t)*threadnums);
  int i;
  for (i=0;i<tam;i++){
    nums[i]=(char)(rand()%201 - 101); 
    //printf("%d\n", nums[i]); 
  }
  
  gettimeofday(&t1, NULL);
  for (i = 0; i < threadnums; i++) {
    int *p=i;
    int t = pthread_create(&threads[i], NULL,&somador, (void*)p);
    
  }
  
  for (i = 0; i < threadnums; i++) {

    pthread_join(threads[i], NULL);
    
  }
  gettimeofday(&t2, NULL);
  printf("%f\n", (double)((t2.tv_sec * 1000000 + t2.tv_usec) -
    (t1.tv_sec * 1000000 + t1.tv_usec))/1000000);

  



  return 0;
}
