#include <stdio.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <pthread.h>

atomic_flag lock = ATOMIC_FLAG_INIT;
int tam = 10;
int soma = 0;
int threadnums=2;

void acquire(){
  while(atomic_flag_test_and_set(&lock));
}

void release(){
  atomic_flag_clear(&lock);
}

void* somador(void *arg){
  char *arr=(char*)arg; 
  acquire();
  int i;
  for (i=0; i<tam;i++){
    soma=soma+(int)arr[i] ;
  }
  release();
  return NULL;
}




int main(void) {
  char nums[tam];
  srand(time(NULL));
  pthread_t * threads = malloc(sizeof(pthread_t)*threadnums);
  int i;
  for (i=0;i<10;i++){
    nums[i]=(char)(rand()%201 - 101); 
    printf("%d\n", nums[i]); 
  }
  somador(nums);
  printf("soma1: %d\n", soma); 

  for (i = 0; i < threadnums; i++) {

    int t = pthread_create(&threads[i], NULL,&somador, nums);
    
  }
  
  for (i = 0; i < threadnums; i++) {

    pthread_join(threads[i], NULL);
    
  }
  printf("%d\n", soma); 

  



  return 0;
}
