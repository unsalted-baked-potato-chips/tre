#ifndef EDITOR_H
#define EDITOR_H


#include <curses.h>

#include "line.h"

struct editor_state{
    struct line *head;
    struct line *current_line; 
    size_t current_line_n;
    size_t line_count;
    size_t view;
    WINDOW * win;
};

struct editor_state *init_editor(struct line *head); 
void editor(struct editor_state *);
void update_window_after(struct editor_state*state);


#endif /* EDITOR_H */
