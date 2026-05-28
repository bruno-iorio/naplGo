#include "mcts.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
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
    int wi = node->children[i]->w;
    int ni = node->children[i]->n;
    if (ni == 0) return i;
    double tmp = (double) wi / (double)ni + C * sqrt(log((double)node->n) / (double)ni);
    if (tmp > score){
      score = tmp;
      best = i;
    }
  }
  return best;
}


MCNode* selection(MCNode* root, Game* game) {
  // we navigate through the graph until we find a node that has yet children to be visited...
  MCNode* node = root;
  while (node->numUnselectedMoves == 0){
    int i = best_score(node);
    play_pos(game, node->children[i]->move);
    node = node->children[i];
  }
  return node;
}


MCNode* expansion(MCNode* parent, int move, Game* game){
  // allocate new node and returns it as child.
  play_pos(game, move);
  MCNode *child = calloc(1, sizeof(MCNode));
  HashNode *entry = ht_lookup(game->state);
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
  child->parent = parent;
  child->move = move; // move that generated this child.
  parent->children[parent->numChildren++] = child;
  return child;
}

int random_playout_move(Game* g) {
    // try random moves
    for (int tries = 0; tries < 50; tries++) {
        int move = rand() % BOARD_SIZE;
        if (g->Board[move].color != EMPTY) continue;
        if (move == g->koPos) continue;
        Game tmp = *g;
        if (play_pos(&tmp, move) == 0)
            return move;
    }
    return PASS;  // give up after 50 tries → triggers pass → game ends sooner
}
int simulation(MCNode* node, Game* game){

  Game s = *game;
  
  int move;
  Move legal[BOARD_SIZE];
  Move good[BOARD_SIZE];
  int legal_len = get_legal_moves(&s,legal);
  int good_len = get_good_moves(&s, good, legal, legal_len);
    
  node->unselectedMoves = create_unselected(good, good_len);
  node->numUnselectedMoves = good_len;
  while(s.pass < 2){
    // pick a move
    int move = random_playout_move(&s);
    play_pos(&s, move);
  }
  float b,w;
  //print_board(&s);
  //printf("\n");
  int res = game_eval(&s, &b, &w);
  return res;
}


void back_propagation(MCNode* node, int res){
  while (node){
    HashNode *entry = ht_lookup(node->state);
    if ((res == 1 && node->turn == WHITE) || (res == 0 && node->turn == BLACK)) {
      if (entry) (entry->w)++;
      (node->w)++;
    }
    if (entry) entry->n++;
    node->n++;
    node = node->parent;
  }
}

Move get_unselected(UnselectedMoves** head, int idx){
  // return move that was unselected from the head unselectedMoves linked list
  if (!head || !(*head))
    return -1;

  UnselectedMoves *search = *head;
  UnselectedMoves *prev = NULL;
  int i = 0;

  // get search to be the idx-th term of the list and removes it from the list
  while (search && i < idx){
    prev = search;
    search = search->next;
    i++;
   }

  if (!search)
      return -1;

  Move move = search->move;

  if (prev)
      prev->next = search->next;
  else // prev was never set so idx == 0
      *head = search->next;   // removing head
  
  // avoid leaks
  free(search);

  return move;
}

void mcts_it(MCNode* root, Game* game){

  // applies selection
  MCNode* node = selection(root, game);

  // get a random unselected move and expands it;
  // has to garantee that numUnselectedMoves > 0 -> whole point of this
  assert(node->numUnselectedMoves > 0 && node->numUnselectedMoves <= BOARD_SIZE);

  // gets random unselected move id:
  int idx = rand() % node->numUnselectedMoves;
  Move move = get_unselected(&node->unselectedMoves, idx);
  node->numUnselectedMoves--; 
  
  MCNode* child = expansion(node, move, game);
  int res = simulation(child, game);
  back_propagation(child, res);
}

// check if unselectedMoves logic is right:
// unselectedMoves is linked list 
// initialization in simulation
// every time expands, has to remove.


MCNode* mcts_loop(int max_it){
  MCNode* root = (MCNode*) malloc(sizeof(MCNode));
  root->state = 0;
  root->w = 0;
  root->n = 0;
  root->turn = BLACK;
  root->numChildren = 0;
  root->parent = NULL;
  root->move = -1; // arbitrary
  Game game;
  game_init(&game);
  Move board[BOARD_SIZE];
  ht_insert(root,0,0);
  for (int i = 0; i!= BOARD_SIZE; i++){
    board[i] = i;
  }
  root->unselectedMoves = create_unselected(board, BOARD_SIZE);
  root->numUnselectedMoves = BOARD_SIZE;
  int it = 0;

  while (it++ < max_it) {
    // fprintf(stderr,"%d",it);
    game_init(&game);
    mcts_it(root, &game);
  }
  print_mctree(root,10);
  return root;
}


UnselectedMoves* create_unselected(Move* moves, int moves_len){
  
  if (moves_len == 0) return NULL;
  UnselectedMoves *p,  *head;
  head = (UnselectedMoves*) malloc(sizeof(UnselectedMoves));
  p = head;

  int i = 0;
  while(i < moves_len){
    UnselectedMoves *p_next = NULL;
    if (i < moves_len-1)
      p_next = (UnselectedMoves*) malloc(sizeof(UnselectedMoves));
    
    p->move = moves[i];
    p->next = p_next;
    p = p_next;

    i++;
  }
  return head;
}

void remove_unselected(UnselectedMoves** head, Move move){
  UnselectedMoves *curr, *prev;
  curr = *head;
  prev = NULL;
  while (curr){
    if (curr->move == move){
      if (prev) prev->next = curr->next;
      else *head = curr->next;
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
    head = head->next;
    free(aux);
  }
}

void ht_init(){
  HashTable = calloc(HASH_TABLE_MAX, sizeof(HashNode*));
}

void ht_free(){
  for (int i = 0 ; i != HASH_TABLE_MAX ; i++){
    HashNode *entry = HashTable[i];
    HashNode *next;
    while (entry){
      next = entry->next;
      free(entry);
      entry = next;
    }
  }
  free(HashTable);
  return;
}

void tree_free(MCNode* root, int max_it){
  MCNode* node;
  MCNode* queue[2 * max_it]; //at most max_it nodes, but 2*max_it for avoiding wrong access
  int head = 0;
  int tail = 1;
  queue[head] = root;
  int cnt = 0;
  while (head != tail){
    cnt++;
    node = queue[head++];
    for(int i = 0; i < node->numChildren; i++){
      queue[tail++] = node->children[i];
    }
    free_unselected(node->unselectedMoves);
    free(node);
  }
  // max_it + 1 nodes in total bc of root
  if (cnt != max_it + 1) fprintf(stderr, " %d %d something wrong happened somewhere...",max_it,cnt);
  return;
}

void test_mcts(){
  //assert(100 < HASH_TABLE_MAX);
  int max_it = 10000;
  srand(time(NULL));
  ht_init();
  MCNode* root = mcts_loop(max_it);
  ht_free();
  //tree_free(root, max_it);
}

void print_mctree(MCNode* node, int depth){
    if (!node) return;

    // indent based on depth
    for (int i = 0; i < depth; i++) printf("  ");

    printf("move=%d w=%d n=%d turn=%s children=%d unselected=%d\n",
        node->move,
        node->w,
        node->n,
        node->turn == BLACK ? "B" : "W",
        node->numChildren,
        node->numUnselectedMoves);

    for (int i = 0; i < node->numChildren; i++){
        print_mctree(node->children[i], depth + 1);
    }
}

