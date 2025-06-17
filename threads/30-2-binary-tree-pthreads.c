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
int MAX_WORKERS = 16;

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

int main(){
  struct t_node *tree = create_empty_node(0);
  non_safe_three = create_empty_node(0);
  int* shuffled_array = create_shuffled_array(size);
  for(int i = 0; i<size; i++){
    insert(&tree, shuffled_array[i]);
  }

  pthread_t* workers[MAX_WORKERS];

  for(int i=0; i<MAX_WORKERS; i++){
    pthread_t *worker;
    pthread_create(worker, NULL, insert_into_non_safe, shuffled_array);
    workers[i] = worker;
  }

  for(int i=0; i<MAX_WORKERS; i++){
    pthread_join(*workers[i], NULL);
  }

  int value = shuffled_array[100];
  printf("%d\n", value);
  find_and_print_value((struct t_node**)&non_safe_three, value);

  return 0;
}
