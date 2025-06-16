#include <stdlib.h>
#include <stdio.h>
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

int main(){
  struct t_node *tree = create_empty_node(150);
  insert(&tree, 10);
  insert(&tree, 200);
  insert(&tree, 1);
  insert(&tree, 500);
  find_and_print_value(&tree, 150);
  find_and_print_value(&tree, 10);
  find_and_print_value(&tree, 200);
  find_and_print_value(&tree, 1);
  find_and_print_value(&tree, 500);
  return 0;
}
