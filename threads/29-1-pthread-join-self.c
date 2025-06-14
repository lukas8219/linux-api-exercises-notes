#include "pthread.h"
#include "stdio.h"
int join_with_manual_prevention(pthread_t tid_target){
  pthread_t c_tid = pthread_self();
  if (c_tid == tid_target) {
    return 35; //do we have ERRORNO
  }
  return pthread_join(tid_target, NULL);
}
int main(){
  int code = pthread_join(pthread_self(), NULL);
  printf("Pthread_Join API %d\n", code);
  int other_code = join_with_manual_prevention(pthread_self());
  printf("Manual %d\n", other_code);
  return 0;
}
