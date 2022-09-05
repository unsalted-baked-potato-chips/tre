#ifndef EDITOR_H
#define EDITOR_H


#include <curses.h>

struct editor_state{
    struct line *head;
    struct line *current_line; 
    ssize_t current_line_n;
    ssize_t line_count;
    ssize_t view;
    char filename[256];
    int line_nw;
    int do_resize;
    WINDOW * win;
    struct editor_state ** self;
};


struct editor_state * init_editor(FILE *file, char filename[256], struct editor_state **self);
void update_window(struct editor_state * state); 
void destroy_editor(struct editor_state * state);

void editor(struct editor_state *);
void update_window_after(struct editor_state*state);
void update_window(struct editor_state * state);
void update_view(struct editor_state *state);
void paint(struct editor_state * state);

#endif /* EDITOR_H */
