all: main 

main.o: main.c
	gcc -c -Wall -Wextra main.c -g

mcts.o:
	gcc -c -Wall -Wextra ../MCTS/mcts.c -g

board.o:
	gcc -c -Wall -Wextra ../Mechanics/board.c -g

sgfparser.o: 
	gcc -c -Wall -Wextra ../Mechanics/sgfparser.c -g

main: main.o board.o mcts.o sgfparser.o
	gcc -o main main.o board.o mcts.o 
	rm board.o sgfparser.o mcts.o main.o

sgfparser:
	gcc -o sgfparser sgfparser.o

clean:
	rm main board.o sgfparser.o mcts.o main.o
