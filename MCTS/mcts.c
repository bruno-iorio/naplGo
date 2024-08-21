#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mcts.h"
#include "../Mechanics/board.h"
#define CAP 30

void init_node(MC_node* node){
  node->children = 0;
  node->w = 0;
  node->n = 0;
  node->possible_size = 0;
  node->cap = CAP;
  node->possiblex = (int *) malloc(sizeof(int)*(node->cap));
  node->possibley = (int *) malloc(sizeof(int)*(node->cap));
  node->next = (MC_node**) malloc(sizeof(MC_node*)*node->cap);
  
  board* Board = create_board(); 
  memcpy((void *)(&(node->Board)),(void *)Board,sizeof(Board));
  free(Board);

  node->turn = BLACK;
  node->pass_count = 0;
  node->count = 0;
  node->Ko_check = 0;
  (node->copy1).tab = (int **) malloc(sizeof(int*)*BOARDSIZE);
  (node->copy2).tab = (int **) malloc(sizeof(int*)*BOARDSIZE);
  
  for(int i = 0; i < BOARDSIZE; i++){
    ((node->copy1).tab)[i] = (int *) malloc(sizeof(int)*BOARDSIZE);
    ((node->copy2).tab)[i] = (int *) malloc(sizeof(int)*BOARDSIZE);
  }
  copy_board((node->copy1).tab,(node->Board).grid,BOARDSIZE);
  copy_board((node->copy2).tab,(node->Board).grid,BOARDSIZE);
  

  list_possible_moves(&(node->possiblex),&(node->possibley),
                      &(node->possible_size), &(node->Board),
                      &(node->copy1),&(node->copy2),
                      node->turn,node->count,
                      node->Ko_check,&(node->cap));
}


void free_node(MC_node* node){
  free(node->next);
  free(node->possiblex);
  free(node->possibley);
  for(int i = 0; i < BOARDSIZE; i++){
    
    free(((node->Board).grid)[i]);
    free(((node->copy1).tab)[i]);
    free(((node->copy2).tab)[i]);
  }
  free((node->copy1).tab);
  free((node->copy2).tab);
  free((node->Board).grid);
  free(node);
}


MC_node* play_move_from_node(MC_node *node,int x, int y){ //just to be used when the node has to be created
  MC_node* other = (MC_node *)malloc(sizeof(MC_node));

  (other->Board).grid = (int **) malloc(sizeof(int*)*BOARDSIZE);
  (other->copy1).tab = (int **) malloc(sizeof(int*)*BOARDSIZE);
  (other->copy2).tab = (int **) malloc(sizeof(int*)*BOARDSIZE);

  for(int i = 0; i < BOARDSIZE; i++){
    ((other->Board).grid)[i] = (int *) malloc(sizeof(int)*BOARDSIZE);
    ((other->copy1).tab)[i] = (int *) malloc(sizeof(int)*BOARDSIZE);
    ((other->copy2).tab)[i] = (int *) malloc(sizeof(int)*BOARDSIZE);
  }
  copy_board((other->Board).grid,(node->Board).grid,BOARDSIZE);
  copy_board((other->copy1).tab,(node->copy1).tab,BOARDSIZE);
  copy_board((other->copy2).tab,(node->copy2).tab,BOARDSIZE);


  other->turn = node->turn;
  other->pass_count = node->pass_count;
  other->count = node->count;
  other->Ko_check = node->Ko_check;
  other->cap = CAP;
  other->w = 0;
  other->n = 0;

  play_move_out_of_match(&(other->Board),&(other->copy1),
                         &(other->copy2),&(other->turn),
                         &(other->pass_count), &(other->count),
                         x,y,&(other->Ko_check));

  other->possible_size = 0;
  other->children = 0;

  other->possiblex = (int*) malloc(sizeof(int)*(other->cap));
  other->possibley = (int*) malloc(sizeof(int)*(other->cap));
  list_possible_moves(&(other->possiblex),&(other->possibley),
                      &(other->possible_size), &(other->Board),
                      &(other->copy1),&(other->copy2),
                      other->turn,other->count,
                      other->Ko_check,&(other->cap));
 
  other->next = (MC_node **) malloc((other->cap) * sizeof(MC_node*));
  return other;
}

MC_node* play_child(MC_node* node,int child){
  if(child >= node->possible_size){
    return NULL;
  }

  if (child < node->children){
    return node->next[child];
  }

  else{
    MC_node* other = (MC_node*) malloc(sizeof(MC_node));
    for(int i = node->children; i < child; i++){
      other = play_move_from_node(node,(node->possiblex)[i],(node->possibley)[i]);
      node->next[i] = other;
    }
    return other;
  }
}




