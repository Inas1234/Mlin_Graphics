CC = gcc
FLAGS = -Wall -Wextra -std=c99

all: test_graphics

test_graphics: examples/test_graphics.c
	$(CC) $(FLAGS) -o test_graphics examples/test_graphics.c -I.

clean:
	rm -f test_graphics
