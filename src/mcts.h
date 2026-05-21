#ifndef MCTS_H
#define MCTS_H


#define HASH_TABLE_MAX 100000
#define CHILDREN_MAX   BOARD_SIZE

#include "board.h"
#include <math.h>
#include <stdlib.h>
#include <assert.h> 
#include<time.h>


extern const double C;
typedef int Move;

typedef struct UnselectedMoves{
  int idx;
  int move;
  struct UnselectedMoves* next;
} UnselectedMoves;

typedef struct MCNode{
  zobristEncoding state;
  int w;
  int n;
  int turn;
  int numChildren;
  struct MCNode* parent;
  struct MCNode* children[CHILDREN_MAX];

  Move move;
  int numUnselectedMoves;
  UnselectedMoves* unselectedMoves;
} MCNode;

typedef struct HashNode {
  zobristEncoding key;
  MCNode* node;
  struct HashNode* next;
  int w;
  int n;
} HashNode;

static HashNode** HashTable;

void remove_unselected(UnselectedMoves** head,Move move);
UnselectedMoves* create_unselected(Move* moves, int moves_len);
void free_unselected(UnselectedMoves* head);

void ht_init();
void ht_free();
void ht_insert(MCNode* node, int w, int n);
HashNode* ht_lookup(zobristEncoding state);

MCNode* selection(MCNode* root, Game* game);
MCNode* expansion(MCNode* node, int move, Game* game);
int simulation(MCNode* node, Game* game);
void back_propagation(MCNode* node, int res);
int random_playout_move(Game* g);

void mcts_loop(int max_it);
void mcts_it(MCNode* root, Game* game);
void test_mcts();
void print_mctree(MCNode* root, int depth);

#endif // MCTS_H
