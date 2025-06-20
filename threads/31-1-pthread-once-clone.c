/*
Implement a function that performs the equivalent to pthread_once(). The first arg should be a pointer to a static var containing a Bool + mutex.
Keep it simple and ignore `init()` failing or being cancelled.
*/

#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdatomic.h>

enum PTHREAD_ONCE_SM {
  NOT_STARTED = 1,
  IN_PROGRESS = 2,
  DONE = 3,
};

//This would be a Global?
typedef struct {
  atomic_int *state;
} PTHREAD_ONCE_CLONE;

void* once_clone(PTHREAD_ONCE_CLONE *once, void (*init_routine)(void)) {
  atomic_int curr_state = atomic_load(once->state);
  if (curr_state == DONE){
    return 0;
  }
  int expected = NOT_STARTED;
  if (atomic_compare_exchange_strong(once->state, &expected, IN_PROGRESS)) {
    init_routine();
    //EXPERIMENT channing to `memory_order_cst`
    atomic_store_explicit(once->state, DONE, memory_order_release);
  } else {
    //EXPERIMENT channing to `memory_order_cst`
    while(atomic_load_explicit(once->state, memory_order_acquire) == IN_PROGRESS){
      sched_yield();
    }
  };
  return 0;
}

void print_it(){
  printf("Yo!!! I am diff! I should appear only a single time!\n");
}

atomic_int initial_state = NOT_STARTED;
static PTHREAD_ONCE_CLONE ONCE = { .state = &initial_state };

void* print_some_value(void* arg){
  once_clone(&ONCE, print_it);
  return 0;
}

int main(){
  int amount_of_workers = 16;
  pthread_t threads[amount_of_workers];
  for(int i = 0; i < amount_of_workers; i++){
    pthread_t thread;
    pthread_create(&thread, NULL, print_some_value, NULL);
    threads[i] = thread;
  };

  for(int i = 0; i < amount_of_workers; i++) {
    pthread_join(threads[i], NULL);
  }
}
