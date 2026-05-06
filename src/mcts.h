#ifndef MCTS_H
#define MCTS_H
#include "board.h"
#include <math.h>

static const double c;

typedef struct {
  zobristEncoding state;
  int w;
  int n;
  int turn;
  int numChildren;
  struct MCNode *parent;
  struct MCNode **children;
} MCNode;

int find_next(MCNode* node);
void mtcs_loop(MCNode* head);

#endif // MCTS_H
