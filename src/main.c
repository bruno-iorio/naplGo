#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "board.h"
#include "mcts.h"
#include "sgfwriter.h"

void test_board(){
  Game game;
  memset(&game,0, sizeof(game));
  
  int winner = game_loop(&game);
}

void test_sgf(){
  write_input_from_sgf("../sgf_test_files/Ueno_x_yashiro_2020.sgf", "out.txt");
}

void test_zobrist(){
  Game game, game2;
  game_init(&game);
  game_init(&game2);
  

  play_pos(&game,POS(0,1));
  play_pos(&game,POS(10,1));
  play_pos(&game,POS(5,4));
  play_pos(&game,POS(5,9));
  play_pos(&game,POS(8,8));
  play_pos(&game,POS(3,2));

  print_board(&game);
  printf("%" PRIu64 "\n", game.state);

  play_pos(&game2,POS(8,8));
  play_pos(&game2,POS(5,9));
  play_pos(&game2,POS(5,4));
  play_pos(&game2,POS(10,1));
  play_pos(&game2,POS(0,1));
  play_pos(&game2,POS(3,2));
  
  print_board(&game);
  printf("%" PRIu64 "\n", game2.state);
}

void test_good_moves(){
  Game game2;
  game_init(&game2);

  play_pos(&game2,POS(0,0));

  play_pos(&game2,POS(18,18));
  
  play_pos(&game2,POS(1,1));

  play_pos(&game2,POS(16,16));

  play_pos(&game2,POS(2,0));
 
  play_pos(&game2,POS(10,1));
  

  print_board(&game2);

  int moves[BOARD_SIZE], good[BOARD_SIZE];
  int legal_len , good_len;

  legal_len = get_legal_moves(&game2 , moves);
  good_len  = get_good_moves(&game2, good, moves, legal_len);
  


  printf("%d %d \n" , good_len, legal_len);
  for (int i = 0; i != BOARD_SIZE; i++){
    printf("%d ", good[i]);
  }
}

int main(int argc, char *argv[]){
  init_zobrist();
  init_neighbors();
  //test_sgf();
  //test_board();
  //test_good_moves();
  //test_zobrist();
  test_mcts();
 return 0;
}
