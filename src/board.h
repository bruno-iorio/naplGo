#ifndef BOARD_H
#define BOARD_H

// important MACROS

#define DEFAULT_SIZE          19
#define BOARD_SIZE(x)         x*x
#define MAX_CHAIN_LIST_SIZE   200
#define MAX_LIBERTY_LIST_SIZE 130 
#define MAX_MOVES             600
#define UPDATE_CHAIN_DELAY    10

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
  int moves[MAX_MOVES];
  int moveCount;
  int chainCount;
  int turn;
  int koPos;
} Game;


/* POS aux functions*/
int POS_CHECK(int pos, int dir);
int INBOUNDS_CHECK(int pos, int dir);


/* game logic callbacks */
int captured_exists(int captured_chains[4]);
int mark_liberties(Game* game, int pos ,int* mark);
int merge_chain(Game* game, int pos, Chain newChain, int* captured_chains);
void handle_captures(Game* game, int* captured_chains);
void update_chains_size(Game* game);

/* info */
void print_rules();
void print_debug(Game* game);


/* main functon */
void game_loop(Game* game);
int play(Game* game, int x, int y);

#endif // BOARD_H
