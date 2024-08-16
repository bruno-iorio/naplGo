#include <stdio.h>
#include <stdlib.h>
#include "../Mechanics/board.h"
#define INIT_CAP  20


typedef struct MC_node{
  struct MC_node *next;
  int leaf;
  int root;
  int w; // games played and won --> important for the search algo
  int n; // games played in total
  int children;
  int capacity;
} MC_node;

void init_node(MC_node*);
void add_win(MC_node*);
void add_game(MC_node*);
int is_root(MC_node*);
int is_leaf(MC_node*);
void add_node(MC_node*,MC_node*);


void init_node(MC_node* node){
  node->children = 0;
  node->capacity = INIT_CAP;
  node->leaf = 1;
  node->root = 1;
  node->w = 0;
  node->n = 0;
  node->next = (MC_node*) malloc(sizeof(MC_node)*(node->capacity));
}

void add_win(MC_node* node){
  node->w++;
}

void add_game(MC_node* node){
  node->n++;
}

int is_root(MC_node* node){
  return node->root;
}

int is_leaf(MC_node* node){
  return node->leaf;
}

void add_node(MC_node* node, MC_node* other){
  if(node->children > node->capacity){
    node->capacity = (node->capacity)*2;
    node->next = (MC_node*) realloc(node->next,sizeof(MC_node)*(node->capacity));
  }
  (node->next)[node->children] = *other; 
  (node->children)++;
  node->leaf = 0;
  other->root = 0;
}


int main(){
  MC_node first;
  MC_node second;
  init_node(&first);
  init_node(&second);
  add_node(&first,&second);
  printf("%d %d %d %d\n",first.children,first.capacity,first.leaf,first.root);
  printf("%d %d %d %d\n",second.children,second.capacity,second.leaf,second.root);
  return 0;
}
