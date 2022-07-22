#include <stdio.h>
#include <stdlib.h>

#include <curses.h>

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

    buffer = (char *) malloc(file_sz+2);
    if (!buffer){
        goto ERR_main;
    }

    fread(buffer, 1, file_sz+1, file);
    buffer[file_sz+1] = 0;

    initscr();
    cbreak();
    noecho();

    addstr(buffer);
    refresh();
    getch();

    endwin();


    fclose(file);
    free(buffer);
    return 0;

ERR_main:
    fclose(file);
    free(buffer);
    return 1;

}
