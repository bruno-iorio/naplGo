#include "../MCTS/mcts.h"
#include "../Mechanics/board.h"
#include "../Mechanics/sgfparser.h"
#include <stdio.h>
#include <stdlib.h>
int main(){
  printf("it runs");
  MC_node *node = (MC_node* ) malloc(sizeof(MC_node));
  init_node(node);
  printf("The node was initiated\n");
  int i = is_leaf(node);
  printf("%d\n",i);
  free_node(node);
  return 0;
}
