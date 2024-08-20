#include <stdio.h>
#include <stdlib.h>
#include "mcts.h"
#include "../Mechanics/board.h"
#define CAP 10

MC_node** PATH;

void init_node(MC_node* node){
  node->children = 0;
  node->w = 0;
  node->n = 0;
  node->possible_size = 0;
  node->next = (MC_node**) malloc(sizeof(MC_node*)*361);
  node->possiblex = (int *) malloc(sizeof(int)*361);
  node->possibley = (int *) malloc(sizeof(int)*361);
  
  board* Board = create_board(); 
  node->Board = *Board;
  free(Board);

  node->turn = BLACK;
  node->pass_count = 0;
  node->count = 0;
  node->Ko_check = 0;
  (node->copy1)->tab = (int **) malloc(sizeof(int*)*BOARDSIZE);
  (node->copy2)->tab = (int **) malloc(sizeof(int*)*BOARDSIZE);
  for(int i = 0; i < BOARDSIZE; i++){
    ((node->copy1)->tab)[i] = (int *) malloc(sizeof(int)*BOARDSIZE);
    ((node->copy2)->tab)[i] = (int *) malloc(sizeof(int)*BOARDSIZE);
  }
  copy_board((node->copy1).tab,(node->Board).grid,BOARDSIZE);
  copy_board((node->copy2).tab,(node->Board).grid,BOARDSIZE);
  list_possible_moves(node->possiblex,node->possibley,
                      &(node->possible_size), &(node->Board),
                      &(node->copy1),&(node->copy2),
                      node->turn,node->count,
                      node->Ko_check);
}


void free_node(MC_node* node){
  free(node->next);
  for(int i = 0; i < BOARDSIZE; i++){
    free(((node->copy1)->tab)[i]);
    free(((node->copy2)->tab)[i]);
  }

  free((node->copy1)->tab);
  free((node->copy2)->tab);
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

MC_node* play_move_from_node(MC_node *node,int x, int y){
  MC_node* other = (MC_node *)malloc(sizeof(MC_node));
  memcpy(other,node,sizeof(MC_node)); 
  
  play_move_out_of_match(&(other->Board),&(other->copy1),
                         &(other->copy2),&(other->turn),
                         &(other->pass_count), &(other->count),
                         x,y,&(other->Ko_check));
  other->possible_size = 0;
  other->children = 0;
  list_possible_moves(other->possiblex,other->possibley,
                      &(other->possible_size), &(other->Board),
                      &(other->copy1),&(other->copy2),
                      other->turn,other->count,other->Ko_check);
  return other;
}

MC_node* play_child(MC_node* node,int child){

  if(child >= node->possible_size){
    return NULL;
  }
  if (child < node->children){
    return MC_node->next[child];
  }
  else{
    for(int i = node->children; i < child; i++){
      MC_node* other = play_move_from_node(node,positionx[i],positiony[i]);
      node->next[i] = other;
    }
    return other;
  }
}


