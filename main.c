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

    struct editor_state * editor_state = init_editor(file, argv[1]);
    funlockfile(file);
    fclose(file);
    file = NULL;

    editor(editor_state);

    destroy_editor(editor_state);
    return 0;

ERR_main:
    funlockfile(file);
    fclose(file);
    destroy_editor(editor_state);
    return 1;

}

