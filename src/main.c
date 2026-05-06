#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "sgfwriter.h"

void test_board(){
  Game game;
  memset(&game,0, sizeof(game));
  int winner = game_loop(&game);
}

void test_sgf(){
  write_input_from_sgf("../sgf_test_files/Ueno_x_yashiro_2020.sgf", "out.txt");
}

int main(int argc, char *argv[]){
  //test_sgf();
  init_zobrist();
  test_board();
 return 0;
}
