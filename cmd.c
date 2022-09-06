#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "editor_ctrl.h"

size_t cmds_len = 2;
char *cmds[]= {
    "q",
    "w",
    "e"
};

int handle_cmd(char *cmd_str, struct editor_state * state){
    int i;
    char * argv;
    if (!*cmd_str) return -1;
    argv = strtok(cmd_str, " ");
    for (i=0; i<cmds_len; i++){
        if (!(strcmp(cmds[i], argv))){
            break;
        }
    }
    argv = strtok(NULL, " ");
    switch (i){
        case 0:
            break;
        case 1:
            if (argv){
                strncpy(state->filename, argv, 255);
                state->filename[255]=0;
            }
            write_buffer(state);
            break;
        case 2:
            if(argv){
                struct editor_state ** editor_ref  =state->self;
                FILE * file;
                argv[255] =0;
                file = fopen(argv, "r");
                if (!file){
                    if ( errno == ENOENT){
                        file = NULL;
                    }else {
                        //TODO:debug printing
                        break;
                    }
                }
                destroy_editor(state);
                *editor_ref = init_editor(file, argv, editor_ref);
            }
            break;

        default:
            i= -1;
    }
    return i;
}
