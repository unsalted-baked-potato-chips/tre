#ifndef EDITOR_H
#define EDITOR_H


#include <curses.h>

struct editor_state;

struct editor_state * init_editor(FILE *file, char filename[256]);
void destroy_editor(struct editor_state * state);


int write_buffer(struct editor_state * state);

void editor(struct editor_state *);
void update_window_after(struct editor_state*state);
void update_window(struct editor_state * state);
void paint(struct editor_state * state);
int move_curs(struct editor_state *, int );
int goto_line(struct editor_state * , int , int );
int goto_next(struct editor_state *state, int col);
int goto_prev(struct editor_state *state, int col);


void edit_insert_char(struct editor_state * state, int ch, int curx);
void edit_insert_nl(struct editor_state * state);
void edit_delete_prev_line(struct editor_state * state);
void edit_delete_char(struct editor_state * state, int col);

#endif /* EDITOR_H */
