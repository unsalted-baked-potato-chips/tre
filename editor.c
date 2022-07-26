#include <ctype.h>
#include <stdlib.h>
#include <curses.h>

#include "line.h"
#include "editor.h"

struct editor_state * init_editor(struct line * head){
    struct editor_state *state = malloc(sizeof(struct editor_state));
    state->current_line = 0;
    state->head = head;
    state->view =0;
    state->win = newwin(getmaxy(stdscr)-1, getmaxx(stdscr)-3, 0, 3);
    keypad(state->win,1);
    state->current_line_n =0;
    state->line_count =0;
    for (struct line *line = head; line; line=line->next)
        state->line_count++;
    return state;
}

void update_window_after(struct editor_state*state){
    int col;
    int y = state->current_line_n - state->view;
    
    struct line *curr = state->head;
    move(y,0);
    clrtobot();
    wclrtobot(state->win);
    for(col = y; curr; curr=curr->next, col++){
        mvwaddstr(state->win, col, 0, curr->str);
        mvprintw(col, 0, "%2d", col);
    }
    for(;col<getmaxy(state->win);col++)
        mvaddch(col, 1, '~');
}
void editor(struct editor_state * state){
    int input;
    int cury, curx;
    struct line * line;
    int line_n;
    while(1) {
        input = wgetch(state->win);
        cury = getcury(state->win);
        curx = getcurx(state->win);
        line=state->head;
        for(line_n = 0;line_n<cury && line; line=line->next, line_n++);
        switch (input){
            case KEY_END:
                return;
            case KEY_UP:
                wmove(state->win, cury?cury-1:cury, curx);
                state->current_line_n = state->current_line_n?state->current_line_n-1: state->current_line_n;
                break;
            case KEY_DOWN:
                wmove(state->win, cury<getmaxy(stdscr)?cury+1:cury, curx);
                state->current_line_n = state->current_line_n<getmaxy(stdscr)?state->current_line_n-1: state->current_line_n;
                break;
            case KEY_RIGHT:
                wmove(state->win, cury, curx<getmaxx(stdscr)?curx+1:curx);
                break;
            case KEY_LEFT:
                wmove(state->win, cury, curx?curx-1:curx);
                state->current_line = state->current_line?state->current_line-1: state->current_line;
                break;
            case KEY_BACKSPACE:
                if(!line) break;
                if (!curx) {
                    if(!line->prev) break;
                    struct line *prev = line->prev;
                    del_nl(line);
                    line = NULL;
                    update_window_after(state);
                    wmove(state->win, cury-1, 0);
                    state->current_line_n = state->current_line_n?state->current_line_n-1: state->current_line_n;
                    refresh();
                    break;
                }
                del_ch(line, curx-1);
                
                mvwaddstr(state->win, line_n, 0, line->str);
                wclrtoeol(state->win);
                wmove(state->win, cury, curx-1);
                break;
            case KEY_ENTER:
            case '\n':
                if(!line) break;
                insert_nl(line, curx);
                update_window_after(state);
                state->current_line_n = state->current_line_n<getmaxy(stdscr)?state->current_line_n-1: state->current_line_n;
                refresh();
                wmove(state->win, cury+1, 0);
                break;
            default:
                if (!line) break;
                if (isprint(input)){
                    insert_ch(line, input, curx);
                }
                mvwaddstr(state->win, line_n, 0, line->str);
                wclrtoeol(state->win);
                wmove(state->win, cury, curx+1);
        }
        wrefresh(state->win);
    }
}

