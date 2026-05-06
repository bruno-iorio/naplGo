#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "board.h"
#include "sgfwriter.h"

//#define PRINT_BOARD

//#define TUI

#define DEBUG_BOARD
//#define DEBUG_COLOR
//#define DEBUG_CHAINS
//#define DEBUG_NEXTPOS
#define DEBUG_CHAINS_INFO

zobristEncoding random_table[2* BOARD_SIZE + 2];

int POS_CHECK(int pos, int dir)  {
  // returns neighbor of pos at direction dir

  if (dir == UP)    return POS_UP(pos);
  if (dir == DOWN)  return POS_DOWN(pos);
  if (dir == LEFT)  return POS_LEFT(pos);
  if (dir == RIGHT) return POS_RIGHT(pos);
}


int INBOUNDS_CHECK(int pos, int dir) {
  // check if neighbors of pos at direction dir from pos is
  // inside of the Board

  if ((dir == UP    && pos / DEFAULT_SIZE == 0)                 ||
      (dir == DOWN  && pos / DEFAULT_SIZE == DEFAULT_SIZE - 1)  ||
      (dir == LEFT  && pos % DEFAULT_SIZE == 0)                 ||
      (dir == RIGHT && pos % DEFAULT_SIZE == DEFAULT_SIZE- 1) ) return 0;
    return 1;
}

void init_zobrist(){
  FILE *f = fopen("/dev/urandom", "rb");
  if (!f){
    fprintf(stderr, "Issue while reading /dev/urandom\n");
  }
  fread(random_table, sizeof(zobristEncoding), 2 * BOARD_SIZE, f);
  fclose(f);
}

void hash(zobristEncoding* state , int pos, int color){
  *state = (*state) ^ random_table[pos * 2 + (color - 1)];
}
void hash_pass(zobristEncoding* state){
  *state = (*state) ^ random_table[2 * DEFAULT_SIZE + 1];
}







void update_chains_size(Game* game){
  // control the size of game->chains every some moves;
  // We only want to update game->chains, game->Board[i].chainId 
  // and game->chainCount.
  Game game2 = *game;
  int cnt = 0;
  for (int i = 0; i < MAX_CHAIN_LIST_SIZE; i++){
    game2.chains[i] = (Chain) {-1,-1,-1,-1};
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
      game2.chains[cnt].libertyCount = game->chains[c_id].libertyCount;
      
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


int mark_liberties(Game *game, int pos, int* mark){
  // Auxiliary function to count the liberties of a Chain.
  // Mark is the liberties a group containing the stone at pos.
  // if a neighbor of pos is empty, checks if mark[neighbor] 
  // was already counted and, if it was not, updated cnt

  int cnt = 0;
  for (int dir = 0; dir != 4; dir++){
    int pos_check = POS_CHECK(pos,dir);
    if (INBOUNDS_CHECK(pos,dir) && game->Board[pos_check].color == EMPTY)  {
      if (mark[pos_check] == 0){
        cnt++;
        mark[pos_check] = 1;
      }
    }
  }
  return cnt;
}

int detect_captures(Game *game, int pos,int* captured_chains){
  int cap_cnt = 0;
  for (int dir = 0; dir != 4; dir++){
    int pos_check = POS_CHECK(pos,dir);
    if (INBOUNDS_CHECK(pos, dir) && game->Board[pos_check].color != game->Board[pos].color && game->Board[pos_check].color != EMPTY){ 
      // neighbor contains an opponent chain -> update liberties of opp chain and check if captured
      game->chains[game->Board[pos_check].chainId].libertyCount--;
      if(game->chains[game->Board[pos_check].chainId].libertyCount == 0){
        captured_chains[cap_cnt++] = game->Board[pos_check].chainId;
      }
    }
  }
  return cap_cnt;
}


int merge_chain(Game *game, int pos, Chain newChain){
  // check if new chain and other chains need merge, if yes, merge them and update libertyList
  // Otherwise add new chain to chain list and update chain Count and libertiesList of the old chain;
  //
  // how merging works: 
  // if merge needed, we iterate over the chain that contains pos, and change their chainId to the chainId
  // of the chain we are merging with. and we set the head of the chain to pos and we connect that tail of 
  // the chain containing pos to the old head of the other chain we are merging with.
  //
  // at the same time, we count liberties to update it.

  int chain_id =  newChain.id;
  int mark[BOARD_SIZE];
  memset(mark, 0, sizeof(mark));
  
  int cap_cnt = 0;
  int lib_cnt = 0;

  for (int dir = 0 ; dir < 4; dir++){
    int pos_check = POS_CHECK(pos,dir);
    if (INBOUNDS_CHECK(pos,dir) && game->Board[pos_check].color == game->Board[pos].color && game->Board[pos_check].chainId != game->Board[pos].chainId){
      // if valid and merging is necessary pos becomes the head and chain_id changes
      int search = pos;
      chain_id = game->Board[pos_check].chainId;
 
      while (game->Board[search].nextPos  !=  -1) {
        // update lib_cnt , update the chain of search,  update search.
        // Note: we are only counting the liberties of the chain containing pos
        lib_cnt += mark_liberties(game, search, mark);
        game->Board[search].chainId = chain_id;
        search = game->Board[search].nextPos;
      }

      // process the last one
      lib_cnt += mark_liberties(game, search, mark);
      game->Board[search].chainId = chain_id;


      // linking chains:
      game->Board[search].nextPos = game->chains[chain_id].head; // tail is connected to old head
      game->chains[chain_id].head = pos;                         // pos becomes head of new chain
      

      // processing remaining liberties
      search = game->chains[chain_id].head; 
      while (game->Board[search].nextPos  !=  -1) {
        lib_cnt += mark_liberties(game, search, mark);
        search = game->Board[search].nextPos;
      }
      lib_cnt += mark_liberties(game, search, mark);
    }
  }
  game->chains[chain_id].libertyCount = lib_cnt;
  return chain_id;
}

void remove_stone(Game* game, int* pos){
  // remove stone at pos and update liberties of adjancent stones.
  // also updates hash. it also updates pos to the next in the chain


  int neighbors_chains[4] = {-1,-1,-1,-1};
  for (int dir = 0; dir != 4 ; dir++){
    int pos_check = POS_CHECK(*pos,dir);
    if (INBOUNDS_CHECK(*pos,dir) && game->Board[pos_check].color != color && game->Board[pos_check].color != EMPTY){
      int neighbors_chain_id = game->Board[pos_check].chainId;

      // we check if the same chain is touched more than one time by a stone :
      if ( neighbors_chains[0] != neighbors_chain_id &&
            neighbors_chains[1] != neighbors_chain_id &&
            neighbors_chains[2] != neighbors_chain_id &&
            neighbors_chains[3] != neighbors_chain_id &&
            neighbors_chain_id  != -1 ) {
        game->chains[neighbors_chain_id].libertyCount++;
        neighbors_chains[dir] = neighbors_chain_id;
      }
    }
  }
  // Now we can set the Board[pos] to the default empty value :
  int next = game->Board[*pos].nextPos;
  hash(&(game->state), *pos, game->Board[*pos].color);
  game->Board[*pos] = (Node) {EMPTY,-1,-1};
  *pos = next;

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
        remove_stone(game,&pos); 
        cap_cnt++;
      }
      remove_stone(game, &pos);
      cap_cnt++;

      // just because ...
      game->chains[captured_chains[i]] = (Chain) {-1,-1,-1,-1};
    }
  }

  // update ko flag -> only 1 stone captured then a ko is possible at last_captured
  game->koPos = (cap_cnt == 1) ?  last_captured : -1;

}

int play(Game* game,int x, int y){
  // human wrapper
  if (x >= 0 && y >= 0 && x <= DEFAULT_SIZE - 1 && y <= DEFAULT_SIZE - 1 && game->Board[pos].color == EMPTY && pos != game->koPos){
    return play(game, pos);
  }
  if (pos == game->koPos) return 2; // ko flag active
  return 1; // out of bounds!!
}

int play_pos(Game *game, int pos){ 
  // Plays move at pos and updates Game state.

  game->Board[pos].color = game->turn;

  //creates newChain containing pos and prepares it for function input
  Chain newChain;
  memset(&newChain, 0, sizeof(newChain));
  newChain.color        = game->turn;
  newChain.head         = pos;
  newChain.id           = game->chainCount;
  newChain.libertyCount = 0;

  game->Board[pos].chainId = newChain.id;

  for (int dir = 0 ; dir < 4; dir++){
    if (INBOUNDS_CHECK(pos,dir) && game->Board[POS_CHECK(pos,dir)].color == EMPTY){
      newChain.libertyCount++;
    }
  }

  // a move can captures at most 4 chains at a time (each cardinal direction):
  if (newChain.id == merge_chain(game, pos, newChain)) 
    game->chains[newChain.id] = newChain;
  else
    game->chainCount--;

  // now we handle captures:
  int captured_chains[4] = {-1,-1,-1,-1}; //list of captured chains
  int num_captured = detect_captures(game,pos,captured_chains);
  if (!num_captured && game->chains[game->Board[pos].chainId].libertyCount == 0) return 3;  // suicided stones without capture
  if (num_captured) handle_captures(game, captured_chains);

  game->chainCount++;
  hash(&(game->state), pos, game->turn);

  game->turn = (game->turn == WHITE) ? BLACK : WHITE;
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
      printf("chain: %d ; liberties: %d ; color %d ; head: %d \n", i, game->chains[i].libertyCount, game->chains[i].color, game->chains[i].head);
    }
    #endif
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
    if (INBOUNDS_CHECK(idx,dir)){
    int next = POS_CHECK(idx,dir);
      if (!visited[next] && game->Board[next].color == color) return 1;
      else if (!visited[next] && game->Board[next].color == EMPTY) queue[tail++] = next;
      }
    }
  }
  return 0;
} 

int eval_winner(Game* game, float* points_b, float* points_w){
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
  *points_b = (float) b_cnt;
  *points_w = (float) w_cnt + game->komi ;
  return ((float) b_cnt > (float) w_cnt + game->komi);
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
      int winner = game_eval(game, &points_b, &points_b);

      if (winner == 1)      printf("BLACK won with %d against %d points\n",points_b,points_w);
      else if (winner == 0) printf("WHITE won with %d against %d points\n",points_w,points_b);
      else if (winner == -1) {
        printf("PASS invalid move because cant eval");
        continue;
      }
    return winner;
    }
  }
  return -1;
}

int game_init(Game* game){
  game->turn = BLACK;
  game->koPos = -1;
  for (int i = 0 ; i != MAX_CHAIN_LIST_SIZE; i++){
    game->chains[i] = (Chain) {-1,-1,-1,-1};
  }
  for (int i = 0 ; i != ; i++){
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

 int game_eval(Game* game, float* points_b, float* points_w){
    #ifdef PRINT_BOARD
      print_board(game);
    #endif
    #ifdef DEBUG_BOARD
      print_debug(game);
    #endif
  return eval_winner(game,points_b,points_w);
}

