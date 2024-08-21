#include "board.h"
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>

board* create_board(void){
  board* Board = (board*) malloc(sizeof(board*));
  Board->grid = (int**) malloc(BOARDSIZE*sizeof(int*));

  for(int i = 0; i < BOARDSIZE; i++){
    Board->grid[i] = (int *) malloc(BOARDSIZE*sizeof(int));
  }
  for(int i = 0; i <  BOARDSIZE; i++){
    for(int j = 0; j <  BOARDSIZE; j++){
      if (i == 0 || i == BOARDSIZE - 1 || j == 0 || j == BOARDSIZE-1){
        Board->grid[i][j] = WALL;
      } else {
        Board->grid[i][j] = EMPTY;
      }
    }
  }
  return Board;
}

void print_board(board* Board){
  for(int i = 0; i < BOARDSIZE; ++i){
    for(int j = 0; j < BOARDSIZE; ++j){
      printf("%d ",Board->grid[i][j]);
    }
    printf("\n");
  }
}
void print_grid(int** grid){
for(int i = 0; i < BOARDSIZE; ++i){
    for(int j = 0; j < BOARDSIZE; ++j){
      printf("%d ",grid[i][j]);
    }
    printf("\n");
  }

}

int play_move(board* Board,int color, int x, int y){
  if(x <= 0 || x >= BOARDSIZE-1 || y <= 0 || y >= BOARDSIZE-1){
    return -1; //error
  }
  Board->grid[x][y] = color;
  return 0;
}

int capture_group(board* Board, int* x, int* y, int n){
  if(Board==NULL || x==NULL || y==NULL){return -1;}
  int i = 0;
  for (i = 0;i<=n;i++) {
    Board->grid[*(x+i)][*(y+i)] = EMPTY;
  }
  return 0;
}

int find_group_liberties(board* Board,int* x, int* y,int x1,int y1, int* n){
  if(x1 <= 0 || x1 >= BOARDSIZE-1 || y1 <= 0 || y1 >= BOARDSIZE-1){
   return -1; //error 
  }
  int color = Board->grid[x1][y1];
  int color1,color2,color3,color4;
  color1 = Board->grid[x1+1][y1];
  color2 = Board->grid[x1-1][y1];
  color3 = Board->grid[x1][y1+1];
  color4 = Board->grid[x1][y1-1];

  *(x + *n) = x1;
  *(y + *n) = y1;
  if(color1 == EMPTY || color2 == EMPTY || color3 == EMPTY || color4 == EMPTY){return 0;}
  
  if(color1 == color){
    int found = 0;
    for(int i = 0; i < *n; i++){
      if(x1 + 1 == *(x + i) && y1 == *(y + i)){found = 1; break;}
    }
    if(!found){
      (*n)++;
      if(find_group_liberties(Board,x,y,x1+1,y1,n) == 0){return 0;}
    }
  }
  if(color2 == color){
    int found = 0;
    for(int i = 0; i < *n; i++){
      if(x1 - 1 == *(x + i) && y1 == *(y + i)){found = 1; break;}
    } 
    if(!found){
      (*n)++;
      if(find_group_liberties(Board,x,y,x1-1,y1,n) == 0){ return 0;};
    }
  }
  if(color3 == color){
    int found = 0;
    for(int i = 0; i < *n; i++){
      if(x1 == *(x + i) && y1 + 1 == *(y + i)){found = 1;break;}
    } 
    if(!found){
      (*n)++;
      if(find_group_liberties(Board,x,y,x1,y1+1,n)==0){return 0;}
    }
  }
  if(color4 == color){
    int found = 0;
    for(int i = 0; i < *n; i++){
      if(x1 == *(x + i) && y1 - 1 == *(y + i)){found = 1; break;}
    } 
    if(!found){
      (*n)++;
      if(find_group_liberties(Board,x,y,x1,y1-1,n)==0){return 0;}
    }
  }
  return 1; //dead
}


int compare_boards(int** grid1,int** grid2, int n){
  int not_equal = 0;
  for(int i = 0; i < n; i++){
    for(int j = 0; j < n; j++){
      if(grid1[i][j] != grid2[i][j]){
        not_equal = 1;
        break;
      }
    }
  }
  return not_equal;
}
void copy_board(int** grid2, int** grid1, int n){
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      grid2[i][j] = grid1[i][j];
  }
}
}

int play_move_out_of_match(board* Board,grid* copy1, grid* copy2, int* turn,int *pass_count,
                           int *count,int x, int y,int* Ko_check){
  if(x == -1 && y == -1){
      (*pass_count)++;
      (*count)++;
      if(*pass_count == 2){
        return 1; //game over;
    }
    (*count)++;
    *turn = (*turn == BLACK) ? WHITE : BLACK; 
    return 0;
    }
    if(x <= 0 || x >= BOARDSIZE-1 || y <= 0 || y >= BOARDSIZE-1){
      printf("Invalid input\n");
    return 5; // input out of board.
    }

  if(Board->grid[x][y] ==EMPTY){
    play_move(Board,*turn,x,y);
    int n[5];
    int group_x[5][361], group_y[5][361]; 
    for (int i = 0; i < 5; i++){n[i] = 0;}
    int color1,color2,color3,color4;
    color1 = Board->grid[x+1][y];
    int captured_flag = 0;
    if(color1 != *turn && color1 != WALL && color1 != EMPTY){
      if(find_group_liberties(Board,group_x[1],group_y[1],x+1,y,&n[1]) == 1){
        capture_group(Board,group_x[1],group_y[1],n[1]);
        captured_flag = 1;
      }
    }
    color2 = Board->grid[x-1][y];
    if(color2 != *turn && color2 != WALL && color2 != EMPTY){
      if(find_group_liberties(Board,group_x[2],group_y[2],x-1,y,&n[2]) == 1){
        capture_group(Board,group_x[2],group_y[2],n[2]);
        captured_flag = 1;
      }
    }
    color3 = Board->grid[x][y+1];
    if(color3 != *turn && color3 != WALL && color3 != EMPTY){
      if(find_group_liberties(Board,group_x[3],group_y[3],x,y+1,&n[3]) == 1){
        capture_group(Board,group_x[3],group_y[3],n[3]);
        captured_flag = 1;
      }
    }
    color4 = Board->grid[x][y-1];
    if(color4 != *turn && color4 != WALL && color4 != EMPTY){
      if(find_group_liberties(Board,group_x[4],group_y[4],x,y-1,&n[4]) == 1){
        capture_group(Board,group_x[4],group_y[4],n[4]);
        captured_flag = 1;
      }
    }
    if(find_group_liberties(Board,group_x[0],group_x[0],x,y,&n[0]) == 1){
      printf("invalid move\n");
      play_move(Board,EMPTY,x,y);
      return 3; //suicide without caputre;
    }
    if(*Ko_check == 1 && captured_flag == 1){  
      if(!compare_boards(Board->grid,copy2->tab,BOARDSIZE)&& *count > 1){
        copy_board(Board->grid,copy1->tab,BOARDSIZE);
        printf("invalid move: ko\n");
        return 2; //invlid for ko
      }
    } 
    *pass_count = 0;
    *Ko_check = (captured_flag == 1) ? 1 : 0;
    *turn = (*turn == BLACK) ? WHITE : BLACK ; 
    copy_board(copy2->tab,copy1->tab,BOARDSIZE);
    copy_board(copy1->tab,Board->grid,BOARDSIZE);
    (*count)++;
  }
  return 0;
}

void list_possible_moves(int** x_list,int** y_list,int* numb, board* Board,grid* copy1,
                         grid* copy2, int turn,int count,int Ko_check,int* cap){
  
  for(int x = 1; x < BOARDSIZE-1; x++){
    for(int y = 1; y < BOARDSIZE-1; y++){
      if(*numb >= *cap){
        *cap = 2*(*cap);
        *x_list =(int*) realloc(*x_list,sizeof(int)*(*cap));
        *y_list =(int*) realloc(*y_list,sizeof(int)*(*cap));
      }
      if(Board->grid[x][y] ==EMPTY){
      play_move(Board,turn,x,y);

      int n[5];
      int group_x[5][361], group_y[5][361]; 

      for (int i = 0; i < 5; i++){n[i] = 0;}
 
      int color1,color2,color3,color4;
 
      color1 = Board->grid[x+1][y];
      int captured_flag = 0;

      if(color1 != turn && color1 != WALL && color1 != EMPTY){
        if(find_group_liberties(Board,group_x[1],group_y[1],x+1,y,&n[1]) == 1){
          capture_group(Board,group_x[1],group_y[1],n[1]);
          captured_flag = 1;
        }
      }
      color2 = Board->grid[x-1][y];
      if(color2 != turn && color2 != WALL && color2 != EMPTY){
        if(find_group_liberties(Board,group_x[2],group_y[2],x-1,y,&n[2]) == 1){
          capture_group(Board,group_x[2],group_y[2],n[2]);
          captured_flag = 1;
        }
      }
      color3 = Board->grid[x][y+1];
      if(color3 != turn && color3 != WALL && color3 != EMPTY){
        if(find_group_liberties(Board,group_x[3],group_y[3],x,y+1,&n[3]) == 1){
          capture_group(Board,group_x[3],group_y[3],n[3]);
          captured_flag = 1;
        }
      }
      color4 = Board->grid[x][y-1];
      if(color4 != turn && color4 != WALL && color4 != EMPTY){
        if(find_group_liberties(Board,group_x[4],group_y[4],x,y-1,&n[4]) == 1){
          capture_group(Board,group_x[4],group_y[4],n[4]);
          captured_flag = 1;
        }
      }
      if(find_group_liberties(Board,group_x[0],group_x[0],x,y,&n[0]) == 1){
        play_move(Board,EMPTY,x,y);
        continue;
      }
      if(Ko_check == 1 && captured_flag == 1){  
        if(!compare_boards(Board->grid,copy2->tab,BOARDSIZE)&& count > 1){
          copy_board(Board->grid,copy1->tab,BOARDSIZE);
          continue;
       //invlid for ko
        }
      } 
    play_move(Board,EMPTY,x,y); //remove the initial move
    (*x_list)[*numb] = x;
    (*y_list)[*numb] = y;
    (*numb)++;
      } 
    }
  }
}

int count_points(board* Board){ //only for games that where finished
  int black_points = 0, white_points = 0;
  for(int x = 1; x<BOARDSIZE-1;x++){
    for(int y = 1; y<BOARDSIZE-1;y++){
      if (Board->grid[x][y] == BLACK){
        black_points++;
      }
      else if (Board->grid[x][y] == WHITE){
        white_points++;
      }
      else{
        if(Board->grid[x+1][y] != WHITE &&
           Board->grid[x-1][y] != WHITE &&
           Board->grid[x][y-1] != WHITE &&
           Board->grid[x][y+1] != WHITE){
           black_points++;
        }
        if(Board->grid[x+1][y] != BLACK &&
           Board->grid[x-1][y] != BLACK &&
           Board->grid[x][y-1] != BLACK &&
           Board->grid[x][y+1] != BLACK){
           white_points++;
        }

      }
    }
  }

return (white_points + 1 > black_points);

}

