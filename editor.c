#include <ctype.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>

#include "line.h"
#include "editor.h"

struct editor_state * init_editor(struct line * head){
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
    int row =0 ;
    int y = state->current_line_n - state->view;
    
    struct line *draw_line = state->current_line;
    move(y,0);
    clrtobot();
    wclrtobot(state->win);
    for(row = y; row < getmaxy(state->win) && draw_line; draw_line=draw_line->next, row++){
        mvwaddstr(state->win, row, 0, draw_line->str);
        mvprintw(row, 0, "%2d", row+state->view);
    }
    for(;row<getmaxy(state->win);row++)
        mvaddch(row, 1, '~');
 
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
void editor(struct editor_state * state){
    int input;
    int cury, curx;
    while(1) {
        input = wgetch(state->win);
        cury = getcury(state->win);
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
                if(cury>=state->line_count) break;
                if (curx==0) {
                    if(!state->current_line->prev) break;
                    del_nl(state->current_line);
                    state->current_line = NULL;
                    
                    state->line_count--;
                    goto_line(state, state->current_line_n-1, 0);
                    update_window_after(state);
                    refresh();
                    break;
                }
                del_ch(state->current_line, curx-1);
                
                mvwaddstr(state->win, state->current_line_n, 0, state->current_line->str);
                wclrtoeol(state->win);
                move_curs(state, curx-1);
                break;
            case KEY_ENTER:
            case '\n':
                if(cury>=state->line_count) break;
                insert_nl(state->current_line, curx);
                state->line_count++;
                update_window_after(state);
                goto_next(state, 0); 
                refresh();
                break;
            default:
                if (cury>=state->line_count) break;
                if (isprint(input)){
                    insert_ch(state->current_line, input, curx);
                }
                mvwaddstr(state->win, state->current_line_n, 0, state->current_line->str);
                wclrtoeol(state->win);
                move_curs(state, curx+1);
        }
        wrefresh(state->win);
    }
}

