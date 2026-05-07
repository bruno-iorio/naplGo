#include "mcts.h"
#include "board.h"

const double c = 1;

HashNode* ht_lookup(zobristEncoding state){
  HashNode* entry = HashTable[state % HASH_TABLE_MAX];
  while (entry){
    if (entry->key == state) return entry;
    entry = entry->next;
  }
  return NULL;
}

void ht_insert(MCNode* node, int w, int n){
  // rule: only insert after lookup failed
  int idx = node->state % HASH_TABLE_MAX;
  HashNode* entry = HashTable[idx];
  HashNode* new_entry = (HashNode*) malloc(sizeof(HashNode));
  new_entry->key = node->state;
  new_entry->node = node;
  new_entry->next = entry;
  new_entry->w = w;
  new_entry->n = n;

  HashTable[idx] = new_entry;
  return;
}

int best_score(MCNode* node){
  int best = -1;
  double score = -1;
  for (int i = 0 ; i < node->numChildren; i++){
    int wi = node->Children[i].w;
    int ni = node->Children[i].n;
    double tmp = (double) wi / (double)ni + c * sqrt(log((double)node->n) / (double)ni);
    if (tmp > score){
      score = tmp;
      best = i;
    }
  }
  return best;
}

MCNode* selection(MCNode* root, Game* game) {
    MCNode* node = root;
    while (node->numUntried > num_illegal_moves(game)){
      int i = best_score(node);
      play_pos(game, node->children[i]->move);
      node = node->children[i];
    }
    return node;
}

void expansion(MCNode* parent, int move, Game* game){
  NodeMC *child = (NodeMC*) malloc(sizeof(NodeMC));
  NodeMC* entry = lookup(game->state);
  child->state = game->state;
  if (entry){
    child->w = entry->w;
    child->n = entry->n;
  }
  else {
    child->w = 0; 
    child->n = 0;
    ht_insert(child,0,0);
  }
  child->turn = (parent->turn == BLACK) ? WHITE : BLACK;
  child->numChildren = 0;
  child->parent = node;
  memset(child->children, 0 , sizeof(child->children));
  parent->children[parent->numChildren++] = child;
  return;
}





void simulation(MCNode* node, Game* game){
  Game s = *game;
  while(true){
    // pick a move
    Move move = pick_move(&s);
    if (move == -1) {
      sim.pass++; 
    }
    else {
      sim.pass = 0;
      play_pos(&sim,move);
    }
  }
  float b,w;
  return eval_winner(&sim, b, w);
}

void back_propagation(){
  return;
}

void back
