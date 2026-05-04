#ifndef BOARD_H
#define BOARD_H

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

#define EMPTY                 0
#define BLACK                 1
#define WHITE                 2


/* Data Structures */

typedef struct{
  int id;
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

/* function signatures*/

int POS_CHECK(int pos, int dir);
int INBOUNDS_CHECK(int pos, int dir);

int markLiberties(Game* game, int pos ,int* mark);
int mergeChain(Game* game, int pos, Chain newChain, int* captured_chains);
void handle_captures(Game* game, int* captured_chains, int color);
void play(Game* game, int x, int y );
void print_rules();
void gameloop(Game* game);

#endif // BOARD_H
