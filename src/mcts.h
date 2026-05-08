#ifndef MCTS_H
#define MCTS_H


#define HASH_TABLE_MAX 100000
#define CHILDREN_MAX   BOARD_SIZE

#include "board.h"
#include <math.h>

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
  struct MCNode** children;

  Move move;
  int numUnselectedMoves;
  UnselectedMoves* UnselectedList;
} MCNode;

typedef struct HashNode {
  zobristEncoding key;
  MCNode* node;
  HashNode* next;
  int w;
  int n;
} HashNode;

static HashNode** HashTable;

UnselectedMoves* remove_unselected(Move* moves, int move_len);
void create_unselected(UnselectedMoves* head, Move* moves);
void free_unselected(UnselectedMoves* head);

Node* ht_lookup(zobristEncoding key);
void ht_init();
void ht_free();
void ht_insert(MCNode* node, int w, int n);
HashNode* ht_lookup(zobristEncoding state);

int selection(MCNode* node, Game* game);
void expansion(MCNode* node, int move, Game* game);
void simulation(MCNode* node, Game* game);
void back_propagation(MCNode* node, int res);

void mcts_loop(int max_it);
void mcts_it(MCNode* root, Game* game);
void test_mcts();


#endif // MCTS_H
