#include <ctype.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "line.h"
#include "editor_ctrl.h"
#include "editor.h"
#include "cmd.h"

int get_line_nw(struct editor_state * state){
    int last = state->line_nw;
    state->line_nw = ((int) log10(state->line_count))+2;
    return last != state->line_nw;
}

struct editor_state * init_editor(FILE *file, char filename[256], struct editor_state ** self){
    long file_sz =0;
    struct line *head = NULL;
    ESCDELAY=0;
    if (file){
        flockfile(file);
        fseek(file, 0, SEEK_END);
        file_sz = ftell(file);
        rewind(file);
        head = read_lines(file,file_sz, NULL);
        funlockfile(file);
        fclose(file);
    }else {
        head = malloc(sizeof(struct line));
        if (!head) {
            fputs("Failed to allocate memory", stderr);
            exit(1);
        }
        head->str = malloc(LINE_LEN_MIN);
        if (!head->str) {
            fputs("Failed to allocate memory", stderr);
            exit(1);
        }
        head->str[0] =0;
        head->max = LINE_LEN_MIN;
        head->prev = NULL;
        head->next = NULL;

    }

    initscr();
    cbreak();
    noecho();
    nl();
    keypad(stdscr,1);

    struct editor_state *state = malloc(sizeof(struct editor_state));
    if (!state){
        fputs("Failed to allocate memory", stderr);
        exit(1);
    }
    state->current_line = head;
    state->head = head;
    state->view =0;
    state->current_line_n =0;
    state->line_count =0;
    strncpy(state->filename, filename, 255);
    state->filename[255]=0;
    state->line_nw = 0;
    state->do_resize = 0;
    state->self = self;

    for (struct line *line = head; line; line=line->next)
        state->line_count++;
    get_line_nw(state); 
    state->win = newwin(getmaxy(stdscr)-1, getmaxx(stdscr)-state->line_nw, 0, state->line_nw);
    keypad(state->win,1);
    wmove(state->win, 0,0);
    paint(state);
    return state;
}

void destroy_editor(struct editor_state * state){
    destroy_line_buffer(state->head);
    delwin(state->win);
    endwin();
    free(state);
}
void paint(struct editor_state * state){
    if (state->do_resize){
        endwin();
        refresh();
        clear();
    }
    update_window(state); 
    refresh();
    wrefresh(state->win);
}
void add_line(struct editor_state * state, struct line * line, int row){
    char * cpy = strdup(line->str);
    char * tok;
    tok = strtok(cpy, "\t");
    wmove(state->win, row, 0);
    wclrtoeol(state->win);
    for (;;){
        if (!tok) break;
        mvwaddstr(state->win, row, chtocol(line, tok-cpy), tok);
        tok = strtok(NULL, "\t");
    }
    free(cpy);
}
void update_window(struct editor_state * state){ 
    int row;
    struct line * draw_line;

    move(0,0);
    clrtobot();
    wclrtobot(state->win);

    if (get_line_nw(state) || state->do_resize){
        wresize(state->win, LINES-1,COLS-state->line_nw );
        mvwin(state->win,0,state->line_nw);
        state->do_resize =0;
    }
    
    for(row = state->current_line_n, draw_line = state->current_line; row != state->view; row--, draw_line=draw_line->prev);
    for(row = 0; row < getmaxy(state->win) && draw_line; draw_line=draw_line->next, row++){
        //mvwaddstr(state->win, row, 0, draw_line->str);
        add_line(state, draw_line, row);
        mvprintw(row, 0, "%d", row+state->view+1);
    }
    for(;row<getmaxy(state->win);row++)
        mvaddch(row, 1, '~');
    wmove(state->win, state->current_line_n -state->view, 0);
}

void update_window_after(struct editor_state*state){
    int startx, starty;
    startx=getcurx(state->win);
    starty=getcury(state->win);
    int y = state->current_line_n - state->view;
    
    struct line *draw_line = state->current_line;
    if (get_line_nw(state)){
        wresize(state->win, LINES-1,COLS-state->line_nw );
        mvwin(state->win,0,state->line_nw);
        update_window(state);
        return;
    }
     move(y,0);
    clrtobot();
    wclrtobot(state->win);

   
    for(; y < getmaxy(state->win) && draw_line; draw_line=draw_line->next, y++){
        //mvwaddstr(state->win, y, 0, draw_line->str);
        add_line(state, draw_line, y);
        mvprintw(y, 0, "%d", y+state->view+1);
    }
    for(;y<getmaxy(state->win);y++)
        mvaddch(y, 1, '~');
 
    wmove(state->win, starty, startx);
}
void update_view(struct editor_state *state){
    if (state->current_line_n-state->view >= getmaxy(state->win)){
        state->view+= getmaxy(state->win);
    } else if(state->current_line_n < state->view){ 
        state->view-= getmaxy(state->win);
        if (state->view <0){
            state->view = 0;
        }
    }else {
        return;
    }
    update_window(state);
    refresh();
}
void editor(struct editor_state * state){
    int input;
    struct editor_state ** state_ref = state->self;
    char buff[128];
    buff[127]=0;
    while(1) {
        input = wgetch(state->win);
        switch (input){
            case 27: //ESC
                //TODO: make a function
                move(LINES-1, 0);//I dont know why getmaxy doesnt work here but LINES does
                clrtoeol();
                echo();
                getnstr(buff, 127);
                noecho();
                refresh();
                if (handle_cmd(buff, state)== 0) return; 
                state = *state_ref;
                move_curs(state, state->current_ch);
                break;
            case KEY_UP:
                goto_prev(state, state->current_ch);
                break;
            case KEY_DOWN:
                goto_next(state, state->current_ch);
                break;
            case KEY_RIGHT:
                move_curs(state, state->current_ch+1);
                break;
            case KEY_LEFT:
                move_curs(state, state->current_ch-1);
                break;
            case KEY_BACKSPACE:
            case 127:
                if (state->current_ch==0) {
                    edit_delete_prev_line(state);
                    break;
                }
                edit_delete_char(state, state->current_ch-1);
                break;
            case KEY_ENTER:
            case '\n':
                edit_insert_nl(state);
                break;
            case KEY_RESIZE:
                state->do_resize =1;
                paint(state);
                break;
            case KEY_STAB:
                input = '\t';
            default:
               edit_insert_char(state, input, state->current_ch);
               break;
       }
        wrefresh(state->win);
    }
}

