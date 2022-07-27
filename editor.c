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
    update_window_after(state);
    wmove(state->win, 0,0);
    refresh();
    wrefresh(state->win);
    return state;
}

void update_window_after(struct editor_state*state){
    int startx, starty;
    startx=getcurx(state->win);
    starty=getcury(state->win);
    int col =0 ;
    int y = state->current_line_n - state->view;
    
    struct line *curr = state->current_line;
    move(y,0);
    clrtobot();
    wclrtobot(state->win);
    for(col = y; curr; curr=curr->next, col++){
        mvwaddstr(state->win, col, 0, curr->str);
        mvprintw(col, 0, "%2d", col);
    }
    for(;col<getmaxy(state->win);col++)
        mvaddch(col, 1, '~');
    wmove(state->win, starty, startx);
}
int goto_line(struct editor_state * state, int line, int col){
    if (line <0 || line>=state->line_count) return 1;
    state->current_line_n = line;
    
    int i =0;
    for (state->current_line = state->head; i<line; i++, state->current_line=state->current_line->next);
    move_curs(state, col);
    return 0;
}

int move_curs(struct editor_state *state, int x){
    if(x<0){

        wmove(state->win, state->current_line_n, 0); 
    }else if( x>=strlen(state->current_line->str)){

        wmove(state->win, state->current_line_n, strlen(state->current_line->str)-1); 
    }else {
        wmove(state->win, state->current_line_n, x); 
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
                goto_line(state, state->current_line_n-1, curx);
                break;
            case KEY_DOWN:
                goto_line(state, state->current_line_n+1, curx);
                break;
            case KEY_RIGHT:
                move_curs(state, curx+1);
                break;
            case KEY_LEFT:
                move_curs(state, curx-1);
                break;
            case KEY_BACKSPACE:
                if(cury>=state->line_count) break;
                if (!curx) {
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
                goto_line(state, state->current_line_n+1, 0);
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

