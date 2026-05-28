#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "board.h"
#include "sgfwriter.h"
#include <inttypes.h>
#include <assert.h>
#define PRINT_BOARD

#define TUI

#define DEBUG_BOARD
#define DEBUG_COLOR
#define DEBUG_CHAINS
#define DEBUG_NEXTPOS
#define DEBUG_CHAINS_INFO



zobristEncoding random_table[2* BOARD_SIZE + 1];
int neighbors_bounds[BOARD_SIZE][4];


void init_zobrist(){
  FILE *f = fopen("/dev/urandom", "rb");
  if (!f){
    fprintf(stderr, "Issue while reading /dev/urandom\n");
  }
  fread(random_table, sizeof(zobristEncoding), 2 * BOARD_SIZE + 1, f);
  fclose(f);
}


void hash(zobristEncoding* state , int pos, int color){
  *state = (*state) ^ random_table[pos * 2 + (color - 1)];
}


void hash_pass(zobristEncoding* state){
  *state = (*state) ^ random_table[2 * DEFAULT_SIZE];
}


int choose_random_move(Game* game, int* moves, int moves_len){
  if (moves_len == 0) return -1;
  int r = rand() %  moves_len;
  return moves[r];
}


int get_good_moves(Game* game, int* good, int* legal_moves, int legal_len){
  int cnt = 0;
  for (int i = 0 ; i < legal_len; i++){
    if (!is_move_self_eye(game, legal_moves[i])) good[cnt++] = legal_moves[i];
  }
  return cnt;
}


int get_legal_moves(Game* game, int* mark){
//marks legal and returns how many were found

int cnt = 0;
for (int pos = 0; pos < BOARD_SIZE ; pos++){
  if (is_move_legal(game, pos)){ // legal
    mark[cnt++] = pos;
  }
}
return cnt;
}


int is_move_legal(Game* game,int pos){
    if (game->Board[pos].color != EMPTY) return 0;  // occupied
    if (pos == game->koPos) return 0;               // ko
    // suicide check: would this move have 0 liberties after placement?
    // a move is legal if:
    // 1. it has at least one empty neighbor, OR
    // 2. it connects to a friendly chain with >1 liberty, OR
    // 3. it captures an enemy chain
    for (int dir = 0; dir < 4; dir++) {
        int nb = neighbors_bounds[pos][dir];
        if (nb == -1) continue;
        if (game->Board[nb].color == EMPTY) return 1;
        if (game->Board[nb].color == game->turn) {
            if (liberty_count(game,game->Board[nb].chainId) > 1) return 1;
        }
        if (game->Board[nb].color != game->turn) {
            if (liberty_count(game,game->Board[nb].chainId) == 1) return 1; // capture
        }
    }
    return 0;  // suicide
}

// int next_is_big_capture()

int is_move_self_eye(Game* game, int pos) {

int x = pos % DEFAULT_SIZE ;
int y = pos / DEFAULT_SIZE ;
int cnt      = 0 ;
int cnt_same = 0 ;
int cnt_opp  = 0 ;

int p[8];
int c[8];

p[0] = (y > 0 )               ? POS_UP(pos)     : -1;
p[1] = (y < DEFAULT_SIZE - 1) ? POS_DOWN(pos)   : -1;
p[2] = (x > 0)                ? POS_LEFT(pos)   : -1;
p[3] = (x < DEFAULT_SIZE - 1) ? POS_RIGHT(pos)  : -1;

p[4] = (x < DEFAULT_SIZE - 1 && y < BOARD_SIZE -1) ? POS_RIGHT(p[1]) : -1;
p[5] = (x < DEFAULT_SIZE - 1 && y > 0)             ? POS_RIGHT(p[0]) : -1;
p[6] = (x > 0 && y < DEFAULT_SIZE - 1)             ? POS_LEFT(p[1])  : -1;
p[7] = (x > 0 && y > 0)                            ? POS_LEFT(p[0])  : -1;

for (int i = 0; i != 8;i++){
  if (p[i] == -1) {
    c[i] = game->turn;
    continue;
  } 
  cnt++;
  if (game->Board[p[i]].color == game->turn) {c[i] = game->turn; cnt_same++;}
  if (game->Board[p[i]].color != game->turn && game->Board[p[i]].color != EMPTY) {c[i] = game->Board[p[i]].color; cnt_opp++;}
}
// analise each case
if (c[0] == c[1] && c[1] == c[2] && c[3] == c[2] && c[3] == game->turn){
  if (cnt == 3 && cnt_same >= 2 && cnt_opp == 0 ||
      cnt == 5 && cnt_same >= 3 && cnt_opp == 0 ||
      cnt == 8 && cnt_same >= 4 && cnt_opp == 0 || 
      cnt == 8 && cnt_same == 7 && cnt_opp == 1) return 1;
}

return 0;
}



int liberty_count(Game* game, int chain_id){
  int cnt = 0;
    for (int i = 0; i < 6; i++)
        cnt += __builtin_popcountll(game->chains[chain_id].liberties[i]);
    return cnt;
}

void update_chains_size(Game* game){
// control the size of game->chains every some moves;
// We only want to update game->chains, game->Board[i].chainId 
Game game2 = *game;
int cnt = 0;
for (int i = 0; i < MAX_CHAIN_LIST_SIZE; i++){
  game2.chains[i] = (Chain) {-1,-1,-1, {0,0,0,0,0,0}};
}

for (int c_id = 0; c_id != game->chainCount; c_id++){
  int head = game->chains[c_id].head;
  
  // check if chain actually
  if (head != -1){
    // create new chain of chainId cnt and same other info as chain[c_id];
    int search = head;
    game2.chains[cnt].id           = cnt;
    game2.chains[cnt].head         = head;
    game2.chains[cnt].color        = game->chains[c_id].color;
    for(int k = 0; k !=6 ; k++){
      game2.chains[cnt].liberties[k] = game->chains[c_id].liberties[k];
    }
    
    // iterate over chain to update the Board[i].chainId
    while (game->Board[search].nextPos != -1){
      game2.Board[search].chainId = cnt;
      search = game->Board[search].nextPos;
    }
    
    // update tail
    game2.Board[search].chainId = cnt;
    cnt++;
  }
}
game2.chainCount = cnt;

// copy content of game2 into *game
*game = game2;
}


int capture_exists(int captured_chains[4]){
  // returns 1 if a capture happened, 0 otherwise
  return captured_chains[0] != -1 || captured_chains[1] != -1 || captured_chains[2] != -1 || captured_chains[3] != -1;
}

int detect_captures(Game *game, int pos,int* captured_chains){
  int cap_cnt = 0;
  int seen_neighbors[4] = {-1,-1,-1,-1};
  for (int dir = 0; dir != 4; dir++){
    int pos_check = neighbors_bounds[pos][dir];
    if ( pos_check != -1 && game->Board[pos_check].color != game->Board[pos].color && game->Board[pos_check].color != EMPTY){ 
      int seen = 0;
      for (int i = 0 ; i != 4 ; i++){
          if (seen_neighbors[i] == game->Board[pos_check].chainId) seen = 1;
      }
        // neighbor contains an opponent chain -> update liberties of opp chain and check if captured
      if (seen) continue;
      seen_neighbors[dir] = game->Board[pos_check].chainId;
      if(check_capture(game, game->Board[pos_check].chainId)){
        captured_chains[cap_cnt++] = game->Board[pos_check].chainId;
      }
    }
  }
  return cap_cnt;
}


void merge_chain(Game *game, int pos, int chain_id){
  // check if new chain and other chains need merge, if yes, merge them and update libertyList
  // Otherwise add new chain to chain list and update chain Count and libertiesList of the old chain;
  //
  // how merging works: 
  // if merge needed, we iterate over the chain that contains pos, and change their chainId to the chainId
  // of the chain we are merging with. and we set the head of the chain to pos and we connect that tail of 
  // the chain containing pos to the old head of the other chain we are merging with.
  //
  // at the same time, we count liberties to update it.
  
  int color = game->Board[pos].color;
  for (int dir = 0 ; dir < 4; dir++){
    int pos_check = neighbors_bounds[pos][dir];
    if ( pos_check != -1 && game->Board[pos_check].color == color && game->Board[pos_check].chainId != chain_id){
      // if valid and merging is necessary pos becomes the head and chain_id changes
      int old_chain = game->Board[pos_check].chainId; // old is the already existing;

      game->Board[pos].chainId = chain_id;
      game->chains[chain_id].head = pos;
      game->Board[pos].nextPos = game->chains[old_chain].head;

      game->chains[chain_id].liberties[pos / 64] &= ~(1ULL << (pos % 64));
      game->chains[old_chain].liberties[pos / 64] &= ~(1ULL << (pos % 64));

      int search = game->chains[chain_id].head; // start at pos
      int guard = 0;

      while(search != -1){
        //fprintf(stderr,"%d %d \n",color, game->Board[pos_check].color);
        assert (guard == 0 || search != pos );
        game->Board[search].chainId = chain_id;
        search = game->Board[search].nextPos;
        guard++;
      }

      // merge liberties : 
      for(int i = 0; i != 6 ; i++){
          game->chains[chain_id].liberties[i] |= game->chains[old_chain].liberties[i];
        }
      game->chains[old_chain] = (Chain) {-1,-1,-1,{0,0,0,0,0,0}};
    }
    if (pos_check != -1 && game->Board[pos_check].color != game->Board[pos].color && game->Board[pos_check].color != EMPTY){
      //update liberties of opp stones:
      game->chains[game->Board[pos_check].chainId].liberties[pos / 64] &= ~(1ULL << (pos % 64));
    }
  }
}


int remove_stone(Game* game, int pos){
  // remove stone at pos and update liberties of adjancent stones.
  // also updates hash. it also returns the next in the chain

  int color = game->Board[pos].color;
  for (int dir = 0; dir != 4 ; dir++){
    int pos_check = neighbors_bounds[pos][dir];
    if (pos_check != -1 && game->Board[pos_check].color != color && game->Board[pos_check].color != EMPTY){
      int neighbors_chain_id = game->Board[pos_check].chainId;
      game->chains[neighbors_chain_id].liberties[pos / 64] |= 1ULL << (pos % 64);
    }
  }
  // Now we can set the Board[pos] to the default empty value :
  int next = game->Board[pos].nextPos;
  hash(&(game->state), pos, game->Board[pos].color);
  game->Board[pos] = (Node) {EMPTY,-1,-1};
  return next;
}


void handle_captures(Game* game, int* captured_chains){
// given the captured chains, we remove the stones and update Board;
int cap_cnt = 0;
int last_captured;

// we know that len(captured_chains) <= 4
for (int i = 0 ; i != 4 ; i++){
  if(captured_chains[i] != -1){
    // found a chain, so we can iterate over it

    int pos = game->chains[captured_chains[i]].head;
    int color = game->Board[pos].color;

    // remove all the stones from a chain: 
    while (game->Board[pos].nextPos != -1){
        int next = remove_stone(game, pos); 
        cap_cnt++;
        game->Board[pos] = (Node) {EMPTY,-1,-1};
        pos = next;
      }
      remove_stone(game, pos);
      last_captured = pos;
      game->Board[pos] = (Node) {EMPTY,-1,-1};
      cap_cnt++;

      // just because ...
      game->chains[captured_chains[i]] = (Chain) {-1,-1,-1,{0,0,0,0,0,0}};
    }
  }
  
  // update ko flag -> only 1 stone captured then a ko is possible at last_captured
  game->koPos = (cap_cnt == 1) ? last_captured : -1;
  if (game->turn == BLACK) game->capturesBlack += cap_cnt;
  else                     game->capturesWhite += cap_cnt;

}


int play(Game* game,int x, int y){
  // human wrapper
  int pos = POS(x,y);
  if (x >= 0 && y >= 0 && x <= DEFAULT_SIZE - 1 && y <= DEFAULT_SIZE - 1){
    return play_pos(game, pos);
  }
  return 1; // out of bounds!!
}


int play_pos(Game *game, int pos){ 
  // Plays move at pos and updates Game state.
  // exit values: 0 -> valid move ; 
  // 2 -> ko invalid ; 
  // 3 -> existing stone invalid ; 
  // 4 -> suicide invalid ; 
  if (pos == game->koPos) return 2; // ko flag active
  if (pos == PASS){
    game->koPos = -1;
    hash_pass(&(game->state));
    game->turn = (game->turn == WHITE) ? BLACK : WHITE;
    game->pass++;
    return 0 ;
  }

  if (game->Board[pos].color != EMPTY) return 3; // stone already place at pos 

  //fprintf(stderr,"turn: %d\n",game->turn);
  game->Board[pos].color = game->turn;

  //creates newChain containing pos 
  Chain newChain;
  memset(&newChain, 0, sizeof(newChain));
  newChain.color        = game->turn;
  newChain.head         = pos;
  newChain.id           = game->chainCount;
  memset(newChain.liberties, 0 , sizeof(newChain.liberties)) ;

  game->Board[pos].chainId = newChain.id;
  for (int dir = 0 ; dir < 4; dir++){
    if (neighbors_bounds[pos][dir] != -1 && game->Board[neighbors_bounds[pos][dir]].color == EMPTY){
      newChain.liberties[neighbors_bounds[pos][dir] / 64] |= 1ULL << (neighbors_bounds[pos][dir] % 64);
    }
  }

  game->chains[newChain.id] = newChain;
  game->chainCount++;
  merge_chain(game, pos, newChain.id);

  // now we handle captures:
  int captured_chains[4] = {-1,-1,-1,-1}; //list of captured chains
  int num_captured = detect_captures(game, pos, captured_chains);
  if (!num_captured && check_capture(game, game->Board[pos].chainId)) return 4;  // suicided stones without capture
  if (num_captured) handle_captures(game, captured_chains);
  
  // assert(game->chainCount < MAX_CHAIN_LIST_SIZE);
  hash(&(game->state), pos, game->turn);
  game->pass = 0;
  game->turn = (game->turn == WHITE) ? BLACK : WHITE;
  game->moveCount++;
  if (game->chainCount > MAX_CHAIN_LIST_SIZE - 4)
    update_chains_size(game);
   // if (game->chains[game->Board[pos].chainId].libertyCount == 0) printf("asdasd");
  return 0;
}


void print_board(Game* game){
   for (int j = 0; j != DEFAULT_SIZE ; j++){
      for (int i = 0; i != DEFAULT_SIZE; i++){
        printf("%d ", game->Board[POS(i, j)].color);
      }
      printf("\n");
    }
}


void print_debug(Game* game){
    #ifdef DEBUG_COLOR
    printf("color: \n");
    for (int j = 0; j != DEFAULT_SIZE ; j++){
      for (int i = 0; i != DEFAULT_SIZE; i++){
        printf("%d ", game->Board[POS(i, j)].color);
      }
      printf("\n");
    } 
    #endif

    #ifdef DEBUG_CHAINS
    printf("chainId: \n");
    for (int j = 0; j != DEFAULT_SIZE ; j++){
      for (int i = 0; i != DEFAULT_SIZE; i++){
        printf("%d ", game->Board[POS(i, j)].chainId);
      }
      printf("\n");
    }
    #endif

    #ifdef DEBUG_NEXTPOS
    printf("nextPos: \n");
    for (int j = 0; j != DEFAULT_SIZE ; j++){
      for (int i = 0; i != DEFAULT_SIZE; i++){
        printf("%d ", game->Board[POS(i, j)].nextPos);
      }
      printf("\n");
    }
    #endif

    #ifdef DEBUG_CHAINS_INFO
    for (int i = 0; i < game->chainCount; i++){
      printf("chain: %d ; liberties: %d ; color %d ; head: %d \n", i, liberty_count(game,i), game->chains[i].color, game->chains[i].head);
    }
    #endif
    int mark[BOARD_SIZE], mark2[BOARD_SIZE];
    int l = get_legal_moves(game,mark);
    int l2 = get_good_moves(game,mark2,mark,l);
    for(int i = 0 ; i < l2 ; i++){
      printf("%d ",mark2[i]);
  }
}


void print_rules(){
  printf("Insert your moves as a pair \"x y\" of integers\n");
  return;
}


int touch_color(const Game* game, int pos, int color){
  int visited[BOARD_SIZE];
  memset(visited, 0, sizeof(visited));
  int head=0; 
  int tail=1;
  int queue[BOARD_SIZE];

  queue[head] = pos;
  while(head < tail){
    int idx = queue[head++];
    if (visited[idx] == 1) continue;
    visited[idx] = 1;
    for (int dir = 0 ; dir != 4  ; dir++){
      if (neighbors_bounds[idx][dir] != -1){
        int next = neighbors_bounds[idx][dir];
        if (!visited[next] && game->Board[next].color == color) return 1;
        else if (!visited[next] && game->Board[next].color == EMPTY) queue[tail++] = next;
      }
    }
  }

  return 0;
}


/*int game_eval(Game* game, float* points_b, float* points_w){
  // evaluates game position
  int b_cnt = 0, w_cnt = 0;
  int visited[BOARD_SIZE];
  memset(visited, 0, sizeof(visited));

  for (int i = 0 ; i != BOARD_SIZE; i++){
    if (game->Board[i].color == EMPTY && visited[i] == 0){
      int touch_black = touch_color(game, i, BLACK);
      int touch_white = touch_color(game, i, WHITE);

      if (touch_black && touch_white) return -1;  // undefined because not everything was captured!

      int queue[BOARD_SIZE];
      int head = 0;
      int tail = 1;
      queue[head] = i;

      while (head < tail){
        int idx = queue[head++];
        if (visited[idx] == 1) continue;
        visited[idx] = 1;

        if      (touch_black) b_cnt++;
        else if (touch_white) w_cnt++;

        for (int dir = 0; dir != 4; dir++){
          int next = POS_CHECK(idx,dir);
          if (INBOUNDS_CHECK(idx,dir) && game->Board[next].color == EMPTY && !visited[next]) queue[tail++] = next;
        }
      }
    }
  }

  *points_b = (float) b_cnt + (float) game->capturesBlack;
  *points_w = (float) w_cnt + game->komi + (float) game->capturesWhite ;
  return (*points_b > *points_w);
}
*/

int game_eval(Game* game, float* points_b, float* points_w) {

    int visited[BOARD_SIZE];
    memset(visited, 0, sizeof(visited));

    int b_score = game->capturesBlack;
    int w_score = game->capturesWhite;

    int queue[BOARD_SIZE];

    for (int i = 0; i < BOARD_SIZE; i++) {

        // count stones directly
        if (game->Board[i].color == BLACK) {
            b_score++;
            continue;
        }

        if (game->Board[i].color == WHITE) {
            w_score++;
            continue;
        }

        // skip visited empty intersections
        if (visited[i])
            continue;

        // flood fill empty region
        int head = 0;
        int tail = 0;

        queue[tail++] = i;
        visited[i] = 1;

        int region_size = 0;

        int touches_black = 0;
        int touches_white = 0;

        while (head < tail) {

            int pos = queue[head++];
            region_size++;

            for (int dir = 0; dir < 4; dir++) {

                if (neighbors_bounds[pos][dir]==-1)
                    continue;

                int next = neighbors_bounds[pos][dir];
                int color = game->Board[next].color;

                if (color == EMPTY) {

                    if (!visited[next]) {
                        visited[next] = 1;
                        queue[tail++] = next;
                    }

                } else if (color == BLACK) {

                    touches_black = 1;

                } else if (color == WHITE) {

                    touches_white = 1;
                }
            }
        }

        // assign territory
        if (touches_black && !touches_white)
            b_score += region_size;

        else if (touches_white && !touches_black)
            w_score += region_size;
    }

    *points_b = (float)b_score;
    *points_w = (float)w_score + game->komi;

    return (*points_b > *points_w);
}


int game_loop(Game* game){
  game_init(game);

  #ifdef TUI
    print_rules();
  #endif
  while (1){
    int out = game_play(game);
    if (out == 1) fprintf(stderr, "Invalid values for x and/or y\n");
    else if (out == 2) {
      float points_w, points_b;
      int winner = game_eval(game, &points_b, &points_w);

      if (winner == 1)      printf("BLACK won with %d against %d points\n",points_b,points_w);
      else if (winner == 0) printf("WHITE won with %d against %d points\n",points_w,points_b);
      // else if (winner == -1) {
        // printf("PASS invalid move because cant eval");
        // continue;
    // }
    return winner;
    }
    #ifdef PRINT_BOARD
      print_board(game);
    #endif
    #ifdef DEBUG_BOARD
      print_debug(game);
    #endif
  }

  return -1;
}


void game_init(Game* game){
  game->moveCount = 0;
  game->state = 0;
  game->turn = BLACK;
  game->koPos = -1;
  game->pass = 0;
  game->capturesWhite = 0;
  game->capturesBlack = 0;
  game->chainCount = 0;
  game->komi = 7.5;
  //fprintf(stderr,"asdasd");
  for (int i = 0 ; i != MAX_CHAIN_LIST_SIZE; i++){
    game->chains[i] = (Chain) {-1,-1,-1,{0,0,0,0,0,0}};
  }
  for (int i = 0 ; i != BOARD_SIZE; i++){
    game->Board[i] = (Node) {EMPTY,-1,-1};
  }
}


int game_play(Game *game){
  // invalid moves array;
  int invalid[BOARD_SIZE];
  memset(&invalid, 0, sizeof(invalid));

  #ifdef TUI
    printf("Play your move: "); 
  #endif
  // player input processing
  int x,y;
  scanf("%d %d",&x,&y);

  if (x == -1 && y == -1){
    game->pass++;
    hash_pass(&(game->state));
    if (game->pass == 2) return 2;
    game->turn = (game->turn == WHITE) ? BLACK : WHITE;
    return 0;
  }

  game->pass = 0;
  Game tmp = *game; // back up board in case of invalid moves
  int error = play(game, x, y);
  int pos = POS(x,y);
  if (invalid[pos] || error){ 
    // not allow move and store "invalidness" to avoid recomputation
    *game = tmp;
    invalid[pos] = 1;
    return 1;
  }
  return 0;
}

void init_neighbors(){
  for (int x = 0 ; x != DEFAULT_SIZE; x++){
    for (int y = 0 ; y != DEFAULT_SIZE; y++){
      for (int dir = 0; dir != 4; dir++){
        neighbors_bounds[x+y*DEFAULT_SIZE][0] = (x > 0) ?  x - 1 + y * DEFAULT_SIZE : -1;
        neighbors_bounds[x+y*DEFAULT_SIZE][1] = (x < DEFAULT_SIZE - 1) ? x + 1 + y * DEFAULT_SIZE: -1;
        neighbors_bounds[x+y*DEFAULT_SIZE][2] = (y > 0) ? x + (y-1)*DEFAULT_SIZE : -1;
        neighbors_bounds[x+y*DEFAULT_SIZE][3] = (y < DEFAULT_SIZE - 1) ? x + (y+1)*DEFAULT_SIZE : -1;
      }
    }
  }
}


