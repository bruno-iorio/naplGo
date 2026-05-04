#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "sgfwriter.h"

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
