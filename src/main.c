#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "board.h"
#include "sgfwriter.h"

void test_board(){
  Game game;
  memset(&game,0, sizeof(game));
  gameloop(&game);
}

void test_sgf(){
}

int main(int argc, char *argv[]){
 return 0;
}
