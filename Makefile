CFLAGS?=-cstd=99 -Wall -pedantic
LIBS?=-lcurses

tre: main.c
	cc $(CFLAGS) main.c -o tre $(LIBS)
