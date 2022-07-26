#include <ctype.h>
#include <string.h>
#include <curses.h>

#include "line.h"
#include "editor.h"
#include "editor_ctrl.h"

int write_buffer(struct editor_state * state){
    FILE * file;                
    file = fopen(state->filename, "w");
    if (!file){
        return 1;
    }

    flockfile(file);

    for(struct line *curr = state->head; curr; curr=curr->next){
        fputs(curr->str, file);
        fputc('\n', file);
    } 

    funlockfile(file);
    fclose(file);
    return 0;

}
int goto_prev(struct editor_state *state, int ch){
    if (!state->current_line->prev) return 1;
    state->current_line_n--;
    state->current_line = state->current_line->prev;
    move_curs(state, ch);
    update_view(state);
    return 0;
}

int goto_next(struct editor_state *state, int ch){
    if (!state->current_line->next) return 1;
    state->current_line_n++;
    state->current_line = state->current_line->next;
    move_curs(state, ch);
    update_view(state);
    return 0;
}

int goto_line(struct editor_state * state, int line, int ch){
    if (line <0 || line>=state->line_count) return 1;
    state->current_line_n = line;
    
    int i =0;
    for (state->current_line = state->head; i<line; i++, state->current_line=state->current_line->next);
    move_curs(state, ch);
    update_view(state);
    return 0;
}

int move_curs(struct editor_state *state, int ch){
    if(ch<0){
        state->current_ch = 0;
    }else{
        int len = strlen(state->current_line->str);
        if( ch>=len){
            state->current_ch = len;
        }
        else {
            state->current_ch = ch;
        }
    }
    wmove(state->win, state->current_line_n-state->view, chtocol(state->current_line, state->current_ch)); 
    return 0;
}

void edit_delete_char(struct editor_state * state, int ch){
    del_ch(state->current_line, ch);

    add_line(state, state->current_line, state->current_line_n-state->view);
    wclrtoeol(state->win);
    move_curs(state, ch);

}
void edit_delete_prev_line(struct editor_state * state){
    if(!state->current_line->prev) return;
    goto_prev(state, strlen(state->current_line->prev->str));
    del_nl(state->current_line->next);
    state->line_count--;
    update_window_after(state);
    refresh();
}
void edit_insert_nl(struct editor_state * state){
    insert_nl(state->current_line, state->current_ch);
    state->line_count++;
    update_window_after(state);
    goto_next(state, 0); 
    refresh();
}
void edit_insert_char(struct editor_state * state, int ch, int curx){
    if (!isprint(ch) && ch != '\t') return;
    insert_ch(state->current_line, ch, curx);
    wmove(state->win, state->current_line_n-state->view, 0);
    wclrtoeol(state->win);
    add_line(state, state->current_line, state->current_line_n-state->view);
    wclrtoeol(state->win);
    move_curs(state, curx+1);
}

