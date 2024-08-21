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
  int possible_size;
  int w; // games played and won --> important for the search algo
  int n; // games played in total
  int children;
  int cap;
} MC_node;


void init_node(MC_node*);
void free_node(MC_node*);
MC_node* play_move_from_node(MC_node*,int,int);
MC_node* play_child(MC_node*,int);

#endif //MCTS_H
