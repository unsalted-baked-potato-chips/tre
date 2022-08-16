#ifndef EDITOR_CTRL_H
#define EDITOR_CTRL_H

#include "editor.h"

int write_buffer(struct editor_state * state);

int move_curs(struct editor_state *, int );
int goto_line(struct editor_state * , int , int );
int goto_next(struct editor_state *state, int col);
int goto_prev(struct editor_state *state, int col);


void edit_insert_char(struct editor_state * state, int ch, int curx);
void edit_insert_nl(struct editor_state * state);
void edit_delete_prev_line(struct editor_state * state);
void edit_delete_char(struct editor_state * state, int col);



#endif /* EDITOR_CTRL_H */
