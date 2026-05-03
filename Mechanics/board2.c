#include <stdlib.h>
#include<stdio.h>
#include <string.h>

#define DEFAULT_SIZE          19
#define MAX_CHAIN_LIST_SIZE   300
#define MAX_LIBERTY_LIST_SIZE 300

#define POS(x,y)              y * DEFAULT_SIZE + x

#define EMPTY                 0
#define BLACK                 1
#define WHITE                 2

#define LEFT                  0
#define RIGHT                 1
#define UP                    2
#define DOWN                  3

// chains inspired from katago engine

typedef struct{
  int Id;
  int color;
  int head;
  int libertiesList[MAX_LIBERTY_LIST_SIZE];
  int libertyCount;
} Chain;

typedef struct{
  int color;
  int chainId;
  int nextPos;
} Node;

typedef struct{
  Node Board[DEFAULT_SIZE];
  Chain chains[MAX_CHAIN_LIST_SIZE];
  int chainCount;
  int turn;
  int ko_x, ko_y, ko_act;
} Game;

void mergeChain(Game *game, int to_check_list[4], Chain newChain, pos){
  // check if new chain and other chains need merge, if yes
  // merge them and update libertyList
  // Otherwise add new chain to chain list and update chain Count and libertiesList of the old chain;
  to_check_list[UP]    = (to_check_list[UP]    && game->Board[pos - DEFAULT_SIZE] == game->Board[pos]);
  to_check_list[DOWN]  = (to_check_list[DOWN]  && game->Board[pos + DEFAULT_SIZE] == game->Board[pos]);
  to_check_list[LEFT]  = (to_check_list[LEFT]  && game->Board[pos - 1] == game->Board[pos]);
  to_check_list[RIGHT] = (to_check_list[RIGHT] && game->Board[pos + 1] == game->Board[pos]);
  return;
}

void play(Game *game, int x, int y){ 
  if (0 <= x && x < DEFAULT_SIZE && 0 <= y  && y <  DEFAULT_SIZE){
    int coord = POS(x,y);
    game->Board[coord].color = game->turn;
    Chain newChain;
    memset(&newChain,0,sizeof(newChain));
    newChain.color = game->turn;
    newChain.head  = coord;
    newChain.Id    = chainCount;
    game->Board[coord].chainId = chainCount;
    game->Board[coord].newPos  = -1; // points to nothing

    int to_check_list[4] = (int) {1,1,1,1};
    if      (x == 0)                to_check_list[0] = 0;
    else if (x == DEFAULT_SIZE - 1) to_check_list[1] = 0;
    if      (y == 0)                to_check_list[2] = 0;
    else if (y == DEFAULT_SIZE - 1) to_check_list[3] = 0;

    mergeChain(&game, to_check_list, newChain, pos);

    game->turn = (game->turn == WHITE) ? BLACK : WHITE;
  }
  else fprintf(stderr, "Coordinates out of bounds!! \n");
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
