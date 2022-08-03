#include <ctype.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include <stdio.h>

#include "line.h"
#include "editor.h"


struct editor_state{
    struct line *head;
    struct line *current_line; 
    ssize_t current_line_n;
    ssize_t line_count;
    ssize_t view;
    WINDOW * win;
};

struct editor_state * init_editor(FILE *file){

    fseek(file, 0, SEEK_END);
    long file_sz = ftell(file);
    rewind(file);

    struct line *head = read_lines(file,file_sz, NULL);

    initscr();
    cbreak();
    noecho();
    nl();
    keypad(stdscr,1);

    struct editor_state *state = malloc(sizeof(struct editor_state));
    state->current_line = head;
    state->head = head;
    state->view =0;
    state->win = newwin(getmaxy(stdscr)-1, getmaxx(stdscr)-3, 0, 3);
    keypad(state->win,1);
    state->current_line_n =0;
    state->line_count =0;
    for (struct line *line = head; line; line=line->next)
        state->line_count++;
    wmove(state->win, 0,0);
    paint(state);
    return state;
}
void write_buffer(struct editor_state * state, FILE *file){
    for(struct line *curr = state->head; curr; curr=curr->next){
        fputs(curr->str, file);
        fputc('\n', file);
    } 
}
void destroy_editor(struct editor_state * state){
    destroy_line_buffer(state->head);
    delwin(state->win);
    endwin();
    free(state);
}
void paint(struct editor_state * state){
    update_window(state); 
    refresh();
    wrefresh(state->win);
}
void update_window(struct editor_state * state){ 
    int row;
    struct line * draw_line;

    move(0,0);
    clrtobot();
    wclrtobot(state->win);
    
    for(row = state->current_line_n, draw_line = state->current_line; row != state->view; row--, draw_line=draw_line->prev);
    for(row = 0; row < getmaxy(state->win) && draw_line; draw_line=draw_line->next, row++){
        mvwaddstr(state->win, row, 0, draw_line->str);
        mvprintw(row, 0, "%2d", row+state->view);
    }
    for(;row<getmaxy(state->win);row++)
        mvaddch(row, 1, '~');
    wmove(state->win, state->current_line_n -state->view, 0);
}

void update_window_after(struct editor_state*state){
    int startx, starty;
    startx=getcurx(state->win);
    starty=getcury(state->win);
    int y = state->current_line_n - state->view;
    
    struct line *draw_line = state->current_line;
    move(y,0);
    clrtobot();
    wclrtobot(state->win);
    for(; y < getmaxy(state->win) && draw_line; draw_line=draw_line->next, y++){
        mvwaddstr(state->win, y, 0, draw_line->str);
        mvprintw(y, 0, "%2d", y+state->view);
    }
    for(;y<getmaxy(state->win);y++)
        mvaddch(y, 1, '~');
 
    wmove(state->win, starty, startx);
}
void update_view(struct editor_state *state){
    if (state->current_line_n-state->view >= getmaxy(state->win)){
        state->view+= getmaxy(state->win);
    } else if(state->current_line_n < state->view){ 
        state->view-= getmaxy(state->win);
        if (state->view <0){
            state->view = 0;
        }
    }else {
        return;
    }
    update_window(state);
    refresh();
}
int goto_prev(struct editor_state *state, int col){
    if (!state->current_line->prev) return 1;
    state->current_line_n--;
    state->current_line = state->current_line->prev;
    move_curs(state, col);
    update_view(state);
    return 0;
}

int goto_next(struct editor_state *state, int col){
    if (!state->current_line->next) return 1;
    state->current_line_n++;
    state->current_line = state->current_line->next;
    move_curs(state, col);
    update_view(state);
    return 0;
}

int goto_line(struct editor_state * state, int line, int col){
    if (line <0 || line>=state->line_count) return 1;
    state->current_line_n = line;
    
    int i =0;
    for (state->current_line = state->head; i<line; i++, state->current_line=state->current_line->next);
    move_curs(state, col);
    update_view(state);
    return 0;
}

int move_curs(struct editor_state *state, int x){
    if(x<0){
        wmove(state->win, state->current_line_n-state->view, 0); 
    }else if( x>strlen(state->current_line->str)){

        wmove(state->win, state->current_line_n-state->view, strlen(state->current_line->str)); 
    }else {
        wmove(state->win, state->current_line_n-state->view, x); 
    }
    return 0;
}

void edit_delete_char(struct editor_state * state, int col){
    del_ch(state->current_line, col);

    mvwaddstr(state->win, state->current_line_n-state->view, 0, state->current_line->str);
    wclrtoeol(state->win);
    move_curs(state, col);

}
void edit_delete_prev_line(struct editor_state * state){
    if(!state->current_line->prev) return;
    del_nl(state->current_line);
    state->current_line = NULL;

    state->line_count--;
    goto_line(state, state->current_line_n-1, 0);
    update_window_after(state);
    refresh();
}
void edit_insert_nl(struct editor_state * state){
    insert_nl(state->current_line, getcurx(state->win));
    state->line_count++;
    update_window_after(state);
    goto_next(state, 0); 
    refresh();
}
void edit_insert_char(struct editor_state * state, int ch, int curx){
    if (!isprint(ch)) return;
    insert_ch(state->current_line, ch, curx);
    mvwaddstr(state->win, state->current_line_n-state->view, 0, state->current_line->str);
    wclrtoeol(state->win);
    move_curs(state, curx+1);
}

void editor(struct editor_state * state){
    int input;
    int curx;
    while(1) {
        input = wgetch(state->win);
        curx = getcurx(state->win);
        switch (input){
            case KEY_END:
                return;
            case KEY_UP:
                goto_prev(state, curx);
                break;
            case KEY_DOWN:
                goto_next(state, curx);
                break;
            case KEY_RIGHT:
                move_curs(state, curx+1);
                break;
            case KEY_LEFT:
                move_curs(state, curx-1);
                break;
            case KEY_BACKSPACE:
                if (curx==0) {
                    edit_delete_prev_line(state);
                    break;
                }
                edit_delete_char(state, curx-1);
                break;
            case KEY_ENTER:
            case '\n':
                edit_insert_nl(state);
               break;
            default:
               edit_insert_char(state, input, curx);
               break;
       }
        wrefresh(state->win);
    }
}

