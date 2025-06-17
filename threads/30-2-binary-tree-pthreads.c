#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdint.h>

int* create_shuffled_array(int size) {
  int* array = malloc(size * sizeof(int));

  for(int i = 0; i<size; i++){
    array[i] = i + 1;
  }

  srand(size);
  for (size_t i = size - 1; i > 0; i--) {
      size_t j = rand() % (i + 1);
      int tmp = array[i];
      array[i] = array[j];
      array[j] = tmp;
  }

  return array;
}

struct t_node {
  struct t_node *right;
  struct t_node *left;
  int value;
};

struct t_node* create_empty_node0(int value){
  struct t_node *node = malloc(sizeof(struct t_node));
  return node;
}

struct t_node* create_empty_node(int value){
  struct t_node *node = create_empty_node0(value);
  node->left = NULL;
  node->right = NULL;
  node->value = value;
  return node;
}

struct t_node** find_node_by_value(struct t_node **node, int value){
  while(*node != NULL){
    int node_value = (*node)->value;
    if (node_value == value){
      return node;
    }
    if (node_value > value) {
      node = &(*node)->left;
    } else {
      node = &(*node)->right;
    }
  }
  return node;
}

void insert(struct t_node **root, int value){
  struct t_node **node = find_node_by_value(root, value);
  if(*node == NULL){
    *node = create_empty_node(value);
  }
}

void find_and_print_value(struct t_node **tree, int value){
  struct t_node **found_node = find_node_by_value(tree, value);
  if (*found_node == NULL) {
    printf("Not found for value %d\n", value);
  } else {
    printf("%d\n", (*found_node)->value);
  }
}

int size = 100000;
struct t_node volatile *non_safe_three;
atomic_int volatile non_safe_counter = 0;

struct t_node volatile *safe_three;
static pthread_mutex_t three_mutex = PTHREAD_MUTEX_INITIALIZER;
atomic_int volatile safe_counter = 0;
struct t_node volatile *safe_three_mirror;
static pthread_mutex_t three_mutex_mirror = PTHREAD_MUTEX_INITIALIZER;
atomic_int volatile safe_counter_mirror = 0;
int MAX_WORKERS = 500;

void* insert_into_non_safe(void* args){
  int* shuffled_array = (int*) args;
  int counter = atomic_load(&non_safe_counter);
  while (counter < size) {
    int value = shuffled_array[counter];
    insert((struct t_node**)&non_safe_three, value);
    counter = atomic_fetch_add(&non_safe_counter, 1);
  }
  return 0;
}

void* insert_into_safe(void* args){
  int* shuffled_array = (int*) args;
  int counter = atomic_load(&safe_counter);
  while (counter < size) {
    int s = pthread_mutex_lock(&three_mutex);
    if (s != 0) {
      exit(1);
      return 0;
    }
    int value = shuffled_array[counter];
    insert((struct t_node**)&safe_three, value);
    s = pthread_mutex_unlock(&three_mutex);
    counter = atomic_fetch_add(&safe_counter, 1);
  }
  return 0;
}

void* insert_into_mirror(void* args){
  int* shuffled_array = (int*) args;
  int counter = atomic_load(&safe_counter_mirror);
  while (counter < size) {
    int s = pthread_mutex_lock(&three_mutex_mirror);
    if (s != 0) {
      exit(1);
      return 0;
    }
    int value = shuffled_array[counter];
    insert((struct t_node**)&safe_three_mirror, value);
    s = pthread_mutex_unlock(&three_mutex_mirror);
    counter = atomic_fetch_add(&safe_counter_mirror, 1);
  }
  return 0;
}

int* tree_to_array_bfs(struct t_node* tree, int* output, int size){
  int front = 0, rear = 0, count = 0;

  struct t_node *queue[size * 2];
  queue[rear++] = tree;
  while(front < rear && count < size) {
    struct t_node* curr = queue[front++];
    output[count++] = curr->value;
    if(curr->left != NULL){
      queue[rear++] = curr->left;
    }

    if(curr->right != NULL){
      queue[rear++] = curr->right;
    }
  }
  return output;
}

int compare_arrays(int* arr0, int* arr1, int size){
  for(int i = 0; i<size; i++){
    if (arr0[i] != arr1[i]) {
      printf("Index[%d] Value[%d] Value[%d]\n", i, arr0[i], arr1[i]);
      return 1;
    }
  }
  return 0;
}

int main(){
  struct t_node *tree = create_empty_node(0);
  non_safe_three = create_empty_node(0);
  safe_three = create_empty_node(0);

  int* shuffled_array = create_shuffled_array(size);
  for(int i = 0; i < size; i++){
    insert(&tree, shuffled_array[i]);
  }

  pthread_t workers_non_safe[MAX_WORKERS];

  for(int i=0; i < MAX_WORKERS; i++){
    pthread_t worker;
    pthread_create(&worker, NULL, insert_into_non_safe, shuffled_array);
    workers_non_safe[i] = worker;
  }

  for(int i=0; i<MAX_WORKERS; i++){
    pthread_join(workers_non_safe[i], NULL);
  }

  pthread_t workers_safe[MAX_WORKERS];
  for(int i=0; i < MAX_WORKERS; i++){
    pthread_t worker;
    pthread_create(&worker, NULL, insert_into_safe, shuffled_array);
    workers_safe[i] = worker;
  }

  for(int i=0; i<MAX_WORKERS; i++){
    pthread_join(workers_safe[i], NULL);
  }

  int resulting_array[size];
  int resulting_array_non_safe[size];
  tree_to_array_bfs((struct t_node*)safe_three, resulting_array, size);
  tree_to_array_bfs((struct t_node*)non_safe_three, resulting_array_non_safe, size);

  int code = compare_arrays(resulting_array, resulting_array_non_safe, size);

  if (code != 0) {
    printf("Safe and non-safe are not equal - expected\n");
  }

  pthread_t workers_safe_mirror[MAX_WORKERS];
  for(int i=0; i < MAX_WORKERS; i++){
    pthread_t worker;
    pthread_create(&worker, NULL, insert_into_mirror, shuffled_array);
    workers_safe_mirror[i] = worker;
  }

  for(int i=0; i<MAX_WORKERS; i++){
    pthread_join(workers_safe_mirror[i], NULL);
  }

  int resulting_array_safe_2[size];
  tree_to_array_bfs((struct t_node*)safe_three_mirror, resulting_array_safe_2, size);
  code = compare_arrays(resulting_array, resulting_array_safe_2, size);

  if (code != 0) {
    printf("Safe is not equal\n");
    return code;
  }

  return 0;
}
