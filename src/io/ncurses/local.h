//
// io/ncurses/local.h
//

#ifndef IO_NCURSES_LOCAL_H
#define IO_NCURSES_LOCAL_H

#ifdef WITH_NCURSES
	#include <ncurses.h>

	extern WINDOW * memoscr;
	extern WINDOW * dispscr;
	extern WINDOW * statscr;
#endif

#endif
