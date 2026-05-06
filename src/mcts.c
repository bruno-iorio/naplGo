#include "mcts.h"
#include "board.h"

const double c = 1;

int selection(MCNode* node){
  int next = 0;
  int next_val = -1;
  int N = node->n;
  for (int i = 0 ; i != node->numChildren; i++){
    double val;
    int ni = node->children[i]->n; 
    int wi = node->children[i]->w;
    val = (double) wi / (double) ni + c * sqrt(log((double) N) / (double) ni); 
    if (val > next_val){
      next = i;
      next_val = val;
    }
  }
  return next;
}

void expansion(MCNode* node){
  return;
}

void simulation(){
  return;
}

void back_propagation(){
  return;
}

void back
