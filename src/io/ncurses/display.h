//
// io/ncurses/display.h
//

#ifndef IO_NCURSES_DISPLAY_H
#define IO_NCURSES_DISPLAY_H

#include <stdio.h>
#include "../display.h"

#ifdef USE_NCURSES
	#include <ncurses.h>
#endif

void nc_init(FILE *);

#endif
