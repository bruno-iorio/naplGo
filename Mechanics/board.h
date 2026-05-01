#ifndef BOARD_H
#define BOARD_H
#define BOARDSIZE 21
#define EMPTY 0
#define BLACK 1
#define WHITE 2
#define WALL 3



typedef struct board{
  int** grid;
  int** groups;
  int turn; //1 if black, 2 if white
} board;

int play_move(board*, int,int,int);
int play_match(board*);
int capture_group(board*, int*, int*, int);
void print_board(board*);
board* create_board();
void print_grid(int**, int);
int compare_boards(int**,int**, int);
void copy_board(int**,int**,int);
int find_group_liberties(board*,int*,int*,int,int,int*);
#endif // BOARD_H
