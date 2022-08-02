#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <curses.h>

#include "editor.h"

int main(int argc, char ** argv){
    FILE * file = NULL;

    if (argc == 1){
        return 1;
    }
    file = fopen(argv[1], "r");
    if (!file){
        return 1;
    }
    
    flockfile(file);

    struct editor_state * editor_state = init_editor(file);
    funlockfile(file);
    fclose(file);
    file = NULL;

    editor(editor_state);

    file = fopen(argv[1], "w");
    if (!file){
        goto ERR_main;
    }

    flockfile(file);
    
    write_buffer(editor_state, file);

    funlockfile(file);
    fclose(file);

    destroy_editor(editor_state);
    return 0;

ERR_main:
    funlockfile(file);
    fclose(file);
    destroy_editor(editor_state);
    return 1;

}

