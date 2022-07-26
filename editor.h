#ifndef EDITOR_H
#define EDITOR_H


#include <curses.h>

#include "line.h"

void editor();
void update_window_after(int , WINDOW * , struct line*);


#endif /* EDITOR_H */
