#include <stdio.h>
#include <string.h>

#include "editor_ctrl.h"

size_t cmds_len = 2;
char *cmds[]= {
    "q",
    "w"
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
    switch (i){
        case 0:
            break;
        case 1:
            argv = strtok(NULL, " ");
            if (argv){
                strncpy(state->filename, argv, 255);
                state->filename[255]=0;
            }
            write_buffer(state);
            break;
        default:
            i= -1;
    }
    return i;
}
