#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "sgfwriter.h"



void write_input_from_sgf(const char* in, const char* out){
  FILE* f = fopen(in,"r");
  if(!f){
    fprintf(stderr, "%could not open file\n");
    exit(1);
  }
  int moves[MAX_MOVES];
  int curr_move = 0;
  char c = fgetc(f);
  int isMove = 0;
  int turn = BLACK;
  int wontbeMove=0;
  while (c != EOF){
    switch (c) {
      case 'B': 
        isMove = 1;
        turn = BLACK;
        c = fgetc(f);
        if (c != '[') isMove = 0;
        break;
      case 'W': 
        isMove = 1;
        turn = WHITE;
        c = fgetc(f);
        if (c != '[') isMove = 0;
        break;
      case ']':
        isMove = 0;
        wontbeMove = 0;
      default:
        if (c >=65 && c <= 90) wontbeMove = 1;
        if (!wontbeMove && isMove){
          if (c >= 97 && c < 123){

            int x = (int) (c - 97);
            moves[curr_move] = x;
            c = fgetc(f); 

            if (c < 97 || c >= 123){
              fprintf(stderr,"1: wrong sgf format!\n");
              fclose(f);
              exit(1);
            }

            int y = (int) (c - 97);
            moves[curr_move+1] = y;
            curr_move += 2;
          }
          else {
            fprintf(stderr,"2: wrong sgf format!\n");
            fclose(f);
            exit(1);
          }
        }
        break;
    }
    c = fgetc(f);
  }
  fclose(f);
  FILE *f_out = fopen(out, "w");
  if (!f_out){
    fprintf(stderr, "Issue writing new file");
    exit(1);
  }
  for (int i = 0; i != curr_move ; i++){
    if (i % 2 == 0)      fprintf(f_out, "%d ", moves[i]);
    else if (i % 2 == 1) fprintf(f_out, "%d\n", moves[i]);
  }

  fprintf(f_out, "-1 -1\n");
  fclose(f_out);
  return;
}

void write_move(const char* filename, int pos, int turn){
  FILE *f = fopen(filename, "w");
  if (!f){
    fprintf(stderr, "Could not open file");
    exit(1);
  }

  int x = pos % DEFAULT_SIZE;
  int y = pos / DEFAULT_SIZE;
  char c = (turn == BLACK) ? 'B' : 'W';
  fprintf(f,";%c[%c%c]",c, x + 97, y + 97);
  fclose(f);
  return;
}
/*

void write_game(const char* filename, const Game* game){
  FILE *f = fopen(filename, "w");
  if (!f){
    fprintf(stderr, "Could not open file");
    exit(1);
  }
  fprintf(f, "(;") ;
  for (int i = 0; i != game->moveCount; i++) {
    int pos = game->moves[i];
    int x = pos % DEFAULT_SIZE;
    int y = pos / DEFAULT_SIZE;
    char c = (i % 2) ? 'B' : 'W';
    fprintf(f,";%c[%c%c]",c, x + 97, y + 97);
  }

  fprintf(f, ")");
  fclose(f);
  return;
}

*/
