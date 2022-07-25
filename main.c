#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <curses.h>

#include "line.h"

void editor();
void update_window_after(int , WINDOW * , struct line*);

int main(int argc, char ** argv){
    FILE * file = NULL;
    char * buffer = NULL;
    long file_sz  = 0;

    if (argc == 1){
        return 1;
    }
    
    file = fopen(argv[1], "r+");
    if (!file){
        return 1;
    }

    fseek(file, 0, SEEK_END);
    file_sz = ftell(file);
    rewind(file);

    const struct line *file_line = read_lines(file,file_sz, NULL);

    initscr();
    cbreak();
    noecho();
    keypad(stdscr,1);
    WINDOW *editor_win = newwin(getmaxy(stdscr)-1, getmaxx(stdscr)-3, 0, 3);
    keypad(editor_win,1);

    struct line *curr = file_line;
    update_window_after(0, editor_win, file_line);
    refresh();
    wrefresh(editor_win);
    editor(editor_win, file_line);

    delwin(editor_win);
    endwin();

    rewind(file);
    for(curr = file_line; curr; curr=curr->next){
        fputs(curr->str, file);
        //if (curr->next)
            fputc('\n', file);
        if (curr->prev) 
            free(curr->prev->str);
        free(curr->prev);
    }
 
    fclose(file);
    free(buffer);
    return 0;

ERR_main:
    fclose(file);
    free(buffer);
    return 1;

}
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

