#ifndef BOARD_H
#define BOARD_H
#define BOARDSIZE 21
#define EMPTY 0
#define BLACK 1
#define WHITE 2
#define WALL 3



typedef struct board{
  int** grid;
} board;

typedef struct grid {
  int** tab;
} grid;

int play_move_out_of_match(board*,grid*,grid*,int*,int*,int*,int, int,int*);
void list_possible_moves(int**,int**,int*,board*,grid*,grid*,int,int,int,int*);
int play_move(board*, int,int,int);
int capture_group(board*, int*, int*, int);
void print_board(board*);
board* create_board();
void print_grid(int**);
int compare_boards(int**,int**, int);
void copy_board(int**,int**,int);
int find_group_liberties(board*,int*,int*,int,int,int*);
#endif // BOARD_H
