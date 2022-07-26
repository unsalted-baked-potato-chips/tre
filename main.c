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

    struct line *file_line = read_lines(file,file_sz, NULL);
    
    fclose(file);
    file = NULL;

    initscr();
    cbreak();
    noecho();
    keypad(stdscr,1);

    struct editor_state * editor_state = init_editor(file_line);
    struct line *curr = file_line;

    update_window_after(editor_state);

    refresh();
    wrefresh(editor_state->win);

    editor(editor_state);
    
    delwin(editor_state->win);
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
