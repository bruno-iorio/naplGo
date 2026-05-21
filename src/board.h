#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

// important MACROS

#define DEFAULT_SIZE          19

#define BOARD_SIZE            (DEFAULT_SIZE * DEFAULT_SIZE)
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

#define PASS                  BOARD_SIZE

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
  int capturesBlack;
  int capturesWhite;
  
  int moveCount;
  zobristEncoding state;
} Game;


extern zobristEncoding random_table[2*BOARD_SIZE + 1];

/* POS aux functions*/
static inline int POS_CHECK(int pos, int dir) {
  // returns neighbor of pos at direction dir

  if (dir == UP)    return POS_UP(pos);
  if (dir == DOWN)  return POS_DOWN(pos);
  if (dir == LEFT)  return POS_LEFT(pos);
  if (dir == RIGHT) return POS_RIGHT(pos);
  return -1;
}


static inline int INBOUNDS_CHECK(int pos, int dir) {
  // check if neighbors of pos at direction dir from pos is
  // inside of the Board

  if ((dir == UP    && pos / DEFAULT_SIZE == 0)                 ||
      (dir == DOWN  && pos / DEFAULT_SIZE == DEFAULT_SIZE - 1)  ||
      (dir == LEFT  && pos % DEFAULT_SIZE == 0)                 ||
      (dir == RIGHT && pos % DEFAULT_SIZE == DEFAULT_SIZE- 1) ) return 0;
  return 1;
}

/* game logic callbacks */
int captured_exists(int captured_chains[4]);
int mark_liberties(Game* game, int pos ,int* mark);
int merge_chain(Game* game, int pos, Chain newChain); 
int detect_captures(Game* game, int pos, int* captured_chains);
void handle_captures(Game* game, int* captured_chains);
void update_chains_size(Game* game);
void add_stone(Game* game, int pos);
int remove_stone(Game* game, int pos);

/* info */
void print_rules();
void print_board(Game* game);
void print_debug(Game* game);

/* main functions */
int game_loop(Game* game);
void game_init(Game* game);
int game_play(Game* game); 
int game_eval(Game* game, float* points_b, float* points_w);
int play(Game* game, int x, int y);
int play_pos(Game* game, int pos);

/* counters */
int touch_color(const Game* game, int pos, int color);

/* hashing */
void init_zobrist();
void hash(zobristEncoding* state, int pos, int color);

/* move quality */
int get_legal_moves(Game* game, int* mark);
int get_good_moves(Game* game, int* good, int* legal_moves, int legal_len);
int choose_random_move(Game* game, int* moves, int moves_len);
int is_move_legal(Game* game, int pos);
int is_move_self_eye(Game* game, int pos);

#endif // BOARD_H
