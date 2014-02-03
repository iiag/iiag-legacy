//
// display.h
//

#ifndef DISPLAY_H
#define DISPLAY_H

#include <ncurses.h>

void init_disp(void);
void end_disp(void);
void disp_put(int, int, chtype);
void reset_memos(void);
void memo(const char *, ...);
void statline(int, const char *, ...);

void scroll_disp(int, int);
void scroll_center(int, int);

extern WINDOW * memoscr;
extern WINDOW * dispscr;
extern WINDOW * statscr;

#endif
