//
// io/ncurses/display.h
//

#ifndef IO_NCURSES_DISPLAY_H
#define IO_NCURSES_DISPLAY_H

#include <stdio.h>
#include "../display.h"

#ifdef WITH_NCURSES
	#include <ncurses.h>
#endif

void nc_init(int, FILE *);

#endif
