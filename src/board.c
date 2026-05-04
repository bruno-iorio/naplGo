#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "board.h"
#include "sgfwriter.h"


int POS_CHECK(int pos, int dir)  {
    if (dir == UP)    return POS_UP(pos);
    if (dir == DOWN)  return POS_DOWN(pos);
    if (dir == LEFT)  return POS_LEFT(pos);
    if (dir == RIGHT) return POS_RIGHT(pos);
}

int INBOUNDS_CHECK(int pos, int dir) {
    if ((dir == UP && pos / DEFAULT_SIZE == 0)                    ||
        (dir == DOWN && pos / DEFAULT_SIZE == DEFAULT_SIZE - 1)   ||
        (dir == LEFT && pos % DEFAULT_SIZE == 0)                  ||
        (dir == RIGHT && pos % DEFAULT_SIZE == DEFAULT_SIZE- 1) ) return 0;
      return 1;
}

int markLiberties(Game *game, int pos, int* mark){
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

int mergeChain(Game *game, int pos, Chain newChain, int* captured_chains){
  // check if new chain and other chains need merge, if yes, merge them and update libertyList
  // Otherwise add new chain to chain list and update chain Count and libertiesList of the old chain;

  int chain_id =  newChain.id;
  int mark[BOARD_SIZE(DEFAULT_SIZE)];
  memset(mark, 0, sizeof(mark));
  
  int cap_cnt = 0;
  int lib_cnt = 0;

  for (int dir = 0 ; dir < 4; dir++){
    int pos_check = POS_CHECK(pos,dir);
    if (INBOUNDS_CHECK(pos,dir) && game->Board[pos_check].color == game->Board[pos].color){
      // if valid and merging is necessary pos becomes the head and chain_id changes
      int search = pos;
      chain_id = game->Board[pos_check].chainId;
 
      while (game->Board[search].nextPos  !=  -1) {
        // update lib_cnt , update the chain of search,  update search.
        lib_cnt += markLiberties(game, search, mark);
        game->Board[search].chainId = chain_id;
        search = game->Board[search].nextPos;
      }

      // process the last one
      lib_cnt += markLiberties(game, search, mark);
      game->Board[search].chainId = chain_id;

      // merge:
      game->Board[search].nextPos = game->chains[chain_id].head;
      search = game->chains[chain_id].head; // now will add the remaining liberties
      game->chains[chain_id].head = pos;

      while (game->Board[search].nextPos  !=  -1) {
        lib_cnt += markLiberties(game, search, mark);
        search = game->Board[search].nextPos;
      }
      lib_cnt += markLiberties(game, search, mark);
    }
    if (INBOUNDS_CHECK(pos, dir) && game->Board[pos_check].color != game->Board[pos].color && game->Board[pos_check].color != EMPTY){ 
      (game->chains[game->Board[pos_check].chainId].libertyCount)--;
      if(game->chains[game->Board[pos_check].chainId].libertyCount == 0){
        captured_chains[cap_cnt++] = game->Board[pos_check].chainId;
      }
    }
  }
  game->chains[chain_id].libertyCount = lib_cnt;
  return chain_id;
}

void handle_captures(Game* game, int* captured_chains, int color){
  int cap_cnt = 0;
  int last_captured;
  for (int i = 0 ; i != 4 ; i++){
    if(captured_chains[i] != -1){
      int pos = game->chains[captured_chains[i]].head;
      while (game->Board[pos].nextPos != -1){
        for (int dir = 0; dir != 4 ; dir++){
          int pos_check = POS_CHECK(pos,dir);
          if (INBOUNDS_CHECK(pos,dir) && game->Board[pos_check].color != color && game->Board[pos_check].color != EMPTY) 
            game->chains[captured_chains[i]].libertyCount--;
        }
        game->Board[pos] = (Node) {EMPTY,-1,-1};
        cap_cnt++;
        last_captured = pos;
      }
      for (int dir = 0; dir != 4 ; dir++){
        int pos_check = POS_CHECK(pos,dir);
        if (INBOUNDS_CHECK(pos,dir) && game->Board[pos_check].color != color && game->Board[pos_check].color != EMPTY) 
          game->chains[captured_chains[i]].libertyCount--;
      }
      game->Board[pos] = (Node) {EMPTY,-1,-1};
      cap_cnt++;
      last_captured = pos;
    }
  }
  if (cap_cnt == 1) { //activate ko_flags
    game->ko_pos = last_captured;
  }
  else {
    game->ko_pos = -1;
  }
}

void play(Game *game, int x, int y){ 
  int pos = POS(x,y);
  if (0 <= x && x < DEFAULT_SIZE && 0 <= y  && y <  DEFAULT_SIZE && game->Board[pos].color == EMPTY && pos != game->ko_pos){
    game->Board[pos].color = game->turn;

    Chain newChain;
    memset(&newChain, 0, sizeof(newChain));
    newChain.color        = game->turn;
    newChain.head         = pos;
    newChain.id           = game->chainCount;
    newChain.libertyCount = 0;

    game->Board[pos].chainId = newChain.id;

    for (int dir = 0 ; dir < 4; dir++){
      if (INBOUNDS_CHECK(pos,dir) && game->Board[POS_CHECK(pos,dir)].color == EMPTY ) 
        newChain.libertyCount++;
    }
    // a move can captures at most 4 chains at a time (each cardinal direction)
    int captured_chains[4] = {-1,-1,-1,-1};
    if (newChain.id == mergeChain(game, pos, newChain, captured_chains)){
      game->chains[newChain.id] = newChain;
    }
    else game->chainCount--;
    handle_captures(game, captured_chains, game->turn);
    game->turn = (game->turn == WHITE) ? BLACK : WHITE;
    game->chainCount++;
    game->moves[game->moveCount++] = pos;
  }
  else fprintf(stderr, "INVALID COORDS!! \n");
}

void print_rules(){
  printf("Insert your moves as a pair \"x y\" of integers\n");
  return;
}

void gameloop(Game* game){
  // pre-setup :
  print_rules();
  game->turn = BLACK;
  memset(game->chains, -1, sizeof(game->chains));
  for (int i = 0; i != DEFAULT_SIZE; i++){
    for (int j = 0; j != DEFAULT_SIZE ; j++){
      game->Board[POS(i,j)].nextPos = -1;
      game->Board[POS(i,j)].chainId = -1;
    }
  }

  while(1){
    int x,y;
    printf("Play your move: ");
    scanf("%d %d",&x,&y);
    printf("\n");
    play(game, x, y);
    
    printf("color: \n");
    for (int j = 0; j != DEFAULT_SIZE ; j++){
      for (int i = 0; i != DEFAULT_SIZE; i++){
        printf("%d ", game->Board[POS(i, j)].color);
      }
      printf("\n");
    }

    printf("chainId: \n");
    for (int j = 0; j != DEFAULT_SIZE ; j++){
      for (int i = 0; i != DEFAULT_SIZE; i++){
        printf("%d ", game->Board[POS(i, j)].chainId);
      }
      printf("\n");
    }

    printf("nextPos: \n");
    for (int j = 0; j != DEFAULT_SIZE ; j++){
      for (int i = 0; i != DEFAULT_SIZE; i++){
        printf("%d ", game->Board[POS(i, j)].nextPos);
      }
      printf("\n");
    }
  for (int i = 0; i < game->chainCount; i++){
      printf("chain: %d ; liberties: %d ; color %d ; head: %d \n",i,game->chains[i].libertyCount, game->chains[i].color, game->chains[i].head);
    }
  }
  return;
}


