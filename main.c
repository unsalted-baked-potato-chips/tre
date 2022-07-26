#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <curses.h>

#include "line.h"
#include "editor.h"

int main(int argc, char ** argv){
    FILE * file = NULL;
    char * buffer = NULL;
    long file_sz  = 0;

    if (argc == 1){
        return 1;
    }
    file = fopen(argv[1], "r");
    if (!file){
        return 1;
    }
    
    flockfile(file);
    fseek(file, 0, SEEK_END);
    file_sz = ftell(file);
    rewind(file);

    const struct line *file_line = read_lines(file,file_sz, NULL);
    
    fclose(file);
    file = NULL;

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

    file = fopen(argv[1], "w");
    if (!file){
        goto ERR_main;
    }
    for(curr = file_line; curr; curr=curr->next){
        fputs(curr->str, file);
        //if (curr->next)
            fputc('\n', file);
        if (curr->prev) 
            free(curr->prev->str);
        free(curr->prev);
    } 
    funlockfile(file);
    fclose(file);
    free(buffer);
    return 0;

ERR_main:
    funlockfile(file);
    fclose(file);
    free(buffer);
    return 1;

}
