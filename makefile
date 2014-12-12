
all: main.o
	gcc main.o -o minesweeper -lglut

main.o: main.c
	gcc -c main.c

clean:
	rm -rf *.o minesweeper

