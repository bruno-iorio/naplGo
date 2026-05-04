

main: main.o board.o sgfwriter.o
	gcc main.o board.o sgfwriter.o -o main

main.o: main.c 
	gcc -c main.c 

board.o: board.c
	gcc -c board.c

sgfwriter.o: sgfwriter.c
	gcc -c sgfwriter.c

clean:
	rm board.o sgfwriter.o main.o main
 
