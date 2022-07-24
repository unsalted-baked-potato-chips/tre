CFLAGS+=-std=c99 -Wall -Wpedantic
LIBS?=-lcurses

tre: main.c
	cc $(CFLAGS) main.c -o tre $(LIBS)
