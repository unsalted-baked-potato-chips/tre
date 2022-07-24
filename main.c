#include <stdio.h>
#include <stdlib.h>

#include <curses.h>

#define LINE_LEN_MIN 5

struct line {
    char * str;
    size_t max;
    struct line * next;
    struct line * prev;
};


struct line * read_lines(FILE *, size_t, struct line *);

void editor();

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

    struct line *file_line = read_lines(file,file_sz, NULL);

    initscr();
    cbreak();
    noecho();
    keypad(stdscr,1);
    for(int col = 0; file_line; file_line=file_line->next, col++){
        move(col, 0);
        addstr(file_line->str);
        refresh();
    }
    
    editor();

    refresh();
    endwin();

    fclose(file);
    free(buffer);
    return 0;

ERR_main:
    fclose(file);
    free(buffer);
    return 1;

}

void editor(){
    int input;
    int cury, curx;
    while(1) {
        input = getch();
        cury = getcury(stdscr);
        curx = getcurx(stdscr);
        switch (input){
            case KEY_END:
                return;
            case KEY_UP:
                move(cury?cury-1:cury, curx);
                refresh();
                break;
            case KEY_DOWN:
                move(cury<getmaxy(stdscr)?cury+1:cury, curx);
                refresh();
                break;
            case KEY_RIGHT:
                move(cury, curx<getmaxx(stdscr)?curx+1:curx);
                refresh();
                break;
            case KEY_LEFT:
                move(cury, curx?curx-1:curx);
                refresh();
                break;
        }
    }
}
//TODO: handle nonl terminated files
struct line * read_lines(FILE * file, size_t file_sz, struct line * prev){
    struct line *line = malloc(sizeof(struct line));
    line->str = malloc(LINE_LEN_MIN);
    line->prev = prev;
    line->max = LINE_LEN_MIN;
    
    long chars_read = fread(line->str, 1, LINE_LEN_MIN, file);
    int i =0;

    while(1){
        for (; i<chars_read && line->str[i]!='\n'; i++);
        if (i<chars_read){ //nl found
            fseek(file, i-chars_read, SEEK_CUR);
            line->str[i] = 0;
            if (ftell(file)+1==file_sz){
                return NULL;
            }else {
                fseek(file, 1, SEEK_CUR);
                line->next = read_lines(file, file_sz, line);
                return line;
            }
        }else {
            line->max+=LINE_LEN_MIN;
            line->str = realloc(line->str, line->max);
            chars_read += fread(line->str+i, 1, LINE_LEN_MIN, file);
        }

    }
}
