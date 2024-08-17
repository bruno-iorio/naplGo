#include <stdio.h>
#include <stdlib.h>
#include "../Mechanics/board.h"
#define INIT_CAP  20


typedef struct MC_node{
  struct MC_node *next;
  board* Board;
  grid *copy1,*copy2;
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
void free_node(MC_node*);

void init_node(MC_node* node){
  node->children = 0;
  node->capacity = INIT_CAP;
  node->leaf = 1;
  node->root = 1;
  node->w = 0;
  node->n = 0;
  node->next = (MC_node*) malloc(sizeof(MC_node)*(node->capacity));
  node->Board = create_board();
  node->copy1 = (grid *) malloc(sizeof(grid));
  node->copy2 = (grid *) malloc(sizeof(grid));

  (node->copy2)->tab = (int **) malloc(sizeof(int*)*BOARDSIZE);
  (node->copy2)->tab = (int **) malloc(sizeof(int*)*BOARDSIZE);
  for(int i = 0; i < BOARDSIZE; i++){
    ((node->copy2)->tab)[i] = (int *) malloc(sizeof(int)*BOARDSIZE);
    ((node->copy2)->tab)[i] = (int *) malloc(sizeof(int)*BOARDSIZE);
  }
}


void free_node(MC_node* node){
  free(node->Board);
  free(node->next);
  for(int i = 0; i < BOARDSIZE; i++){
    free((node->copy1)->tab[i]);
    free((node->copy2)->tab[i]);
  }
  free(node->copy1);
  free(node->copy2);
  free(node);
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
