#ifndef LINE_H
#define LINE_H

#include <stddef.h>
#include <stdio.h>

#define LINE_LEN_MIN 50


struct line {
    char * str;
    size_t max;
    struct line * next;
    struct line * prev;
};


struct line * read_lines(FILE *, size_t, struct line *);

int insert_ch(struct line*, char, size_t);
int insert_str(struct line*, char*, size_t);
int del_ch(struct line * , size_t );
int del_nl(struct line *);
int insert_nl(struct line *, int);
int append_nl(struct line *);

#endif /* LINE_H */
