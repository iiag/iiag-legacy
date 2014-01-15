//
// display.h
//

#ifndef DISPLAY_H
#define DISPLAY_H

#include <ncurses.h>

void init_disp(void);
void memo(const char *, ...);

extern WINDOW * memoscr;
extern WINDOW * dispscr;
extern WINDOW * statscr;

#endif
