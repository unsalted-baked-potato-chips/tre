#include <stdlib.h>
#include <string.h>

#include <curses.h>

#include "line.h"

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
            if (ftell(file)+1>=file_sz){
                line->next = NULL;
            }else {
                fseek(file, 1, SEEK_CUR);
                line->next = read_lines(file, file_sz, line);
            }
            return line;
        }else {
            line->max+=LINE_LEN_MIN;
            line->str = realloc(line->str, line->max);
            chars_read += fread(line->str+i, 1, LINE_LEN_MIN, file);
        }

    }
}

void destroy_line_buffer(struct line *head){
    for (; head->next; head=head->next){
        free(head->prev);
        head->prev = NULL;
    }
    free(head);
}
int insert_ch(struct line * line, char ch, size_t pos){
    if (strlen(line->str)+2> line->max){
        line->max += LINE_LEN_MIN;
        line->str = realloc(line->str,line->max);
    }

    memmove(line->str+pos+1, line->str+pos, strlen(line->str+pos)+1);
    line->str[pos]=ch;
    return 0;
}

int insert_str(struct line * line, char *str, size_t pos){
    return 0;
}
int del_ch(struct line * line, size_t pos){
    memmove(line->str+pos,line->str+pos+1, strlen(line->str+pos+1)+1);
    return 0;
}

int del_nl(struct line *line){
    if (!line->prev) return 0;
    const size_t sz =strlen(line->str)+strlen(line->prev->str)+1;
    if (line->prev->max<sz){
        line->prev->str = realloc(line->prev->str, LINE_LEN_MIN*(sz/LINE_LEN_MIN+1));
        line->prev->max = LINE_LEN_MIN*(sz/LINE_LEN_MIN+1);
    }
    strcat(line->prev->str, line->str);
    line->prev->next =line->next;
    if (line->next)
        line->next->prev =line->prev;
    free(line->str);
    free(line);

    return 0;
}
int insert_nl(struct line *line, int pos){
    int rstrlen = strlen(line->str+pos);
    struct line *nl = malloc(sizeof(struct line));
    nl->prev = line;
    nl->next = line->next;
    nl->max = LINE_LEN_MIN*(rstrlen/LINE_LEN_MIN+1);
    
    nl->str = malloc(LINE_LEN_MIN*(rstrlen/LINE_LEN_MIN+1));

    strcpy(nl->str, line->str+pos);
    line->str[pos]=0;


    if (line->next)
        line->next->prev=nl;
    line->next = nl;

    return 0;
}
int append_nl(struct line *line){
    struct line *nl = malloc(sizeof(struct line));
    nl->prev = line;
    nl->next = line->next;
    nl->max = LINE_LEN_MIN;
    nl->str = malloc(LINE_LEN_MIN);
    nl->str[0]=0;

    if (line->next)
        line->next->prev=nl;
    line->next = nl;

    return 0;
}
