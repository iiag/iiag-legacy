//
// display.c
//

#include <stdarg.h>
#include "world.h"
#include "display.h"

static int max_width;
static int max_height;

WINDOW * memoscr;
WINDOW * dispscr;
WINDOW * statscr;

void init_disp(void)
{
	initscr();
	raw();
	noecho();
	keypad(stdscr, TRUE);
	getmaxyx(stdscr, max_height, max_width);

	// just hide the cursor for now
	curs_set(0);

	memoscr = newwin(1, max_width, 0, 0);
	dispscr = newwin(max_height - 4, max_width, 1, 0);
	statscr = newwin(3, max_width, max_height - 3, 0);
}

void end_disp(void)
{
	endwin();
}

void memo(const char * fmt, ...)
{
	va_list vl;
	va_start(vl, fmt);

	wmove(memoscr, 0, 0);
	wclrtoeol(memoscr);
	vw_printw(memoscr, fmt, vl);
	wrefresh(memoscr);

	va_end(vl);
}

