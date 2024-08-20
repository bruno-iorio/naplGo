#ifndef MCTS_H
#define MCTS_H
#include "../Mechanics/board.h"
typedef struct MC_node{

  struct MC_node **next;
  int turn;
  int pass_count;
  int count; 
  int Ko_check;
  board Board;
  grid copy1;
  grid copy2;
  int* possiblex;
  int* possibley;
  int possible_size
  int w; // games played and won --> important for the search algo
  int n; // games played in total
  int children;
} MC_node;


void init_node(MC_node*);
void add_win(MC_node*);
void add_game(MC_node*);
int is_root(MC_node*);
int is_leaf(MC_node*);
void add_node(MC_node*,MC_node*);
void free_node(MC_node*);
#endif //MCTS_H
