#include "fila.h"

int init_fila(struct fila* f){
  f->head = 0;
  f->end = 0;
  return 0;
}

int adiciona_elemento(struct fila* f, int i){
  f->est[f->end]=i;
  f->end=(f->end+1)%200;
  if (f->end == f->head) return -1;  //Estouro de fila!
  return 0;
}

int remove_elemento(struct fila* f){
  if (f->head == f->end) return -1; //Fila vazia!
  int rslt = f->est[f->head];
  f->head=(f->head+1)%200;
  return rslt;
}

int olhar_primeiro_fila (struct fila *f) {
  if (filavazia(f) == 1) {
    return -1;
  }
  return f->est[f->head];
}

int filavazia(struct fila *f){
  if (f->head == f->end) return 1; //Fila vazia!
  return 0;
}

int printfila(struct fila* f){
  int nhead = f->head;
  int nend = f->end;
  while(nhead!=nend){
    printf("%d,",f->est[nhead]);
    nhead=(nhead+1)%200;
  }
  printf("\n\n");
  return 0;
}