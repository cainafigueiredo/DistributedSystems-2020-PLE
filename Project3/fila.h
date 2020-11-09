#include <stdio.h>

#define TAMANHO_FILA 200

struct fila {
  int est[TAMANHO_FILA];
  int head;
  int end;
};

extern int init_fila(struct fila *f);

extern int adiciona_elemento(struct fila *f, int i);

extern int remove_elemento(struct fila *f);

extern int olhar_primeiro_fila(struct fila *f);

extern int filavazia(struct fila *f);

extern int printfila(struct fila *f);