
CC = gcc
OBJS = main.o board.o sgfwriter.o mcts.o

# optimized build (for speed measurement)
main: $(OBJS)
	$(CC) -O2 $(OBJS) -o main -lm

# profiling build (for gprof)
prof: CFLAGS = -O0 -pg -g
prof: $(OBJS)
	$(CC) -O0 -pg -g $(OBJS) -o main_prof -lm

%.o: %.c
	$(CC) $(CFLAGS) -c -g $< -o $@

clean:
	rm -f $(OBJS) main main_prof gmon.out
