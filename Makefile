CFLAGS+=-std=c99 -Wall -Wpedantic
LIBS?=-lcurses -lm

tre: main.c line.c editor.c cmd.c cmd.h line.h editor.h
	cc $(CFLAGS) main.c line.c editor.c cmd.c -o tre $(LIBS)

clean:
	rm tre tre.core

.PHONY: clean
