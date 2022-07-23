#include <stdio.h>
#include <stdlib.h>

#include <curses.h>

#define LINE_LEN_MIN 50

struct line {
    char * str;
    size_t max;
    struct line * next;
    struct line * prev;
};


struct line * load_file(FILE *);

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

    struct line *file_line = load_file(file);

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
struct line *read_line(FILE * file){
    struct line *line = (struct line *) malloc(sizeof(struct line));
    line->next = NULL;
    line->prev = NULL;
    line->max = LINE_LEN_MIN;
    line->str = (char *) malloc(LINE_LEN_MIN);

    while(!feof(file)){
        ssize_t pos = 0;
        size_t read = fread(line->str+pos, 1, LINE_LEN_MIN, file);
        for (pos = 1;pos<read; pos++){
            if (line->str[pos]== '\n'){
                line->str[pos]=0;
                fseek(file, pos-read, SEEK_CUR);
                printf("%d, %d\n", pos, read);
                return line;
            }
        }
        line->max+=LINE_LEN_MIN;
        realloc(line->str, line->max);
    }
    return NULL;
}
struct line * load_file(FILE * file){
    struct line * prev = NULL;
    struct line * curr = NULL;
    
    prev=read_line(file);
    while(1){
        curr = read_line(file);
        if (prev)
            prev->next=curr;
        curr->prev = prev;
        prev = curr;
        if(!curr) break;    
    }
    for (prev=curr;curr;prev=curr, curr=curr->prev);
    return prev;
}
