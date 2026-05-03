#include <stdlib.h>
#include<stdio.h>
#include <string.h>

#define DEFAULT_SIZE          19
#define BOARD_SIZE(x)         x*x
#define MAX_CHAIN_LIST_SIZE   300
#define MAX_LIBERTY_LIST_SIZE 300

#define LEFT                  0
#define RIGHT                 1
#define UP                    2
#define DOWN                  3

#define POS(x,y)              y * DEFAULT_SIZE + x
#define POS_LEFT(pos)         pos - 1
#define POS_RIGHT(pos)        pos + 1
#define POS_UP(pos)           pos - DEFAULT_SIZE
#define POS_DOWN(pos)         pos + DEFAULT_SIZE

int POS_CHECK(pos, dir)  {
    if (dir == UP)    return POS_UP(pos);
    if (dir == DOWN)  return POS_DOWN(pos);
    if (dir == LEFT)  return POS_LEFT(pos);
    if (dir == RIGHT) return POS_RIGHT(pos);
  }

int INBOUNDS_CHECK(pos, dir) {
    if (dir == UP && pos / DEFAULT_SIZE == 0                  ||
        dir == DOWN && pos / DEFAULT_SIZE == DEFAULT_SIZE-1   ||
        dir == LEFT && pos / DEFAULT_SIZE == 0                ||
        dir == RIGHT && pos / DEFAULT_SIZE == DEFAULT_SIZE-1) return 1;
      return 0;
}
#define EMPTY                 0
#define BLACK                 1
#define WHITE                 2

// chains inspired from katago engine

typedef struct{
  int Id;
  int color;
  int head;
  int libertyCount;
} Chain;

typedef struct{
  int color;
  int chainId;
  int nextPos;
} Node;

typedef struct{
  Node Board[BOARD_SIZE(DEFAULT_SIZE)];
  Chain chains[MAX_CHAIN_LIST_SIZE];
  int chainCount;
  int turn;
  int ko_pos;
} Game;


int markLiberties(Game *game, int pos, int *mark[BOARD_SIZE(DEFAULT_SIZE)]){
  int cnt = 0;
  for (int dir = 0; dir != 4; dir++){
    int pos_check = POS_CHECK(pos,dir);
    if (INBOUNDS_CHECK(pos,dir) && game->Board[pos_check].color == EMPTY)  {
      if (*mark[pos_check] == 0) cnt++;
      *mark[pos_check] = 1;
    }
  }
  return cnt
}

int mergeChain(Game *game, int to_check_list[4], int pos, int chain_id, int captured_chains[4]){
  // check if new chain and other chains need merge, if yes, merge them and update libertyList
  // Otherwise add new chain to chain list and update chain Count and libertiesList of the old chain;
  int mark[BOARD_SIZE(DEFAULT_SIZE)];
  int cap_cnt = 0;
  int lib_cnt = 0;
  for (int dir = 0 ; dir < 4; dir++){
    int pos_check = POS_CHECK(pos);
    if (INBOUNDS_CHECK(pos,dir) && game->Board[pos_check].color == game->Board[pos].color){
      // if valid and merging is necessary pos becomes the head and chain_id changes
      int search = pos;
      chain_id = game->Board[pos_check].chainId;
      while (game->Board[search].nextPos  !=  -1) {
        // update lib_cnt , update the chain of search,  update search.
        lib_cnt += markLiberties(&game, search, &mark);
        game->Board[search].chainId = chain_id;
        search = game->Board[search].nextPos;
      }
      // process the last one
      lib_cnt += markLiberties(&game, search, &mark);
      game->Board[search].chainId = chain_id;
      
      // merge: 
      gane->Board[search].next = game->chains[chain_id].head;
      game->chains[chainId].head = pos;
    }
      if (INBOUNDS_CHECK(pos, dir) && game->Board[pos_check].color != game->Board[pos].color && game->Board[pos_check].color != EMPTY){ 
        // check if captured
        if(--(game->chains[game->Board[pos_check].chainId].libertiesCount) == 0){
          captured_chains[cap_cnt++] = game->Board[pos_check].chainId;
        }
      }
    }
  game->chain[chain_id].libertiesCount = lib_cnt;
  return chain_id;
}

void handle_captures(Game* game, int captured_chains[4], int color){
  int cap_cnt = 0;
  int last_captured;
  for (int i = 0 ; i != 4 ; i++){
    if(captured_chains[i] != -1){
      int pos = game->chains[captured_chains[i]].head;
      while (game->Board[pos].next != -1){
        int pos_check = POS_CHECK(pos,dir);
        for (int dir = 0; dir != 4 ; dir++){
          if (INBOUNDS_CHECK(pos,dir) && game->Board[pos_check].color != color && game->Board[pos_check].color != EMPTY) 
            game->chains[captured_chains[i]].libertiesCount--;
        }
        game->Board[pos].color = EMPTY;
        cap_cnt++;
        last_captured = pos;
      }
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
  int coord = POS(x,y);
  if (0 <= x && x < DEFAULT_SIZE && 0 <= y  && y <  DEFAULT_SIZE && game->Board[coord].color == EMPTY){
    game->Board[coord].color = game->turn;

    Chain newChain;
    memset(&newChain, 0, sizeof(newChain));
    newChain.color = game->turn;
    newChain.head  = coord;
    newChain.Id    = chainCount;

    game->Board[coord].chainId = chainCount;
    game->Board[coord].newPos  = -1; // points to nothing

    for (int dir = 0 ; dir < 4; dir++){
      if (INBOUNDS_CHECK(pos,dir) && game->Board[POS_CHECK(dir)].color == EMPTY ) newChain.libertiesCount++;
    }
    
    int captured_chains[4] = {-1,-1,-1,-1};
    if (newChain.id == mergeChain(&game, to_check_list, pos, newChain.id, &captured_chains){
      game->chains[newChain.id] = newChain;
    }
    handle_captures(&game,captured_chains, game->turn);
    game->turn = (game->turn == WHITE) ? BLACK : WHITE;
  }
  else fprintf(stderr, "INVALID COORDS!! \n");
}

void gameloop(Game* game){
  while(1){
    int x,y;
    if 
    printf("Play your move:");
    scanf("%d %d",&x,&y);
    printf("\n");
    play(game, x, y);
    for (int i = 0; i != DEFAULT_SIZE; i++){
      for (int j = 0; j != DEFAULT_SIZE ; j++){
        printf("%d ", game->b.grid[i][j]);
      }
      printf("\n");
    }
  }
  return;
}

int main(){
  Game game;
  memset(&game,0, sizeof(game));
  gameloop(&game);
  return 0;
}
