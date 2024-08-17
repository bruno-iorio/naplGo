all: main

main: main.o board.o mcts.o sgfparser.o sgf_player.o
	gcc -o main main.o board.o mcts.o sgfparser.o sgf_player.o

main.o: main.c:
	gcc -c main.c

mcts.o:
	gcc -c mcts.c

board.o: board.c
	gcc -c board.c

sgfparser.o: sgfparser.c
	gcc -c sgfparser.c
	
sgf_player.o: sgf_player.c
	gcc -c sgf_player.c

remove:
	rm main sgf_player.o board.o sgfparser.o mcts.o main.o
 
