#include "mcts.h"
#include "board.h"

const double C = 1;
int ht_size = 0;

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
    double tmp = (double) wi / (double)ni + C * sqrt(log((double)node->n) / (double)ni);
    if (tmp > score){
      score = tmp;
      best = i;
    }
  }
  return best;
}


MCNode* selection(MCNode* root, Game* game) {
    MCNode* node = root;
    while (node->numUnselectedMoves == 0){
      int i = best_score(node);
      play_pos(game, node->children[i]->move);
      node = node->children[i];
    }
    return node;
}


void expansion(MCNode* parent, int move, int unselectedId ,Game* game){
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
  parent->children[parent->numChildren++] = child;
  return;
}


void simulation(MCNode* node, Game* game){

  Game s = *game;
  Move legal[BOARD_SIZE];
  Move good[BOARD_SIZE];
  int legal_len = get_legal_moves(game,legal);
  int good_len = get_good_moves(game,good, legal, legal_len);
  int move;
  node->unselectedMoves = create_unselected(good, good_len);

  while(true){
    // pick a move

    legal_len = get_legal_moves(game,legal);
    good_len = get_good_moves(game,good, legal, legal_len);
    move = choose_random_move(&s, good, good_len);

    if (move == -1) {
      break;
    }
    else {
      play_pos(&s, move);
    }
  }

  float b,w;
  return eval_winner(&s, &b, &w);
}


void back_propagation(MCNode* node, int res){
  while (node){
    HashNode *entry = ht_lookup(node->state);
    if ((res == 1 && node->turn == BLACK) ||(res == 0 && node->turn == WHITE)) {
      (entry->w)++;
      (node->w)++;
    }
    node->n++;
    node = node->parent;
  }
  return;
}

Move get_unselected(UnselectedMoves* head, int idx){
  UnselectedMoves *search = head;
  UnselectedMoves *prev;
  Move move;

  while (search){
    if (search->idx == idx){
      move = search->move;
      prev->next = search;
      break;
    }

    prev = search;
    search = search->next;
  }
  search=search->next;
  while(search){
    search->idx--;
    search=search.next;
  }
  return move;
}

void mcts_it(MCNode* root, Game* game){
  MCNode* node = selection(root, game);

  int idx = rand() % node->unselectedMoves;
  Move move = get_unselected(idx);
  node->numUnselectedMoves--;
  expansion(node, move, game);

  int res = simulation(node, game);

  back_propagation(node, res);
}


void mcts_loop(int max_it){
  MCNode* root = (NodeMC*) malloc(sizeof(NodeMC));
  root->key = 0;
  root->w = 0;
  root->n = 0;
  root->turn = BLACK;
  root->numChildren = 0;

  int it = 0;
  while (it++ < max_it) {
    Game game;
    init_game(&game);
    mcts_it(root, &game);
  }
  return;
}


UnselectedMoves* create_unselected(Move* moves, int moves_len){
  if (moves_len == 0) return 0;
  UnselectedMoves *p, *head;
  p = head;
  while(i++ < moves_len){
    p = (UnselectedMoves*) malloc(sizeof(UnselectedMoves));
    p->idx = i;
    p->move = moves[i];
    p = p.next;
  }
  return head;
}

void remove_unselected(UnselectedMoves* head, Move move){
  UnselectedMoves *curr, *prev;
  curr = head;
  while (curr){
    if (curr->move == move){
      prev->next = curr->next;
      free(curr);
      break;
  }

  prev = curr;
  curr = curr->next;
  }
}

void free_unselected(UnselectedMoves* head){
  UnselectedMoves* aux = head;
  while (head){
    aux = head;
    head = head.next;
    free(aux);
  }
}

void ht_init(){
  HashTable = (HashNode**) malloc(sizeof(HashNode*) * HASH_TABLE_MAX);
}

void ht_free(){
  // also free nodes ...
  for (int i = 0 ; i != HASH_TABLE_MAX ; i++){
    HashTable* entry = HashTable[i];
    HashTable* next;
    while (entry){
      next = entry->next;
      free_unselected(entry->node->unselectedMoves);
      free(entry->node->children);
      free(entry->node);
      free(entry);
      entry = next;
    }
  }

  return;
}

void test_mcts(){
  ht_init();
  mcts_loop(10);
  ht_free();
}

