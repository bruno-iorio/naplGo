#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

// important MACROS

#define DEFAULT_SIZE          19

#define BOARD_SIZE            (DEFAULT_SIZE) * (DEFAULT_SIZE)
#define MAX_CHAIN_LIST_SIZE   BOARD_SIZE
#define MAX_LIBERTY_LIST_SIZE 130 
#define MAX_MOVES             600
#define UPDATE_CHAIN_DELAY    10
#define DEFAULT_KOMI          7.5

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

typedef uint64_t zobristEncoding; 

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
  Node Board[BOARD_SIZE];
  Chain chains[MAX_CHAIN_LIST_SIZE];
  int chainCount;
  int turn;
  int koPos;
  int pass;
  float komi;
  zobristEncoding state;
} Game;

extern zobristEncoding random_table[2*BOARD_SIZE + 2];

/* POS aux functions*/
int POS_CHECK(int pos, int dir);
int INBOUNDS_CHECK(int pos, int dir);

/* game logic callbacks */
int captured_exists(int captured_chains[4]);
int mark_liberties(Game* game, int pos ,int* mark);
int merge_chain(Game* game, int pos, Chain newChain); 
int detect_captures(Game* game, int pos, int* captured_chains);
void handle_captures(Game* game, int* captured_chains);
void update_chains_size(Game* game);
void add_stone(Game* game, int pos);
void remove_stone(Game* game, int* pos);

/* info */
void print_rules();
void print_debug(Game* game);

/* main functions */
int game_loop(Game* game);
int game_init(Game* game);
int game_play(Game* game); 
int game_eval(Game* game, float* points_b, float* points_w);

int play(Game* game, int x, int y);
int play_pos(Game* game, int pos);

int eval_winner(Game* game, float* points_b, float* points_w);
int touch_color(const Game* game, int pos, int color);

/* hashing */
void init_zobrist();
void hash(zobristEncoding* state, int pos, int color);

int find_legal_moves(Game* game, int* mark);
int is_move_legal(Game* game, int pos);
int is_move_self_eye(Game* game, int pos);

#endif // BOARD_H
