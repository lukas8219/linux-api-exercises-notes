#include "pthread.h"
#include <stdlib.h>
#include <stdio.h>
static volatile int global = 0;

static void *thread_fn(void* args){
  int loops = *((int *) args);
  int local;
  pthread_t tid = pthread_self();
  for(int i=0; i<loops; i++){
    local = global;
    local++;
    global = local;
    //printf("Tid[%ld] Global[%d]\n", tid, global);
    // Adding this ^ causes IO Buffering, more Syscalls etc - thus `masking` the race conditions
    // This is called Heisenburg effect
  }
  return 0;
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
  printf("Final value is Global[%d]", global);
  exit(EXIT_SUCCESS);
}
