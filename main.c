#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <curses.h>

#include "editor.h"

int main(int argc, char ** argv){
    FILE * file = NULL;
    struct editor_state * editor_state = NULL;

    if (argc != 2){
        return 1;
    }
    file = fopen(argv[1], "r");
    if (!file){
        if ( errno == ENOENT){
            file = NULL;
        }else {
            perror(NULL);
            return 1;
        }
    }
    
    editor_state = init_editor(file, argv[1]);

    editor(editor_state);

    destroy_editor(editor_state);
    return 0;

ERR_main:
    funlockfile(file);
    fclose(file);
    destroy_editor(editor_state);
    return 1;

}

