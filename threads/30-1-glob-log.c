#include "pthread.h"
#include <stdlib.h>
#include <stdio.h>
static volatile int global = 0;

static void *thread_fn(void* args){
  int loops = *((int *) args);
  int local;
  for(int i=0; i<loops; i++){
    local = global;
    local++;
    global = local;
  }
}
int main(){
  pthread_t f1, f2;
  int loops = 100000;
  int code = pthread_create(&f1, NULL, thread_fn, &loops);
  if (code != 0) {
    exit(EXIT_FAILURE);
  }
  code = pthread_create(&f2, NULL, thread_fn, &loops);
  if (code != 0) {
    exit(EXIT_FAILURE);
  }
  code = pthread_join(f1, NULL);
  code = pthread_join(f2, NULL);
  printf("%d\n", global);
  exit(EXIT_SUCCESS);
}
