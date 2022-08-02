#ifndef EDITOR_H
#define EDITOR_H


#include <curses.h>

#include "line.h"

struct editor_state{
    struct line *head;
    struct line *current_line; 
    ssize_t current_line_n;
    ssize_t line_count;
    ssize_t view;
    WINDOW * win;
};

struct editor_state *init_editor(FILE *); 
void destroy_editor(struct editor_state * state);


void write_buffer(struct editor_state * state, FILE *file);

void editor(struct editor_state *);
void update_window_after(struct editor_state*state);
void update_window(struct editor_state * state);
void paint(struct editor_state * state);
int move_curs(struct editor_state *, int );
int goto_line(struct editor_state * , int , int );
int goto_next(struct editor_state *state, int col);
int goto_prev(struct editor_state *state, int col);


#endif /* EDITOR_H */
