#ifndef MCTS_H
#define MCTS_H


#define HASH_TABLE_MAX 100000
#define CHILDREN_MAX   BOARD_SIZE

#include "board.h"
#include <math.h>

extern const double C;
typedef int Move;

typedef struct MCNode{
  zobristEncoding state;
  int w;
  int n;
  int turn;
  int numChildren;
  struct MCNode *parent;
  struct MCNode* children[CHILDREN_MAX];

  Move move; // last move
  Move* unselectedMoves;
  int numUnselectedMoves;

} MCNode;

typedef struct HashNode {
  zobristEncoding key;
  MCNode* node;
  HashNode* next;
  int w;
  int n;
} HashTableNode;

static HashNode* HashTable[HASH_TABLE_MAX];

Node* lookup(zobristEncoding key);
void insert(Node* node); 

game reconstruct_game(MCNode* node);

int selection(MCNode* node);
void expansion(MCNode* node);
void simulation(MCNode* node);
void back_propagation(MCNode* node)

#endif // MCTS_H
