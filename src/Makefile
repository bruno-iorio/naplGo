
main: main.o board.o sgfwriter.o mcts.o
	gcc -g main.o board.o sgfwriter.o mcts.o -o main -lm

main.o: main.c 
	gcc -g -c main.c 

board.o: board.c
	gcc -g -c board.c

sgfwriter.o: sgfwriter.c
	gcc -g -c sgfwriter.c

mcts.o: mcts.c 
	gcc -g -c mcts.c

clean:
	rm board.o mcts.o sgfwriter.o main.o main
 
