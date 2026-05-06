#ifndef MCTS_H
#define MCTS_H
#define HASH_TABLE_MAX 100000
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
  struct MCNode **children;
  int capacityChildren

  Move move; // last move
  Move* untriedMoves;
  int numUntriedMoves;

} MCNode;

typedef struct HashNode {
  zobristEncoding key;
  MCNode* node;
  HashNode* next;
} HashTableSlot;

static HashNode* HashTable[HASH_TABLE_MAX];

Node* lookup(zobristEncoding key);
void insert(Node* node); 

game reconstruct_game(MCNode* node);

int selection(MCNode* node);
void expansion(MCNode* node);
void simulation(MCNode* node);
void back_propagation(MCNode* node)

#endif // MCTS_H
