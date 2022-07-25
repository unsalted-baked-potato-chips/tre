CFLAGS+=-std=c99 -Wall -Wpedantic
LIBS?=-lcurses

tre: main.c line.c
	cc $(CFLAGS) main.c line.c -o tre $(LIBS)
