#include <stdio.h>
#include <string.h>

#include "editor.h"

size_t cmds_len = 2;
char *cmds[]= {
    "q",
    "w"
};

int handle_cmd(char *cmd_str, struct editor_state * state){
    int i;
    for (i=0; i<cmds_len; i++){
        if (!(strcmp(cmds[i], strtok(cmd_str, " ")))){
            break;
        }
    }
    switch (i){
        case 0:
            break;
        case 1:
            write_buffer(state);
        default:
            i= -1;
    }
    return i;
}
