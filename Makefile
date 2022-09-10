CFLAGS+=-std=c99 -Wall -Wpedantic -D_XOPEN_SOURCE=700
LIBS?=-lcurses -lm

tre: main.c line.c editor.c editor_ctrl.c cmd.c cmd.h line.h editor.h
	cc $(CFLAGS) main.c line.c editor_ctrl.c editor.c cmd.c -o tre $(LIBS)

clean:
	rm tre tre.core

.PHONY: clean
