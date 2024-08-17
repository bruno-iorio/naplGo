all: main

main.o: main.c
	gcc -c main.c

mcts.o:
	gcc -c ../MCTS/mcts.c

board.o:
	gcc -c ../Mechanics/board.c

sgfparser.o: 
	gcc -c ../Mechanics/sgfparser.c

main: main.o board.o mcts.o sgfparser.o
	gcc -o main main.o board.o mcts.o sgfparser.o

clean:
	rm main board.o sgfparser.o mcts.o main.o
