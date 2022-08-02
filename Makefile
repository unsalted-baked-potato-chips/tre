CFLAGS+=-std=c99 -Wall -Wpedantic
LIBS?=-lcurses

tre: main.c line.c editor.c line.h editor.h
	cc $(CFLAGS) main.c line.c editor.c -o tre $(LIBS)

clean:
	rm tre tre.core

.PHONY: clean
