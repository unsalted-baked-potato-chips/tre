#include <ctype.h>
#include <curses.h>

#include "line.h"
#include "editor.h"

void update_window_after(int y, WINDOW * win, struct line*head){
    int col;
    struct line *curr = head;
    move(y,0);
    clrtobot();
    wclrtobot(win);
    for(col = y; curr; curr=curr->next, col++){
        mvwaddstr(win, col, 0, curr->str);
        mvprintw(col, 0, "%2d", col);
    }
    for(;col<getmaxy(win);col++)
        mvaddch(col, 1, '~');
}
void editor(WINDOW *win, struct line * head){
    int input;
    int cury, curx;
    struct line * line;
    int line_n;
    while(1) {
        input = wgetch(win);
        cury = getcury(win);
        curx = getcurx(win);
        line=head;
        for(line_n = 0;line_n<cury && line; line=line->next, line_n++);
        switch (input){
            case KEY_END:
                return;
            case KEY_UP:
                wmove(win, cury?cury-1:cury, curx);
                break;
            case KEY_DOWN:
                wmove(win, cury<getmaxy(stdscr)?cury+1:cury, curx);
                break;
            case KEY_RIGHT:
                wmove(win, cury, curx<getmaxx(stdscr)?curx+1:curx);
                break;
            case KEY_LEFT:
                wmove(win, cury, curx?curx-1:curx);
                break;
            case KEY_BACKSPACE:
                if(!line) break;
                if (!curx) {
                    if(!line->prev) break;
                    struct line *prev = line->prev;
                    del_nl(line);
                    line = NULL;
                    update_window_after(cury-1, win, prev);
                    wmove(win, cury-1, 0);
                    refresh();
                    break;
                }
                del_ch(line, curx-1);
                
                mvwaddstr(win, line_n, 0, line->str);
                wclrtoeol(win);
                wmove(win, cury, curx-1);
                break;
            case KEY_ENTER:
            case '\n':
                if(!line) break;
                insert_nl(line, curx);
                update_window_after(cury, win, line);
                refresh();
                wmove(win, cury+1, 0);
                break;
            default:
                if (!line) break;
                if (isprint(input)){
                    insert_ch(line, input, curx);
                }
                mvwaddstr(win, line_n, 0, line->str);
                wclrtoeol(win);
                wmove(win, cury, curx+1);
        }
        wrefresh(win);
    }
}

