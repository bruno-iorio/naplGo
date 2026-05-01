#include <stdlib.h>
#include<stdio.h>
#include <string.h>
#define DEFAULT_SIZE 19
#define BLACK 0
#define WHITE 1
#define MAX_QUEUE_SIZE 200

typedef struct{
  int n_up;
  int n_down;
  int n_left;
  int n_right;
} Node;

typedef struct{
  Node groups[DEFAULT_SIZE][DEFAULT_SIZE];
  int liberty_cnt[DEFAULT_SIZE][DEFAULT_SIZE];
  int grid[DEFAULT_SIZE][DEFAULT_SIZE]; 
} Board;

typedef struct{
  Board b;
  int turn;
  int ko;
} Game;
typedef struct{
  int x,y;
} Coord;
typedef struct{
  Coord data[MAX_QUEUE_SIZE];
  int top;
  int tail;
} Queue;

Coord pop(Queue* q){
  if (q->tail > q->top) {
    q->top++;
    return q->data[q->top - 1];
  }
  return q->data[q->top];
}

void push(Queue* q, Coord p){
  q->data[q->tail++] = p;
  return;
}

int isEmpty(Queue q){
  if (q->tail == q->top) return 1;
  return 0;
}


void update_group(Board* b, int x, int y){
    if (x < DEFAULT_SIZE - 1 && b->grid[x+1][y] == b->grid[x][y]){
      b->groups[x][y].n_right  = 1;
      b->groups[x+1][y].n_left = 1;
    }
    if (x > 0 && b->grid[x-1][y] == b->grid[x][y]){
      b->groups[x][y].n_left= 1;
      b->groups[x-1][y].n_right = 1;
    }
    if (y < DEFAULT_SIZE - 1 && b->grid[x][y+1] == b->grid[x][y]){
      b->groups[x][y].n_down= 1;
      b->groups[x][y+1].n_up = 1;
    }
    if (y > 0 && b->grid[x][y-1] == b->grid[x][y]){
      b->groups[x][y].n_up= 1;
      b->groups[x][y-1].n_down= 1;
    }
 return;
}
void update_liberties(Board* b, int x, int y){
  int b_[DEFAULT_SIZE][DEFAULT_SIZE] = {0};
  int visited[DEFAULT_SIZE][DEFAULT_SIZE] = {0};
  int visited_update[DEFAULT_SIZE][DEFAULT_SIZE] = {0};
  int sum = 0;
  Queue q = create_queue();
  push(&q, (Coord) {x,y});
  while (!isEmpty(q)){
    Coord next = pop(&q);
    int x_ = next.x;
    int y_ = next.y;
    if (x_ == 0 && y == 0) {
      if (b->grid[x_+1][y_] == 0 && b_[x_+1][y_] == 0){
        b_[x_+1][y_] = 1;
        sum++;
      }
      if (b->grid[x_][y_+1] == 0 && b_[x_][y_+1] == 0){
        b_[x_][y_+1] = 1;
        sum++;
      }
    }

    else if (x_ == DEFAULT_SIZE - 1 && y_ == DEFAULT_SIZE-1){
      if (b->grid[x_-1][y_] == 0 && b_[x_-1][y_] == 0){
        b_[x_-1][y_] = 1;
        sum++;
      }
      if (b->grid[x_][y_-1] == 0 && b_[x_][y_-1] == 0){
        b_[x_][y_-1] = 1;
        sum++;
      }
    }

    else if (x_ == 0 && y_ < DEFAULT_SIZE-1 && y_ > 0){
      if (b->grid[x_+1][y_] == 0 && b_[x_+1][y_] == 0){
        b_[x_-1][y_] = 1;
        sum++;
      }
      if (b->grid[x_][y_-1] == 0 && b_[x_][y_-1] == 0){
        b_[x_][y_-1] = 1;
        sum++;
      }      
      if (b->grid[x_][y_+1] == 0 && b_[x_][y_+1] == 0){
        b_[x_][y_+1] = 1;
        sum++;
      }
    }
    
    else if (x_ == DEFAULT_SIZE-1 && y_ < DEFAULT_SIZE-1 && y_ > 0){
      if (b->grid[x_-1][y_] == 0 && b_[x_-1][y_] == 0){
        b_[x_-1][y_] = 1;
        sum++;
      }
      if (b->grid[x_][y_-1] == 0 && b_[x_][y_-1] == 0){
        b_[x_][y_-1] = 1;
        sum++;
      }      
      if (b->grid[x_][y_+1] == 0 && b_[x_][y_+1] == 0){
        b_[x_][y_+1] = 1;
        sum++;
      }
    }

    else if (y_ == 0 && x_ < DEFAULT_SIZE-1 && x_ > 0){
      if (b->grid[x_+1][y_] == 0 && b_[x_+1][y_] == 0){
        b_[x_+1][y_] = 1;
        sum++;
      }
      if (b->grid[x_-1][y_] == 0 && b_[x_-1][y_] == 0){
        b_[x_-1][y_] = 1;
        sum++;
      }      
      if (b->grid[x_][y_+1] == 0 && b_[x_][y_+1] == 0){
        b_[x_][y_+1] = 1;
        sum++;
      }
    }
    else if (y_ == DEFAULT_SIZE-1 && x_ < DEFAULT_SIZE-1 && x_ > 0){
      if (b->grid[x_-1][y_] == 0 && b_[x_-1][y_] == 0){
        b_[x_-1][y_] = 1;
        sum++;
      }
      if (b->grid[x_][y_-1] == 0 && b_[x_][y_-1] == 0){
        b_[x_][y_-1] = 1;
        sum++;
      }      
      if (b->grid[x_+1][y_] == 0 && b_[x_+1][y_] == 0){
        b_[x_+1][y_] = 1;
        sum++;
      }
    }
    else{
      if (b->grid[x_-1][y_] == 0 && b_[x_-1][y_] == 0){
        b_[x_-1][y_] = 1;
        sum++;
      }
      if (b->grid[x_][y_-1] == 0 && b_[x_][y_-1] == 0){
        b_[x_][y_-1] = 1;
        sum++;
      }      
      if (b->grid[x_+1][y_] == 0 && b_[x_+1][y_] == 0){
        b_[x_+1][y_] = 1;
        sum++;
      }
      if (b->grid[x_][y_+1] == 0 && b_[x_][y_+1] == 0){
        b_[x_][y_+1] = 1;
        sum++;
      }
    }

    if (b->groups[x_][y_].n_up && !visited[x_][y-1_] ){
      push(&q, (Coord) {x_,y_-1});
    }
    if (b->groups[x_][y_].n_down && !visited[x_][y+1_]){
      push(&q, (Coord) {x_,y_+1});
    }
    if (b->groups[x_][y_].n_left && !visited[x_-1][y-1_]){
      push(&q, (Coord) {x_-1,y_});
    }
    if (b->groups[x_][y_].n_right && !visited[x_+1][y_]){
      push(&q, (Coord) {x_+1,y_});
    }
  }
  Queue q_update;
  push(&q_update,(Coord) {x,y});
  while (!isEmpty(q_update)){
    Coord next = pop(&q_update);
    int x_ = next.x;
    int y_ = next.y;
    b->liberty_cnt[x_][y_] = sum;
    if (b->groups[x_][y_].n_up && !visited_update[x_][y-1_] ){
      push(&q_update, (Coord) {x_,y_-1});
    }
    if (b->groups[x_][y_].n_down && !visited_update[x_][y+1_]){
      push(&q_update, (Coord) {x_,y_+1});
    }
    if (b->groups[x_][y_].n_left && !visited_update[x_-1][y-1_]){
      push(&q_update, (Coord) {x_-1,y_});
    }
    if (b->groups[x_][y_].n_right && !visited_update[x_+1][y_]){
      push(&q_update, (Coord) {x_+1,y_});
    }
  }
  return;
}

void play(Game *game, int x, int y){ 
  // add stone and then merge it with possible groups
  if (0 <= x && x < DEFAULT_SIZE && 0 <= y  && y <  DEFAULT_SIZE){
    game->b.grid[x][y] = game->turn + 1;
    update_group(&(game->b),x,y);
    // update_liberties(x);
    game->turn = (game->turn == 1) ? 0 : 1;
  }

  else fprintf(stderr, "Coordinates out of bounds!! \n");
  return;
}

void gameloop(Game* game){
  while(1){
    int x,y;
    if 
    printf("Play your move:");
    scanf("%d %d",&x,&y);
    printf("\n");
    play(game, x, y);
    for (int i = 0; i != DEFAULT_SIZE; i++){
      for (int j = 0; j != DEFAULT_SIZE ; j++){
        printf("%d ", game->b.grid[i][j]);
      }
      printf("\n");
    }
  }
  return;
}

int main(){
  Game game;
  memset(&game,0, sizeof(game));
  gameloop(&game);
  return 0;
}
